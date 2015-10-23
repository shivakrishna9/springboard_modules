<?php
/**
 * @file
 * Admin functionality for Fundraiser Data Warehousing.
 */

function fundraiser_data_warehousing_admin_settings_form($form, &$form_state) {
  $form['fundraiser_data_warehousing_url'] = array(
    '#type' => 'textfield',
    '#title' => 'Data Warehouse URL',
    '#default_value' => variable_get('fundraiser_data_warehousing_url', ''),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function fundraiser_data_warehousing_admin_settings_form_submit($form, &$form_state) {
  variable_set('fundraiser_data_warehousing_url', $form_state['values']['fundraiser_data_warehousing_url']);
}
