<?php

/**
 * @file
 * Admin settings form and associated functions.
 */

function fundraiser_date_mode_admin() {
  $form['fundraiser_date_mode_set_date_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set date mode'),
    '#default_value' => variable_get('fundraiser_date_mode_set_date_mode', FALSE),
    '#description' => t('Process all sustainer charges on a set date each month. ' .
      'If this checkbox is left unchecked, sustainers will instead be processed ' .
      'on the date of their anniversary. NOTE: Turning this feature on will convert ' .
      'current sustainers to the new date for all future (and only future) charges. '.
      'Turning this feature off thereafter will revert future charges to anniversary.'),
  );

  $dates = array_combine(range(1, 28), range(1, 28)); // Why 28? Feb.
  $form['fundraiser_date_mode_set_dates'] = array(
    '#type' => 'select',
    '#title' => t('Process date'),
    '#description' => t('Select the day of each month to process sustainers.'),
    '#options' => $dates,
    '#multiple' => FALSE,
    '#default_value' => variable_get('fundraiser_date_mode_set_dates', '15'),
  );

  $seconds = array_combine(range(1, 60), range(1, 60));
  $form['fundraiser_date_mode_set_seconds'] = array(
    '#type' => 'select',
    '#title' => t('Seconds spent'),
    '#description' => t('Set how many seconds for each drupal queue run to spend on the queue processing sustainer charges. '.
      'If drupal queue is running as part of drupal cron, leave this at about 30 seconds or less so it won\'t time out.' .
      'If drupal queue is running standalone through drush then this can be higher to process more sustainers in a single run.'),
    '#options' => $seconds,
    '#default_value' => variable_get('fundraiser_date_mode_set_seconds', 30),
  );

  $form['batch_warning'] = array(
    '#type' => 'markup',
    '#markup' => '<div><p><strong>Warning:</strong>  Changing the settings ' .
      'will start off a batch process that needs to be completed. About ' .
      fundraiser_date_mode_max_records() .
      ' records will need to be updated.  These changes will affect future scheduled charges. ' .
      'However this will not include any scheduled charges in the current month ' .
      'to prevent charges from being rescheduled into the past.' .
      'Beware that changing these settings repeatedly could have side effects.</p></div>',
  );
  $form['#submit'][] = 'fundraiser_date_mode_admin_submit';
  return system_settings_form($form);
}

function fundraiser_date_mode_admin_submit($form, $form_state) {
  $date_mode_setting = variable_get('fundraiser_date_mode_set_date_mode', FALSE);
  $date_setting = variable_get('fundraiser_date_mode_set_dates', '15');

  $mode_changed = !($date_mode_setting == $form_state['values']['fundraiser_date_mode_set_date_mode']);
  $date_changed = !($date_setting == $form_state['values']['fundraiser_date_mode_set_dates']);
  if ($mode_changed && $form_state['values']['fundraiser_date_mode_set_date_mode']) {
    $op = 'date_mode_enabled';
  }
  elseif ($mode_changed && !$form_state['values']['fundraiser_date_mode_set_date_mode']) {
    $op = 'date_mode_disabled';
  }
  elseif ($date_changed && $form_state['values']['fundraiser_date_mode_set_date_mode']) {
    $op = 'date_mode_date_changed';
  }

  // Date mode setting has changed or date mode is enabled and the process date has changed.
  if ($mode_changed || ($form_state['values']['fundraiser_date_mode_set_date_mode'] && $date_changed)) {
    $batch = fundraiser_date_mode_update_date_mode_batch($op, $form_state['values']['fundraiser_date_mode_set_date_mode'], $form_state['values']['fundraiser_date_mode_set_dates']);
    batch_set($batch);

  }
}

/**
 *
 * @param $mode
 * @param $date
 * @return array
 */
function fundraiser_date_mode_update_date_mode_batch($op, $date) {
  $time = '01:01:01'; // One minute, one second after 1am.
  $operations = array();
  $now = time();
  $series_data = fundraiser_date_mode_get_sustainer_series_data();
  $master_ids = array_keys($series_data);
  $max = count($master_ids);
  foreach ($master_ids as $master_id) {
    // we pass mode, date, and time to every operation call even though they don't change.
    $operations[] = array('fundraiser_date_mode_update_date_mode_operation', array($op, $date, $time, $master_id, $max, $series_data[$master_id]));
  }

  $batch = array(
    'operations' => $operations,
    'finished' => 'fundraiser_date_mode_update_date_mode_finished',
    'title' => t('Processing sustainers for date mode update'),
    'init_message' => t('Date mode update is starting.'),
    'progress_message' => t('Processed @current sustainer sets out of @total.'),
    'error_message' => t('The date mode update has encountered an error.'),
  );continue
  return $batch;
}

