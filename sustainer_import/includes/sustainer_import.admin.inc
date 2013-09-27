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
     '#description' => t('Current supported formats are discussed in the module documentation. Allowed file extensions: .txt');
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
    $file = file_save_upload('file', array(
        'file_validate_extensions' => array('txt'), // Validate extensions.
    ));
    // If the file passed validation:
    if ($file) {
        // Move the file, into the Drupal file system
        if ($file = file_move($file, 'public://')) {
            // Save the file for use in the submit handler.
            $form_state['storage']['file'] = $file;
        }
        else {
            form_set_error('import_file', t('Failed to write the uploaded file to the site\'s file folder.'));
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

}


