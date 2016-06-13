<?php
/**
 * @file
 * Admin form definition for salesforce_donation_history.
 */

/**
 * Page callback.
 */
function salesforce_donation_history_query() {
  $form = array();
  $form['salesforce_donation_history_query'] = array(
    '#type' => 'textarea',
    '#title' => t('Opportunities query'),
    '#default_value' => variable_get('salesforce_donation_history_query', DONATION_HISTORY_DEFAULT_SOQL),
    '#description' => "Enter the Salesforce SOQL query used to get opportunities. The query must end in <samp>WHERE AccountId = ':accountId'</samp>. Do not include a LIMIT clause.",
  );
  $form['#validate'][] = 'salesforce_donation_history_query_validate';
  $form = system_settings_form($form);
  return $form;
}

/**
 * Page callback.
 */
function salesforce_donation_history_fields() {
  $form = array();
  $field_definitions = variable_get('salesforce_donation_history_query_fields');
  $field_defaults = variable_get('salesforce_donation_history_field_settings', NULL);
  $form['salesforce_donation_history_field_settings'] = array(
    '#prefix' => '<div id="salesforce_donation_history_field_settings">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'salesforce_donation_history_admin_field_table'
  );
  foreach ($field_definitions as $sf_table => $sf_field) {
    $form['salesforce_donation_history_field_settings'][$sf_table]['label'] = array(
      '#type' => 'textfield',
      '#default_value' => $field_defaults[$sf_table]['label'],
      '#extra_data' => array(
        'field_table' => $sf_table,
        'field_name' => '',
      ),
    );
    $form['salesforce_donation_history_field_settings'][$sf_table]['show_on_tab'] = array(
      '#type' => 'checkbox',
      '#default_value' => $field_defaults[$sf_table]['show_on_tab'],
    );
    $form['salesforce_donation_history_field_settings'][$sf_table]['show_on_receipt'] = array(
      '#type' => 'checkbox',
      '#default_value' => $field_defaults[$sf_table]['show_on_receipt'],
    );
    $form['salesforce_donation_history_field_settings'][$sf_table]['show_on_summary'] = array(
      '#type' => 'checkbox',
      '#default_value' => $field_defaults[$sf_table]['show_on_summary'],
    );
    foreach ($sf_field as $name => $value) {
      $form['salesforce_donation_history_field_settings'][$sf_table . '-' . $name]['label'] = array(
        '#type' => 'textfield',
        '#default_value' => $field_defaults[$sf_table . '-' . $name]['label'],
        '#extra_data' => array(
          'field_table' => $sf_table,
          'field_name' => $name,
        ),
      );
      $form['salesforce_donation_history_field_settings'][$sf_table . '-' . $name]['show_on_tab'] = array(
        '#type' => 'checkbox',
        '#default_value' => $field_defaults[$sf_table . '-' . $name]['show_on_tab'],
      );
      $form['salesforce_donation_history_field_settings'][$sf_table . '-' . $name]['show_on_receipt'] = array(
        '#type' => 'checkbox',
        '#default_value' => $field_defaults[$sf_table . '-' . $name]['show_on_receipt'],
      );
      $form['salesforce_donation_history_field_settings'][$sf_table . '-' . $name]['show_on_summary'] = array(
        '#type' => 'checkbox',
        '#default_value' => $field_defaults[$sf_table . '-' . $name]['show_on_summary'],
      );
    }
  }
  $form = system_settings_form($form);
  return $form;
}

/**
 * Validation callback for salesforce_donation_history_query form.
 */
function salesforce_donation_history_query_validate($form, &$form_state){
  $result = salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query']);
  if ($result->size != 1) {
    form_set_error('salesforce_donation_history_query', t('There was a problem performing this query. A sample record could not be found.'));
  }
  else {
    variable_set('salesforce_donation_history_query_fields', salesforce_donation_history_get_query_fields($result->records));
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
  return $result;
}