function fundraiser_date_mode_update_date_mode_operation($op, $date, $time, $master_did, $max, $data) {
  // initialize some sandbox values
  if (!isset($context['sandbox']['progress'])) {
    // Use the $context['sandbox'] at your convenience to store the
    // information needed to track progression between successive calls.
    $context['sandbox']['op'] = $op;
    $context['sandbox']['date'] = $date;
    $context['sandbox']['now'] = strtotime('now');
    // count the number of sustainers to process
    $context['sandbox']['progress'] = 0;
    $contents['sandbox']['data'] = $data;
  }
  $master_order = $data['master_order'];
  $series_orders = $data['series'];
  drupal_set_message('original next charge:' . date('m/d/Y G:i:s', $master_order['next_charge']));
  drupal_set_message('new day:' . $date);

  switch ($op) {
    case 'date_mode_enabled':
      // Store existing next_charge dates in case date mode is disabled. This
      // guarantees we have the original charge dates when rolling back changes
      // made when date mode is enabled.
      fundraiser_date_mode_save_series_charge_dates($master_did);
      _fundraiser_date_mode_alter_next_charge($date, $master_order, $series_orders);
      break;
    case 'date_mode_disabled':
      // Revert to original dates.
     _fundraiser_date_mode_revert_sustainer_dates($master_order, $series_orders);
      break;
    case 'date_mode_date_changed':
      // modify converted dates
      _fundraiser_date_mode_alter_next_charge($date, $master_order, $series_orders);
      break;
    default:
      return;
  }

  $time_delta = _fundraiser_date_mode_calculate_delta($master_order['next_charge'], $date);
  drupal_set_message('delta:' . $time_delta/86400);
  if ($time_delta == 0) {
   $time_delta = _fundraiser_date_mode_calculate_delta($series_orders[0]['next_charge'], $date);
    drupal_set_message('delta recalculated:' . $time_delta/86400);
  }

  // 86400 seconds in a day.
  return;

  // offset number of months from the baseline date
  $increment = 0;
  for ($i = 0; $i < count($series_orders); $i++) {
    $order = $series_orders[$i];
    $now = time();
    $current_time = _fundraiser_date_mode_explode_date(time());
    $order_time = _fundraiser_date_mode_explode_date($order['next_charge']);

    // Date mode enabled, calculate a new next_charge date for the order.
    if ($mode) {
      $datestring = $order_time['month'] . '/' . $date . '/' . $order_time['year'];
      $datestring .= ' ' . $order_time['hour'] . ':' . $order_time['minute'] . ':' . $order_time['second'];
      $new_date = strtotime($datestring);
      drupal_set_message('original date:' . date('m/d/Y G:i:s', $order['next_charge']) . ' new date:' . date('m/d/Y G:i:s', $new_date));
    }
    else {

    }
    return;

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

function fundraiser_date_mode_update_date_mode_finised() {
  return t('all done');
}



function _fundraiser_date_mode_alter_next_charge($date, $master_order, $series_orders) {
  $orders_to_process = _fundraiser_date_mode_orders_to_change($series_orders, $date);

  $master_order_date = _fundraiser_date_mode_explode_date($master_order['next_charge']);
  $orders_on_same_day = _fundraiser_date_mode_validate_order_dates($orders_to_process, $master_order_date['day']);

  if (count($orders_to_process) == 0) {
    return;
  }
  else {
    // Some of the sustainer series next_charge dates need to be
    // moved to the day sustainers will be processed on. They all currently
    // fall on the same day of the month so they can be updated in bulk.
    if  ($orders_on_same_day) {
      // Do mass date conversion.
      $time_delta = _fundraiser_date_mode_calculate_delta($master_order['next_charge'], $date);
      fundraiser_date_mode_update_sustainer_date($time_delta, $master_order['master_did'], $orders_to_process);
    }
    else {
      // Some of the next_charge dates in this series are on different days
      // of the month. This means we have to calculate time_delta for orders
      // individually to make sure they all get updated correctly.
      // TODO: add max to context to prevent partial conversions.
      foreach ($series_orders as $order) {
        $time_delta = _fundraiser_date_mode_calculate_delta($order['next_charge'], $date);
        fundraiser_date_mode_update_sustainer_date($time_delta, $order['master_did'], $order['did']);
      }
    }
  }
}




