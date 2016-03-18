<?php

/**
 * @file
 * Administration.
 */

/**
 * Form callback for admin page.
 */
function salesforce_login_admin_form($form, $form_state) {
  $form = array();

  $form['salesforce_login_multiple_contacts_error'] = array(
    '#type' => 'textarea',
    '#title' => t('Multiple contacts error message'),
    '#default_value' => variable_get('salesforce_login_multiple_contacts_error'),
    '#description' => t('Display this error message when a new user tries to login for the first time but multiple contacts are found in salesforce.'),
    '#required' => TRUE,
  );

  $form['salesforce_login_no_contacts_error'] = array(
    '#type' => 'textarea',
    '#title' => t('No contact found error message'),
    '#default_value' => variable_get('salesforce_login_no_contact_found_error'),
    '#description' => t('Display this error message when a new user tries to login for the first time but multiple contacts are found in salesforce.'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}