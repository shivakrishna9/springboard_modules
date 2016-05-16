<?php
/**
 * @file
 * Admin form definition for salesforce_donation_history.
 */

function salesforce_donation_history_admin() {
  $form = array();
  $form['salesforce_donation_history_query'] = array(
    '#type' => 'textarea',
    '#title' => t('Opportunities query'),
    '#default_value' => variable_get('salesforce_donation_history_query', DONATION_HISTORY_DEFAULT_SOQL),
    '#description' => 'Enter the Salesforce SOQL query used to get opportunities',
  );
  $form = system_settings_form($form);
  return $form;
}
