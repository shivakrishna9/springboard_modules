<?php

/**
 * @file admin form definitions and associated helper functions
 */

/**
 * Admin settings form.
 */
function sustainer_import_admin() {

  $form['sustainer_import_queue_process_record_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Import queue process record count'),
    '#description' => t('The number of records in the import queue to process when the import processor is run'),
    '#default_value' => variable_get('sustainer_import_queue_process_record_count', 100),
  );
  return system_settings_form($form);
}

/**
 * Sustainer records import form.
 *
 * Upload sustainer records and queue for processing.
 */
function sustainer_import_import_page() {
  $form['queue_report'] = array(
    '#type' => 'markup',
    '#value' => '',
  );
  // import reporting

  // file import UI
  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload import file'),
  );
   $form['upload']['import_file'] = array(
     '#type' => 'file',
     '#title' => t('File'),
     '#description' => t('Current supported formats are discussed in the module documentation. Allowed file extensions: .txt'),
   );
   $form['upload']['submit'] = array(
     '#type' => 'submit',
     '#value' => t('Upload'),
   );
   $form['#validate'][] = 'sustainer_import_upload_validate';
   $form['#submit'][] = 'sustainer_import_upload_submit';
   return $form;
}

/**
 * Validate import file uploads
 */
function sustainer_import_upload_validate(&$form, $form_state) {

  $file = file_save_upload('import_file', array(
    'file_validate_extensions' => array('txt'), // Validate extensions.
  ));

  // If the file passed validation:
  if ($file) {
    // Move the file, into the Drupal file system
    $dir = "private://sustainer_import";
    if (file_prepare_directory($dir, FILE_CREATE_DIRECTORY)) {
      if ($file = file_move($file, $dir, FILE_EXISTS_RENAME)) {
        // parse that bad boy
        _sustainer_import_parse_file($file);
      }
      else {
        form_set_error('import_file', t('Unable to write the uploaded file to the private file directory. Please check the file system configuration.'));
      }
    }
  }
  else {
    form_set_error('import_file', t('File is missing or the file extension is not allowed.'));
  }
}

/**
 * Submit import file uploads
 */
function sustainer_import_upload_submit($form, $form_state) {

}

function _sustainer_import_parse_file($file) {
  if ($file->uri) {
    $input = array();
    $infile = fopen($file->uri, 'r');
    while ($line = fgets($infile)) {
      $data = @unserialize($line);
      // we want to skip any null records caused by extraneous newlines.
      if (is_array($data)) {
        $input[] = $data[0];
      }
    }
    // We're manually handling serialization issues here so we want to suppress
    // the natural output of unserialize() in the event of an error.
    $record_count = 0;
    if (is_array($input)) {
      foreach($input as $order_detail) {
        // inject into db table
        sustainer_import_import_record_insert($order_detail);
        ++$record_count;
      }
    }
    else {
      drupal_set_message('Uploaded file is in an incorrect format or contains errors', 'error');
      return FALSE;
    }
    drupal_set_message(t('!record_count records have been added to the import queue.', array('!record_count' => $record_count)));
  }
  else {

  }
}

/**
 * Report on current items in import queue.
 * Run import queue.
 */
function sustainer_import_queue_report() {

}

/**
 * Edit record in import queue.
 */
