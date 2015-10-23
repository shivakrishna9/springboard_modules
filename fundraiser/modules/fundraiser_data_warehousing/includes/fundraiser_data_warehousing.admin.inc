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

  return system_settings_form($form);
}

function fundraiser_data_warehousing_admin_settings_form_submit($form, &$form_state) {

}
