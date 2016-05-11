<?php
/**
 * @file
 * Admin form definition for salesforce_tax_receipts.
 */

function salesforce_tax_receipts_admin() {
  $form = array();
  $form['salesforce_tax_receipts_query'] = array(
    '#type' => 'textarea',
    '#title' => t('Opportunities query'),
    '#default_value' => variable_get('salesforce_tax_receipts_query', 'SELECT AccountId, Amount, CC_Last_4__c, CC_Type__c FROM Opportunity'),
    '#description' => 'Enter the Salesforce SOQL query used to get opportunities',
  );
  $form = system_settings_form($form);
  return $form;
}