function sustainer_import_edit_queue_item($form, $form_state, $record) {
  $payment_schedule = array();
  $form = array();

  // stash record values that don't change
  $form['master_order_id'] = array('#type' => 'value', '#value' => $record['master_order_id']);
  $form['rid'] = array('#type' => 'value', '#value' => $record['rid']);
  $form['created'] = array('#type' => 'value', '#value' => $record['record']['created']);

  // top level details about the record
  $form['master_order_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Master order details'),
  );
  $form['master_order_details']['master_salesforce_opportunity_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Salesforce Opportunity ID'),
    '#default_value' => !empty($record['record']['salesforce_opportunity_id']) ? $record['record']['salesforce_opportunity_id'] : '',
    '#required' => TRUE,
  );
  $form['master_order_details']['salesforce_recurring_donation_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Salesforce Recurring Donation ID'),
    '#default_value' => !empty($record['record']['salesforce_recurring_donation_id']) ? $record['record']['salesforce_recurring_donation_id'] : '',
    '#required' => TRUE,
  );
  $form['master_order_details']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount'),
    '#default_value' => !empty($record['record']['amount']) ? $record['record']['amount'] : '',
    '#required' => TRUE,
  );

  // user account details
  $form['user_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('User info'),
  );
  $form['user_info']['sf_contact_id'] = array(
    '#type' => 'textfield',
    '#title' => t('SF contact id'),
    '#default_value' => !empty($record['record']['user_info']['sf_contact_id']) ? $record['record']['user_info']['sf_contact_id'] : '',
    '#required' => TRUE,
  );
  $form['user_info']['sf_account_id'] = array(
    '#type' => 'textfield',
    '#title' => t('SF account id'),
    '#default_value' => !empty($record['record']['user_info']['sf_account_id']) ? $record['record']['user_info']['sf_account_id'] : '',
  );
  $form['user_info']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => !empty($record['record']['user_info']['first_name']) ? $record['record']['user_info']['first_name'] : '',
  );
  $form['user_info']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => !empty($record['record']['user_info']['last_name']) ? $record['record']['user_info']['last_name'] : '',
  );
  $form['user_info']['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#default_value' => !empty($record['record']['user_info']['email']) ? $record['record']['user_info']['email'] : '',
  );

   // billing information
  $form['billing_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Billing info'),
  );
  $form['billing_info']['address_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Address 1'),
    '#default_value' => !empty($record['record']['billing_info']['address_1']) ? $record['record']['billing_info']['address_1'] : '',
    '#required' => TRUE,
  );
  $form['billing_info']['address_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address 2'),
    '#default_value' => !empty($record['record']['billing_info']['address_2']) ? $record['record']['billing_info']['address_2'] : '',
  );
  $form['billing_info']['country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => !empty($record['record']['billing_info']['country']) ? $record['record']['billing_info']['country'] : '',
    '#required' => TRUE,
  );
  $form['billing_info']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => !empty($record['record']['billing_info']['city']) ? $record['record']['billing_info']['city'] : '',
    '#required' => TRUE,
  );
  $form['billing_info']['state'] = array(
    '#type' => 'textfield',
    '#title' => t('State'),
    '#default_value' => !empty($record['record']['billing_info']['state']) ? $record['record']['billing_info']['state'] : '',
    '#required' => TRUE,
  );
  $form['billing_info']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#default_value' => !empty($record['record']['billing_info']['zip']) ? $record['record']['billing_info']['zip'] : '',
    '#required' => TRUE,
  );

  // card/payment details
  $form['payment_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Details'),
  );
  $form['payment_details']['gateway_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway type'),
    '#default_value' => !empty($record['record']['payment_details']['gateway_type']) ? $record['record']['payment_details']['gateway_type'] : '',
    '#required' => TRUE,
  );
  $form['payment_details']['gateway_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway token'),
    '#default_value' => !empty($record['record']['payment_details']['gateway_token']) ? $record['record']['payment_details']['gateway_token'] : '',
    '#required' => TRUE,
  );
  $form['payment_details']['last_four'] = array(
    '#type' => 'textfield',
    '#title' => t('Last four'),
    '#default_value' => !empty($record['record']['payment_details']['last_four']) ? $record['record']['payment_details']['last_four'] : '',
    '#required' => TRUE,
  );
  $form['payment_details']['card_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Card type'),
    '#default_value' => !empty($record['record']['payment_details']['type']) ? $record['record']['payment_details']['type'] : '',
    '#required' => TRUE,
  );
  $form['payment_details']['month'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration month'),
    '#default_value' => !empty($record['record']['payment_details']['month']) ? $record['record']['payment_details']['month'] : '',
    '#required' => TRUE,
  );
  $form['payment_details']['year'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration year'),
    '#default_value' => !empty($record['record']['payment_details']['year']) ? $record['record']['payment_details']['year'] : '',
    '#required' => TRUE,
  );

  $form['payment_schedule'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment schedule'),
  );

  $payment_schedule = !empty($record['record']['payment_schedule']) ? $record['record']['payment_schedule'] : array();
  $count = 0;

  // payment schedule details
  foreach ($payment_schedule as $payment_instance) {
    $form['payment_schedule'][$count] = array(
      '#type' => 'fieldset',
      '#title' => t('Record !count', array('!count' => $count +1)),
    );
    $form['payment_schedule'][$count]['year_' . $count] = array(
      '#type' => 'textfield',
      '#title' => t('Year'),
      '#default_value' => !empty($payment_schedule[$count]['year']) ? $payment_schedule[$count]['year'] : '',
      '#required' => TRUE,
    );
    $form['payment_schedule'][$count]['month_' . $count] = array(
      '#type' => 'textfield',
      '#title' => t('Month'),
      '#default_value' => !empty($payment_schedule[$count]['month']) ? $payment_schedule[$count]['month'] : '',
      '#required' => TRUE,
    );
    $form['payment_schedule'][$count]['day_' . $count] = array(
      '#type' => 'textfield',
      '#title' => t('Day'),
      '#default_value' => !empty($payment_schedule[$count]['day']) ? $payment_schedule[$count]['day'] : '',
      '#required' => TRUE,
    );
    $form['payment_schedule'][$count]['sf_opportunity_id_' . $count] = array(
      '#type' => 'textfield',
      '#title' => t('SF opportunity id'),
      '#default_value' => !empty($payment_schedule[$count]['salesforce_opportunity_id']) ? $payment_schedule[$count]['salesforce_opportunity_id'] : '',
      '#required' => TRUE,
    );
    $form['payment_schedule'][$count]['status_' . $count] = array(
      '#type' => 'textfield',
      '#title' => t('Status'),
      '#default_value' => !empty($payment_schedule[$count]['status']) ? $payment_schedule[$count]['status'] : '',
      '#required' => TRUE,
    );
    $form['payment_schedule'][$count]['attempts_' . $count] = array('#type' => 'value', '#value' => $payment_schedule[$count]['attempts']);
    ++$count;
  }
  $form['sustainer_series_count'] = array('#type' => 'value', '#value' => $count);
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#submit'][] = 'sustainer_import_edit_queue_item_submit';
  return $form;
}

