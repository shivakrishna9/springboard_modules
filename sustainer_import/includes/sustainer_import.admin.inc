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
    $input = '';
    $infile = fopen($file->uri, 'r');
    while ($line = fgets($infile)) {
      $input .= $line;
    }
    // We're manually handling serialization issues here so we want to suppress
    // the natural output of unserialize() in the event of an error.
    $input = @unserialize($input);
    if (is_array($input)) {
      foreach($input as $order_detail) {
        // inject into db table
        sustainer_import_import_record_insert($order_detail);
      }
      dsm($input, 'uploaded input');
    }
    else {
      drupal_set_message('Uploaded file is in an incorrect format or contains errors', 'error');
      // throw error on file format
    }
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
function sustainer_import_edit_queue_item() {
  $args = func_get_args();
  dsm($args, 'args');
  $payment_schedule = array();
  $form = array();
  $form['user_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('User info'),
  );
  $form['user_info']['sf_contact_id'] = array(
    '#type' => 'textfield',
    '#title' => t('SF contact id'),
    '#default_value' => '',
  );
  $form['user_info']['sf_account_id'] = array(
    '#type' => 'textfield',
    '#title' => t('SF account id'),
    '#default_value' => '',
  );
  $form['user_info']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => '',
  );
  $form['user_info']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => '',
  );
  $form['user_info']['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#default_value' => '',
  );

  $form['billing_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Billing info'),
  );
  $form['billing_info']['address_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Address 1'),
    '#default_value' => '',
  );
  $form['billing_info']['address_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address 2'),
    '#default_value' => '',
  );
  $form['billing_info']['country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => '',
  );
  $form['billing_info']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => '',
  );
  $form['billing_info']['state'] = array(
    '#type' => 'textfield',
    '#title' => t('State'),
    '#default_value' => '',
  );
  $form['billing_info']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#default_value' => '',
  );

  $form['payment_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Details'),
  );
  $form['payment_details']['gateway_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway type'),
    '#default_value' => '',
  );
  $form['payment_details']['gateway_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway token'),
    '#default_value' => '',
  );
  $form['payment_details']['last_four'] = array(
    '#type' => 'textfield',
    '#title' => t('Last four'),
    '#default_value' => '',
  );
  $form['payment_details']['card_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Card type'),
    '#default_value' => '',
  );
  $form['payment_details']['month'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration month'),
    '#default_value' => '',
  );
  $form['payment_details']['year'] = array(
    '#type' => 'textfield',
    '#title' => t('Expiration month'),
    '#default_value' => '',
  );

  $form['payment_schedule'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment schedule'),
  );
  foreach ($payment_schedule as $payment_instance) {
    ++$count;
    $form['payment_schedule'][$count] = array(
      '#type' => 'fieldset',
      '#title' => $count,
    );
    $form['payment_schedule'][$count]['year'] = array(
      '#type' => 'textfield',
      '#title' => t('Year'),
    );
    $form['payment_schedule'][$count]['month'] = array(
      '#type' => 'textfield',
      '#title' => t('Month'),
    );
    $form['payment_schedule'][$count]['day'] = array(
      '#type' => 'textfield',
      '#title' => t('Day'),
    );
    $form['payment_schedule'][$count]['sf_opportunity_id'] = array(
      '#type' => 'textfield',
      '#title' => t('SF opportunity id'),
    );
    $form['payment_schedule'][$count]['status'] = array(
      '#type' => 'textfield',
      '#title' => t('Status'),
    );
  }
}


