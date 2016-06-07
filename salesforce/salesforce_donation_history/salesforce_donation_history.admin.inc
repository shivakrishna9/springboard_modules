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
    '#description' => "Enter the Salesforce SOQL query used to get opportunities. The query must end in <samp>WHERE AccountId = ':accountId'</samp>. Do not include a LIMIT clause.",
  );
  $form['#validate'][] = 'salesforce_donation_history_admin_validate';
  $form = system_settings_form($form);
  return $form;
}

/**
 * Validation callback for salesforce_donation_history_admin form.
 */
function salesforce_donation_history_admin_validate($form, &$form_state){
  // $result = salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query']);
  if (!salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query'])) {
    form_set_error('salesforce_donation_history_query', t('There was a problem performing this query. A sample record could not be found.'));
  }
}

/**
 * Perform a test query of the given SOQL.
 *
 * Replaces the WHERE claue of the SOQL query with LIMIT 1. This should return
 * the first available result, including all of the fields included in the
 * query.
 */
function salesforce_donation_history_test_query($query) {
  $soql = strtr($query, array("WHERE AccountId = ':accountId'" => 'LIMIT 1'));
  // @TODO maybe do this in a try/catch to avoid unexpected error message if the
  // SOQL is malformed.
  $sfapi = salesforce_get_api();
  $soap = new SalesforceSoapPartner($sfapi);
  $response = $soap->query($soql);
  $result = new QueryResult($response);
  dpm($result);
  // Print createQueryResultTable($response->records, 0);.
  if ($result->size != 1) {
    return FALSE;
  }
  return TRUE;
}
