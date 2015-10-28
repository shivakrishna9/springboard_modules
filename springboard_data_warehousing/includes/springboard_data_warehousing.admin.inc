<?php

/**
 * @file
 * Admin functionality for Springboard Data Warehousing.
 */

/**
 * Data warehousing settings form.
 */
function springboard_data_warehousing_admin_settings_form($form, &$form_state) {
  $form['springboard_data_warehousing_url'] = array(
    '#type' => 'textfield',
    '#title' => 'Data Warehouse URL',
    '#default_value' => variable_get('springboard_data_warehousing_url', ''),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for data warehousing settings form.
 */
function springboard_data_warehousing_admin_settings_form_submit($form, &$form_state) {
  variable_set('springboard_data_warehousing_url', $form_state['values']['springboard_data_warehousing_url']);
}
