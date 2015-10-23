<?php
/**
 * @file
 * Admin functionality for Fundraiser Data Warehousing.
 */

function fundraiser_data_warehousing_admin_settings_form($form, &$form_state) {
  $form['fundraiser_data_warehousing_url'] = array(
    '#type' => 'textfield',
    '#title' => 'Data Warehouse URL',
  );
}

function fundraiser_data_warehousing_admin_settings_form_submit($form, &$form_state) {

}
