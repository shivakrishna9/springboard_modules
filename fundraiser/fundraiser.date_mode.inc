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
      'If this checkbox is left unchecked, sustainers will instead be processed ' .
      'on the date of their anniversary. NOTE: Turning this feature on will convert ' .
      'current sustainers to the new date for all future (and only future) charges. '.
      'Turning this feature off thereafter will revert future charges to anniversary.'),
    '#access' => $queue_possible,
  );

  $dates = array_combine(range(1, 28), range(1, 28)); // Why 28? Feb.
  $form['fundraiser_sustainer_set_dates'] = array(
    '#type' => 'select',
    '#title' => t('Process date'),
    '#description' => t('Select the day of each month to process sustainers.'),
    '#options' => $dates,
    '#multiple' => FALSE,
    '#default_value' => variable_get('fundraiser_sustainer_set_dates', '15'),
    '#access' => $queue_possible,
  );

  $seconds = array_combine(range(1, 60), range(1, 60));
  $form['fundraiser_sustainer_set_seconds'] = array(
    '#type' => 'select',
    '#title' => t('Seconds spent'),
    '#description' => t('Set how many seconds for each drupal queue run to spend on the queue processing sustainer charges. '.
      'If drupal queue is running as part of drupal cron, leave this at about 30 seconds or less so it won\'t time out.' .
      'If drupal queue is running standalone through drush then this can be higher to process more sustainers in a single run.'),
    '#options' => $seconds,
    '#default_value' => variable_get('fundraiser_sustainer_set_seconds', 30),
    '#access' => $queue_possible,
  );

  $form['batch_warning'] = array(
    '#type' => 'markup',
    '#value' => '<div><p><strong>Warning:</strong>  Changing the settings ' .
      'will start off a batch process that needs to be completed. About ' .
      fundraiser_sustainer_date_mode_max_records() .
      ' records will need to be updated.  These changes will affect future scheduled charges. ' .
      'However this will not include any scheduled charges in the current month ' .
      'to prevent charges from being rescheduled into the past.' .
      'Beware that changing these settings repeatedly could have side effects.</p></div>',
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
function fundraiser_sustainer_date_mode_form_submit($form, &$form_state) {
  // Check and exit if needed.
  if (!module_exists('drupal_queue')) {
    drupal_set_message('Drupal Queue is not enabled, so no changes have been saved.');
    return;
  }

  // Handle modification of currently active sustainer charge dates.
  // To match the date formatting requested.
  $old_mode = variable_get('fundraiser_sustainer_set_date_mode', FALSE);
  $new_mode = $form_state['values']['fundraiser_sustainer_set_date_mode'];
  $old_date = variable_get('fundraiser_sustainer_set_dates', '15');
  $new_date = $form_state['values']['fundraiser_sustainer_set_dates'];

  // save the new values
  variable_set('fundraiser_sustainer_set_date_mode', $new_mode);
  variable_set('fundraiser_sustainer_set_dates', $new_date);
  variable_set('fundraiser_sustainer_set_seconds', $form_state['values']['fundraiser_sustainer_set_seconds']);

  // Check if we need to change anything.
  if ($old_mode == $new_mode && $old_date == $new_date) {
    drupal_set_message('No changes made.');
    return; // If we haven't changed any setting.
  }
  if ($old_mode == $new_mode && $new_mode == FALSE) {
    drupal_set_message('Day of the month has been changed, but it does not have an effect when date mode is disabled.');
    return; // If we're still in the same mode, but the mode doesn't need dates.
  }

  // Looks like we switched modes or dates, so we need to take action.

  // if we're switching to date mode from anniversary mode,
  // create the table to store the original charges.
  if ($old_mode == FALSE && $new_mode == TRUE) {
    fundraiser_save_date_mode_values();
    // after this, fall through to start the batch update.
  }
  // if we're switching from date mode to anniversary mode,
  // restore the original values only and drop the table
  elseif ($old_mode == TRUE && $new_mode == FALSE) {
    fundraiser_update_recurring_with_original_dates();

    // in this case we are not switching sustainers that get added after the switch to date mode
    // they will remain on the day of the month and not on anniversary of the original order
    drupal_set_message('Sustainers have been restored to their previous anniversary dates.');
    return;
  }

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

  $now = strtotime('now');
  $master_ids = fundraiser_get_master_order_ids_after($now);

  $max = count($master_ids);
  foreach ($master_ids as $master_id) {
    // we pass mode, date, and time to every operation call even though they don't change.
    // The reason is because we can't seem to be able to set $context['sandbox'] otherwise.
    $operations[] = array('fundraiser_sustainer_update_date_mode_operation', array($mode, $date, $time, $master_id, $max));
  }

  $batch = array(
    'operations' => $operations,
    'finished' => 'fundraiser_sustainer_update_date_mode_finished',
    // We can define custom messages instead of the default ones.
    'title' => t('Processing sustainers for date mode update'),
    'init_message' => t('Date mode update is starting.'),
    'progress_message' => t('Processed @current sustainer sets out of @total.'),
    'error_message' => t('The date mode update has encountered an error.'),
    'file' => drupal_get_path('module', 'fundraiser') . '/fundraiser.date_mode.inc',
  );

  return $batch;
}

/**
 * A batch operation for the date mode update.
 *
 * @param boolean $mode
 * @param integer $date The day of the month.
 * @param string $time The time to use for the next charges.
 * @param integer $master_id  The order ID of the master order.
 * @param integer $max  The number of serieses to process.
 * @param array $context  The batch variable that gets passed by reference between operation calls.
 */
function fundraiser_sustainer_update_date_mode_operation($mode, $date, $time, $master_id, $max, &$context) {

  // initialize some sandbox values
  if (!isset($context['sandbox']['progress'])) {
    // Use the $context['sandbox'] at your convenience to store the
    // information needed to track progression between successive calls.
    $context['sandbox']['mode'] = $mode;
    $context['sandbox']['date'] = $date;
    $context['sandbox']['time'] = $time;
    $context['sandbox']['now'] = strtotime('now');
    // count the number of sustainers to process
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = $max;
  }

  $orders = fundraiser_get_sustainer_series($master_id);
  $baseline_info = fundraiser_sustainer_get_baseline_info($orders);
  // if there's no legit baseline date, go to the next series.
  if ($baseline_info === FALSE) {
    // Update our progress information.
    $context['sandbox']['progress']++;
    return;
  }

  // start at the baseline sustainer, skipping the irrelevant ones.
  $start_at = $baseline_info['index'];
  $baseline_date = $baseline_info['date'];

  // change the day of the baseline date if date is on
  if ($mode) {
    $baseline_date = strtotime(date("Y-m-$date $time", $baseline_date));
  }
  // Otherwise use the day of the month that the master order was charged on.
  else {
    $date_of_master = fundraiser_sustainers_get_master_order_day_of_month($master_id);

    // note that strtotime will add the days to make this a valid date
    $baseline_date = strtotime(date("Y-m-$date_of_master H:i:s", $baseline_date));
  }

  // offset number of months from the baseline date
  $increment = 0;
  for ($i = $start_at; $i < count($orders); $i++) {
    $order = $orders[$i];

    $new_date = strtotime('+' . $increment . ' months', $baseline_date);
    $increment++;

    // skip past orders
    // shouldn't happen because $start_at should already be past them.
    if ($order['next_charge'] < $now) {
      continue;
    }

    // if the next charge is in this month, leave it alone.
    // This should prevent sliding charges into the past.
    if (date('Y-m', $order['next_charge']) == $this_year_month) {
      continue;
    }

    // skip future failed and canceled orders.
    if (in_array($order['gateway_resp'], array('failed', 'canceled'))) {
      continue;
    }

    // no change, skip it
    if ($order['next_charge'] == $new_date) {
      continue;
    }

    // do the update
    db_query('UPDATE {fundraiser_recurring} ' .
      'SET next_charge = %d ' .
      'WHERE order_id = %d', $new_date, $order['order_id']);

    // update the order in salesforce
    if (module_exists('sf_donation')) {
      sf_queue_insert($order['order_id'], FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
    }

    $message = 'Updated order '. $order['order_id'] .
      ' (master order ' . $master_id . ')' .
      ' from ' .
      date('Y-m-d', $order['next_charge']) .
      ' to ' . date('Y-m-d', $new_date);

    $replacements = array(
      '!from' => date('n/j/Y', $order['next_charge']),
      '!to' => date('n/j/Y', $new_date)
    );

    uc_order_comment_save($order['order_id'], 0, t('Payment charge date changed from !from to !to.', $replacements), 'admin');

    watchdog('fundraiser_sustainers', $message);
    // Store some result for post-processing in the finished callback.
    $context['results'][] = $message;
  }

  // Update our progress information.
  $context['sandbox']['progress']++;

  $context['message'] = 'Master Order ID ' . $master_id;
}

/**
 * Batch 'finished' callback used by update date mode.
 */
function fundraiser_sustainer_update_date_mode_finished($success, $results, $operations) {

  if ($success) {
    // display the number of sustainers processed and the result messages.
    drupal_set_message('Finished updating the existing sustainers to the new date. ' . count($results) . ' sustainers processed.');
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message('An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE));
  }

}

/**
 * Returns the charge day of the month of the master order.
 *
 * Used for switching to anniversary mode.
 *
 * @param integer $master_id
 *
 * @return bool|string
 */
function fundraiser_sustainers_get_master_order_day_of_month($master_id) {
  return date('d', db_result(db_query(
        'SELECT created FROM {uc_orders} ' .
        'WHERE order_id = %d', $master_id
      )));
}


/**
 * Returns an array of master order IDs for serieses that have future orders.
 *
 * @param integer $timestamp
 *
 * @return array
 */
function fundraiser_get_master_order_ids_after($timestamp) {
  $return = array();

  $results = db_query("SELECT DISTINCT master_order_id FROM {fundraiser_recurring}
    WHERE next_charge > %d
    ORDER BY master_order_id ASC, next_charge ASC",
    $timestamp
  );

  while ($row = db_fetch_array($results)) {
    $return[] = $row['master_order_id'];
  }

  return $return;
}

/**
 * Returns info on all orders in a sustainer series.
 *
 * The 0th element is the master order.
 *
 * @param integer $master_order_id
 * @return array
 */
function fundraiser_get_sustainer_series($master_order_id) {
  $return = array();

  $return[0] = db_fetch_array(
    db_query("SELECT order_id, created AS next_charge, modified
    FROM {uc_orders} WHERE order_id = %d", $master_order_id)
  );

  // Assume the master order succeeded.
  $return[0]['gateway_resp'] = 'success';
  $return[0]['attempts'] = 0;

  $results = db_query("SELECT order_id, next_charge, gateway_resp, attempts
    FROM {fundraiser_recurring} WHERE master_order_id = %d ORDER BY order_id ASC",
    $master_order_id
  );

  while ($row = db_fetch_array($results)) {
    $return[] = $row;
  }

  return $return;
}

/**
 * Gets the array index and date of a baseline to calculate future orders from.
 *
 * Ignores the master order, past orders, canceled or failed orders.
 *
 * @param array $orders  An array of orders with at least 'next_charge' and 'gateway_resp' keys.
 *
 * @return array|bool
 */
function fundraiser_sustainer_get_baseline_info($orders) {
  $now = strtotime('now');
  foreach ($orders as $i => $order) {
    // element 0 is the master order
    // element 1 is the first sustainer
    // skip the master order.
    // skip past orders
    // skip future failed and canceled orders.
    if ($i == 0 ||
      $order['next_charge'] < $now ||
      in_array($order['gateway_resp'], array('failed', 'canceled'))
    ) {
      continue;
    }

    // after passing that criteria, the first order in the sequence is the baseline.
    return array(
      'index' => $i,
      'date' => $order['next_charge'],
    );

  }
  return FALSE;
}

/**
 * Empties the date mode table and copies the next_charge values
 * to the date mode table.
 */
function fundraiser_save_date_mode_values() {
  db_query("TRUNCATE TABLE {fundraiser_recurring_date_mode}");
  db_query("INSERT INTO {fundraiser_recurring_date_mode} (order_id, original_next_charge)
    (
    SELECT order_id, next_charge FROM fundraiser_recurring
    WHERE from_unixtime(next_charge) > now() AND gateway_resp IS NULL
    )"
  );
}

/**
 * Restores the originally changed values of next_charge.
 */
function fundraiser_update_recurring_with_original_dates() {
  db_query("UPDATE {fundraiser_recurring} AS r
    INNER JOIN {fundraiser_recurring_date_mode} AS d ON r.order_id = d.order_id
    SET r.next_charge = d.original_next_charge
    WHERE from_unixtime(r.next_charge) > now() AND r.gateway_resp IS NULL"
  );
}
