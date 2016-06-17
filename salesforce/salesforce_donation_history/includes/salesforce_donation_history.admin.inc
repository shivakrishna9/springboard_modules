<?php
/**
 * @file
 * Admin form definitions for salesforce_donation_history.
 */

/**
 * Page callback.
 */
function salesforce_donation_history_query() {
  $form = array();
  $form['salesforce_donation_history_query'] = array(
    '#type' => 'textarea',
    '#title' => t('Opportunities query'),
    '#default_value' => variable_get('salesforce_donation_history_query', SALESFORCE_DONATION_HISTORY_DEFAULT_SOQL),
    '#description' => "Enter the Salesforce SOQL query used to get opportunities. <ul><li>The Opportunity fields <samp>Id</samp> and <samp>CloseDate</samp> are required.</li><li>The query must end in <samp>WHERE AccountId = ':accountId'</samp>.</li><li>Do not include a LIMIT clause.</li>",
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
    '#theme' => 'salesforce_donation_history_admin_field_table',
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
 * Page callback.
 */
function salesforce_donation_history_receipts() {
  $form = array();
  $defaults = array(
    'value' => '',
    'format' => filter_default_format(),
  );
  $form['header_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Header Fields'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['header_fields']['salesforce_donation_history_receipts_header_banner'] = array(
    '#type' => 'managed_file',
    '#title' => t('Header Banner Image'),
    '#theme' => 'salesforce_donation_history_image_preview',
    '#description' => t('Upload a file, allowed extensions: jpg, jpeg, png, gif'),
    '#default_value' => variable_get('salesforce_donation_history_receipts_header_banner', ''),
    '#upload_location' => 'public://salesforce_donation_history/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
  );
  $salesforce_donation_history_receipts_address = variable_get('salesforce_donation_history_receipts_address', $defaults);
  $form['header_fields']['salesforce_donation_history_receipts_address'] = array(
    '#type' => 'text_format',
    '#title' => t('Address'),
    '#default_value' => $salesforce_donation_history_receipts_address['value'],
    '#description' => "",
    '#format' => $salesforce_donation_history_receipts_address['format'],
  );
  $salesforce_donation_history_receipts_intro = variable_get('salesforce_donation_history_receipts_intro', $defaults);
  $form['header_fields']['salesforce_donation_history_receipts_intro'] = array(
    '#type' => 'text_format',
    '#title' => t('Introduction'),
    '#default_value' => $salesforce_donation_history_receipts_intro['value'],
    '#description' => "",
    '#format' => $salesforce_donation_history_receipts_intro['format'],

  );
  $form['table'] = array(
    '#type' => 'markup',
    '#markup' => t('A table with donation information will appear between the header and footer fields, here. You can control which columns are displayed in this table on the <a href="fields">Fields</a> tab.'),
  );
  $form['footer_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Footer Fields'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $salesforce_donation_history_receipts_outro = variable_get('salesforce_donation_history_receipts_outro', $defaults);
  $form['footer_fields']['salesforce_donation_history_receipts_outro'] = array(
    '#type' => 'text_format',
    '#title' => t('Footer text'),
    '#default_value' => $salesforce_donation_history_receipts_outro['value'],
    '#description' => "",
    '#format' => $salesforce_donation_history_receipts_outro['format'],
  );
  $form['footer_fields']['salesforce_donation_history_receipts_footer_banner'] = array(
    '#type' => 'managed_file',
    '#title' => t('Footer Banner Image'),
    '#theme' => 'salesforce_donation_history_image_preview',
    '#description' => t('Upload a file, allowed extensions: jpg, jpeg, png, gif'),
    '#default_value' => variable_get('salesforce_donation_history_receipts_footer_banner', ''),
    '#upload_location' => 'public://salesforce_donation_history/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
  );
  $form['#submit'][] = 'salesforce_donation_history_receipts_submit';
  $form = system_settings_form($form);
  return $form;
}

/**
 * Submit handler for salesforce_donation_history_receipts form.
 */
function salesforce_donation_history_receipts_submit($form, $form_state) {
  if ($form_state['values']['salesforce_donation_history_receipts_header_banner']) {
    $value = $form_state['values']['salesforce_donation_history_receipts_header_banner'];
    $file = file_load($value);
    if ($file && !$file->status) {
      $file->status = FILE_STATUS_PERMANENT;
      file_save($file);
      file_usage_add($file, 'salesforce_donation_history', 'admin_default', 1);
    }
  }
  if ($form_state['values']['salesforce_donation_history_receipts_footer_banner']) {
    $value = $form_state['values']['salesforce_donation_history_receipts_footer_banner'];
    $file = file_load($value);
    if ($file && !$file->status) {
      $file->status = FILE_STATUS_PERMANENT;
      file_save($file);
      file_usage_add($file, 'salesforce_donation_history', 'admin_default', 1);
    }
  }
}

/**
 * Validation handler for salesforce_donation_history_query form.
 */
function salesforce_donation_history_query_validate($form, &$form_state) {
  $result = salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query']);
  if ($result->size != 1) {
    form_set_error('salesforce_donation_history_query', t('There was a problem performing this query. A sample record could not be found.'));
  }
  else {
    $query_fields = salesforce_donation_history_get_query_fields($result->records);
    $field_types = salesforce_donation_history_get_field_types($query_fields);
    variable_set('salesforce_donation_history_query_fields', $field_types);
    cache_clear_all('*', 'cache_salesforce_donation_history', TRUE);
    drupal_set_message('Please review the field configuration options below.');
    $form_state['redirect'] = array('admin/config/salesforce/donation_history/fields');
  }
}
