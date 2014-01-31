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

  // Date mode setting has changed or date mode is enabled and the process date has changed.
  if ($mode_changed || ($form_state['values']['fundraiser_date_mode_set_date_mode'] && $date_changed)) {
    drupal_set_message('date mode settings changed, batch em up');
    $batch = fundraiser_date_mode_update_date_mode_batch($form_state['values']['fundraiser_date_mode_set_date_mode'], $form_state['values']['fundraiser_date_mode_set_dates']);
    batch_set($batch);

  }
}

/**
 *
 * @param $mode
 * @param $date
 * @return array
 */
function fundraiser_date_mode_update_date_mode_batch($mode, $date) {
  $time = '01:01:01'; // One minute, one second after 1am.
  $operations = array();
  $now = strtotime('now');
  $series_data = fundraiser_date_mode_get_sustainer_series_data();
  $master_ids = array_keys($series_data);
  $max = count($master_ids);
  foreach ($master_ids as $master_id) {
    // we pass mode, date, and time to every operation call even though they don't change.
    // The reason is because we cana't seem to be able to set $context['sandbox'] otherwise.
    $operations[] = array('fundraiser_date_mode_update_date_mode_operation', array($mode, $date, $time, $master_id, $max, $series_data[$master_id]));
  }

  $batch = array(
    'operations' => $operations,
    'finished' => 'fundraiser_date_mode_update_date_mode_finished',
    // We can define custom messages instead of the default ones.
    'title' => t('Processing sustainers for date mode update'),
    'init_message' => t('Date mode update is starting.'),
    'progress_message' => t('Processed @current sustainer sets out of @total.'),
    'error_message' => t('The date mode update has encountered an error.'),
  );
  return $batch;
}

function fundraiser_date_mode_update_date_mode_operation($mode, $date, $time, $master_did, $max, $data) {
  $master_order = $data['master_order'];
  $series_orders = $data['series'];
  drupal_set_message('original next charge:' . date('m/d/Y G:i:s', $master_order['next_charge']));
  drupal_set_message('new day:' . $date);
  $time_delta = _fundraiser_date_mode_calculate_delta($mode, $master_order['next_charge'], $date);
  drupal_set_message('time delta:' . $time_delta);
  $orders_to_process = _fundraiser_date_mode_orders_to_change($series_orders, $time_delta);
  dsm($orders_to_process, 'orders to process');
  fundraiser_date_mode_update_sustainer_dates($time_delta, $master_did, array_keys($orders_to_process));
  // 86400 seconds in a day.
  return;


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
    $contents['sandbox']['data'] = $data;
  }

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

      //$original_date = fundraiser_date_mode_get_original_next_charge($order['did']);
    }
    return;
    // skip past orders
    if ($order['next_charge'] < $now) {
      continue;
    }

    // if the next charge is in this month, leave it alone.
    // This should prevent sliding charges into the past.
    if ($order_time['year'] == $current_time['year'] && $order_time['month'] == $current_time['month']) {
      continue;
    }

    // Skip future failed and canceled orders.
    // This also shouldn't happen, $data should only contain orders with
    // NULL gateway_resp.
    if (in_array($order['gateway_resp'], array('failed', 'canceled'))) {
      continue;
    }

    // No change, skip it.
    if ($order['next_charge'] == $new_date) {
      continue;
    }

    // do the update
    fundraiser_date_mode_update_sustainer_date($timestamo, $order_id);
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

function fundraiser_date_mode_update_date_mode_finised() {
  return t('all done');
}


function fundraiser_date_mode_get_sustainer_series_data($master_did) {
  $results = db_query('
    SELECT *
    FROM {fundraiser_sustainers}
    WHERE
      master_did = :master_did
  ', array(':master_did' => $master_did));

  while ($row = $results->fetchAssoc()) {
    if ($row['master_did'] == $row['did']) {
      $data[$row['master_did']]['master_order'] = $row;
    }
    else {
      if ($row['next_charge'] > time() && empty($row['gateway_resp'])) {
        $data[$row['master_did']]['series'][] = $row;
      }
    }
  }
  return $data;
}
/*
 *     AND
      did != master_did
    AND
      next_charge > UNIX_TIMESTAMP(NOW())
    AND
      gateway_resp IS NULL
 */

function fundraiser_date_mode_update_sustainer_dates($time_delta, $master_did, $dids) {
  $operator = $time_delta < 0 ? '-' : '+';
  $query = db_update('fundraiser_sustainers') // Table name no longer needs {}
    ->expression('next_charge', 'next_charge ' . $operator . ' :delta', array(':delta' => $time_delta))
    ->condition('master_did', $master_did, '=')
    ->condition('did', $dids, 'IN');
  dsm($query->__toString(), 'update query');
  dsm($query->arguments(), 'arguments');
  $query->execute();
}

function fudnraiser_date_mode_save_original_date($master_did, $did, $timestamp) {

}

function _fundraiser_date_mode_explode_date($timestamp) {
  $date_string = date('Y/n/j/G/i/s', $timestamp);
  $date = explode('/', $date_string);
  $time = array(
    'year' => $date[0],
    'month' => $date[1],
    'day' => $date[2],
    'hour' => $date[3],
    'minute' => $date[4],
    'second' => $date[5],
  );
  return $time;
}

function _fundraiser_date_mode_orders_to_change($orders, $time_delta) {
  $selected_orders = array();
  $current_time = _fundraiser_date_mode_explode_date(time());

  foreach ($orders as $order) {
    $order_time = _fundraiser_date_mode_explode_date($order['next_charge']);
    // Ignore orders that are ready to process.
    if ($order['next_charge'] < $current_time) {
      continue;
    }
    // if the next charge is in this month, leave it alone.
    // This should prevent sliding charges into the past.
    if ($order_time['year'] == $current_time['year'] && $order_time['month'] == $current_time['month']) {
      continue;
    }

    // Skip future failed and canceled orders.
    // This also shouldn't happen, $data should only contain orders with
    // NULL gateway_resp.
    if (in_array($order['gateway_resp'], array('failed', 'canceled'))) {
      continue;
    }
    // No change, skip it.
    if ($order['next_charge'] == $new_date) {
      continue;
    }
    $selected_orders[$order['did']] = $order;
  }
  return $orders;
}

function _fundraiser_date_mode_calculate_delta($mode, $charge_time, $date) {
  $original_charge_date = _fundraiser_date_mode_explode_date($charge_time);
  // 86400 seconds in 24 hours.
  $delta = ($original_charge_date['day'] - $date) * 86400;
  return $delta;
}