function sustainer_import_edit_queue_item_submit($form, $form_state) {

  $payment_schedule = array();

  for ($i = 0; $i < $form_state['values']['sustainer_series_count']; ++$i) {
    $payment_schedule[$i] = array(
      'year' => $form_state['values']['year_' . $i],
      'month' => $form_state['values']['month_' . $i],
      'day' => $form_state['values']['day_' . $i],
      'salesforce_opportunity_id' => $form_state['values']['sf_opportunity_id_' . $i],
      'salesforce_recurring_id' => '', // not in use
      'status' => $form_state['values']['status_' . $i],
      'attempts' => $form_state['values']['attempts_' . $i], // passed through from original record, no edit.
    );
  }

  $record = array(
    'order_id' => $form_state['values']['master_order_id'],
    'salesforce_opportunity_id' => $form_state['values']['master_salesforce_opportunity_id'],
    'salesforce_recurring_donation_id' => $form_state['values']['salesforce_recurring_donation_id'],
    'amount' => (float) $form_state['values']['amount'],
    'created' => $form_state['values']['created'],
    'user_info' => array(
      'sf_contact_id' => $form_state['values']['sf_contact_id'],
      'sf_account_id' => $form_state['values']['sf_account_id'],
      'first_name' => $form_state['values']['first_name'],
      'last_name' => $form_state['values']['last_name'],
      'email' => $form_state['values']['mail'],
    ),
    'billing_info' => array(
      'address_1' => $form_state['values']['address_1'],
      'address_2' => $form_state['values']['address_2'],
      'city' => $form_state['values']['city'],
      'state' => $form_state['values']['state'],
      'zip' => $form_state['values']['zip'],
      'country' => $form_state['values']['country'],
    ),
    'payment_details' => array(
      'gateway_type' => $form_state['values']['gateway_type'],
      'gateway_token' => $form_state['values']['gateway_token'],
      'last_four' => $form_state['values']['last_four'],
      'type' => $form_state['values']['card_type'],
      'month' => $form_state['values']['month'],
      'year' => $form_state['values']['year'],
    ),
    'payment_schedule' => $payment_schedule,
  );

  $data = array(
    'rid' => $form_state['values']['rid'],
    'master_order_id' => $form_state['values']['master_order_id'],
    'record' => $record, // TODO: serialize
    'created' => time(),
    'status' => '', // reset status on save, this requeues the record.
    'gateway_type' => $form_state['values']['gateway_type'],
  );

  $data['record'] = serialize($data['record']);
  drupal_write_record('sustainer_import_queue', $data, array('rid'));
  drupal_set_message('This record has been updated and re-queued for import.');
}


function sustainer_import_queue_failure_report() {
  $failures = sustainer_import_get_failed_queue_items();
  $headers = array('master order id', 'date', 'id log', 'errors', 'actions');

  foreach ($failures as $rid => $record) {
    $log = sustainer_import_get_log_entry($rid);

    $status_rows[] = array(
      'data' => array(
        $record['order_id'],
        format_date($log['timestamp'], 'short'),
        _sustainer_import_unpack_id_log($log),
        _sustainer_import_unpack_errors($log),
        l('Edit', 'admin/config/development/sustainer_import/' . $rid . '/edit'),
      )
    );
  }
  $headers = array(
    array('data' => array('Master order ID')),
    array('data' => array('Date')),
    array('data' => array('ID Log')),
    array('data' => array('Errors')),
    array('data' => array('Actions')),
  );
  $table = theme('table', array('header' => $headers, 'rows' => $status_rows));
  return $table;
}

function _sustainer_import_unpack_id_log($log) {
  $output = '';
  foreach ($log['import_ids'] as $key => $data) {
    $output .= '<p>' . $key . ':' . $data['value'] . '  (' . $data['message'] . ')</p>';
  }
  return $output;
}

function _sustainer_import_unpack_errors($log) {
  $output = '';
  foreach ($log['errors'] as $key => $error) {
    $output .= '<p>' . $error . '</p>';
  }
  return $output;
}
