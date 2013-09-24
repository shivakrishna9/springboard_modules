<?php
/**
 * @file
 * Includes settings form and handlers for batch changing sustainer date mode settings.
 */

/**
 * Form handler for the date mode settings.
 */
function fundraiser_sustainer_date_mode_form() {
  $form = array();

  $queue_possible = FALSE;
  if (module_exists('drupal_queue')) {
    $queue_possible = TRUE;
  }
  else {
    $form['fundraiser_sustainer_set_date_mode_help'] = array(
        '#type' => 'markup',
        '#value' => '<div>'. t('Sustainer date mode scheduling is only available with Drupal Queue installed.') .'</div>',
    );

    return $form;
  }

  $form['fundraiser_sustainer_set_date_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set date mode'),
    '#default_value' => variable_get('fundraiser_sustainer_set_date_mode', FALSE),
    '#description' => t('Process all sustainer charges on a set date each month. ' .
      'If this checkbox is left unmarked, sustainers will instead be processed ' .
      'on the date of their anniversary. NOTE: Turning this feature on will convert ' .
      'current sustainers to the new date for all future (and only future) charges. '.
      'Turning this feature off thereafter will revert future charges to anniversary.'),
    '#access' => $queue_possible,
  );

  $dates = array_combine(range(1,28), range(1,28)); // Why 28? Feb.
  $form['fundraiser_sustainer_set_dates'] = array(
    '#type' => 'select',
    '#title' => t('Process date'),
    '#description' => t('Select the day of each month to process sustainers.'),
    '#options' => $dates,
    '#multiple' => FALSE, // TODO set true.
    '#default_value' => variable_get('fundraiser_sustainer_set_dates', array('15')),
    '#access' => $queue_possible,
  );

  $seconds = array_combine(range(1,60), range(1,60));
  $form['sustainer']['fundraiser_sustainer_set_seconds'] = array(
    '#type' => 'select',
    '#title' => t('Seconds spent'),
    '#description' => t('Set how many seconds for each cron to spend on the queue processing sustainer charges. '.
      'On average a charge takes about a second to process depending on the responsiveness of the gateway.'),
    '#options' => $seconds,
    '#default_value' => variable_get('fundraiser_sustainer_set_seconds', 30),
    '#access' => $queue_possible,
  );

  $form['batch_warning'] = array(
    '#type' => 'markup',
    '#value' => '<div><p><strong>Warning:</strong>  Changing the settings will start off a batch process that needs to be completed. At least ' .
      fundraiser_sustainer_date_mode_max_records() . ' records will need to be updated.</p></div>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

/**
 * Submit handler for the date mode settings.
 */
function fundraiser_sustaienr_date_mode_form_submit($form, &$form_state) {
  // Check and exit if needed.
  if (!module_exists('drupal_queue')) {
    drupal_set_message('Drupal Queue is not enabled, so no changes have been saved.');
    return;
  }

  // Handle modification of currently active sustainer charge dates.
  // To match the date formatting requested.
  $old_mode = variable_get('fundraiser_sustainer_set_date_mode', FALSE);
  $new_mode = $form_state['values']['fundraiser_sustainer_set_date_mode'];
  $old_date = variable_get('fundraiser_sustainer_set_dates', array('15'));
  $new_date = $form_state['values']['fundraiser_sustainer_set_dates'];

  // save the new values
  variable_set('fundraiser_sustainer_set_date_mode', $new_mode);
  variable_set('fundraiser_sustainer_set_dates', $new_date);

  // TODO: this will be changed when we support more than one date.
  // Check if we need to change anything.
  if ($old_mode == $new_mode && $old_date == $new_date) {
    drupal_set_message('No changes made.');
    return; // If we haven't changed any setting.
  }
  if ($old_mode == $new_mode && $new_mode == FALSE) {
    drupal_set_message('Day of the month has been changed, but it does not have an effect when date mode is disabled.');
    return; // If we're still in the same mode, but the mode doesn't need dates.
  }

  // Looks like we switched modes or dates, so we need to update all the future charges.

  // Start the batch process running
  batch_set(fundraiser_sustainer_update_date_mode_batch($new_mode, $new_date));

}

/**
 * Estimate of how many sustainer rows might be affected by a date mode changes.
 *
 * Estimate will most likely be higher than the actual number.
 *
 * @return integer|FALSE  The number of future sustainer records.
 */
function fundraiser_sustainer_date_mode_max_records() {
  return db_result(db_query('SELECT COUNT(master_order_id) FROM {fundraiser_recurring} ' .
      'WHERE next_charge > UNIX_TIMESTAMP(NOW()) AND gateway_resp IS NULL'));
}

/**
 * Creates the batch array for the update date mode job.
 *
 * This method does not yet do the work; it simply builds an array
 * of the work which needs to be done during the batch processing.
 * The results of this function will be passed to the batch
 * processor for actual processing.
 *
 * @param boolean $mode  The date mode setting.
 * @param integer $date  The numerical day of the month to set the charge date.
 */
function fundraiser_sustainer_update_date_mode_batch($mode, $date) {
  $time = '01:01:01'; // One minute, one second after 1am.
  $operations = array();

  $future_orders = db_query('SELECT master_order_id, order_id, next_charge FROM {fundraiser_recurring} ' .
    'WHERE next_charge > UNIX_TIMESTAMP(NOW())'); // Only the future ones.

  while ($order_info = db_fetch_array($future_orders)) {
    // we pass mode, date, and time to every operation call even though they don't change.
    // The reason is because we can't seem to be able to set $context['sandbox'] otherwise.
    $operations[] = array('fundraiser_sustainer_update_date_mode_operation', array($mode, $date, $time, $order_info));
  }

  $batch = array(
    'operations' => $operations,
    'finished' => 'fundraiser_sustainer_update_date_mode_finished',
    // We can define custom messages instead of the default ones.
    'title' => t('Processing sustainers for date mode update'),
    'init_message' => t('Date mode update is starting.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('The date mode update has encountered an error.'),
    'file' => drupal_get_path('module', 'fundraiser') . '/fundraiser.admin.inc',
  );

  return $batch;
}

/**
 * A batch operation for the date mode update.
 *
 * @param boolean $mode
 * @param integer $date The day of the month.
 * @param string The time to use for the next charges.
 * @param array $order_info  Includes order_id, master_order_id, and next_charge
 * @param array $context  The batch variable that gets passed by reference between operation calls.
 */
function fundraiser_sustainer_update_date_mode_operation($mode, $date, $time, $order_info, &$context) {

  // initialize some sandbox values
  if (!isset($context['sandbox']['progress'])) {
    // Use the $context['sandbox'] at your convenience to store the
    // information needed to track progression between successive calls.
    $context['sandbox']['mode'] = $mode;
    $context['sandbox']['date'] = $date;
    $context['sandbox']['time'] = $time;
    $context['sandbox']['now'] = strtotime('now');
    $context['sandbox']['progress'] = 0;
    // count the number of sustainers to process
    $context['sandbox']['max'] = fundraiser_sustainer_date_mode_max_records();
    $context['sandbox']['master_orders'] = array();
  }

  // Set them all to match the updated charge date.
  if ($context['sandbox']['mode'] == FALSE) {
    // Load the master order if it's not already been loaded to get the original anniversary.
    if (!in_array($order_info['master_order_id'], array_keys($context['sandbox']['master_orders']))) {
      $result = db_query('SELECT created FROM {uc_orders} ' .
        'WHERE order_id = %d', $order_info['master_order_id']);
      while ($master_order_info = db_fetch_array($result)) {
        $context['sandbox']['master_orders'][ $order_info['master_order_id'] ]  = $master_order_info['created'];
      }
    }
    $date = date('j', $context['sandbox']['master_orders'][ $order_info['master_order_id'] ]);
    $time = date('H:i:s', $context['sandbox']['master_orders'][ $order_info['master_order_id'] ]); // The original time.
  }
  else {
    $date = $context['sandbox']['date'];
    $time = $context['sandbox']['time'];
  }

  // Set all to the date set, by sliding forward from today.
  $base_date = $order_info['next_charge'];
  $this_month_year = date('F Y', $base_date); // Use the month and date of the next charge.
  $set_charge_date = strtotime($date . ' ' . $this_month_year . ' '. $time);
  // If our current date is already past the given charge date, then this new charge date
  // Will be in the past. We cannot kick it forward by one, as we'll double up on next month.
  // So we need to just drop this one in the series. We can't just move it forward to the end
  // of the series as the series is capped by expiration. This spare order just won't be processed.
  // This is such an edge case I don't think it will be a concern.
  if ($set_charge_date >= $now) {
    //drupal_set_message('Updated order #'. $order_info['order_id'] . ' from date ' .  date('d F Y', $base_date) .
    //  ' to date ' . date('d F Y', $set_charge_date) . '.');
    // Now that we know the correct new date for this order, update the table.
    db_query('UPDATE {fundraiser_recurring} ' .
      'SET next_charge = %d ' .
      'WHERE order_id = %d', $set_charge_date, $order_info['order_id']);

    $message = 'Updated order #'. $order_info['order_id'] . ' from date ' .  date('d F Y', $base_date) .
      ' to date ' . date('d F Y', $set_charge_date) . '.';

  }
  else {
    // We need to cancel this order, as it's not available anymore.
    global $user;

    $reason = 'Cancelled during switch from set-date queue to anniversary queue, date had passed.';
    $messaeg = 'Order canceled by an administrator. Reason: ' . $reason;

    db_query('UPDATE {fundraiser_recurring} SET gateway_resp = \'canceled\', cancellation_reason = \'%s\' ' .
      'WHERE order_id = %d AND (gateway_resp IS NULL OR gateway_resp = \'failed\')', $reason, $order_info['order_id']);
    uc_order_comment_save($order_info['order_id'], $user->uid, $message, 'order', 'canceled', FALSE);
  }

  watchdog('fundraiser', $message);

  // Store some result for post-processing in the finished callback.
  $context['results'][] = $message;

  // Update our progress information.
  $context['sandbox']['progress']++;

  $context['message'] = 'Order ID ' . $order_info['order_id'];
}

/**
 * Batch 'finished' callback used by update date mode.
 */
function fundraiser_sustainer_update_date_mode_finished($success, $results, $operations) {

  if ($success) {
    // display the number of sustainers processed and the result messages.
    drupal_set_message('Finished updating the existing sustainers to the new date. ' . count($results) . ' sustainers processed.');

    foreach ($results as $result) {
      drupal_set_message($result);
    }

  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message('An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE));
  }

}
