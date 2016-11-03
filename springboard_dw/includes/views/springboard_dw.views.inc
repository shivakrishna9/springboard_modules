<?php

/**
 * @file
 * Defines Springboard Data Warehouse Views.
 */

/**
 * Implements hook_views_data().
 */
function springboard_dw_views_data() {
  // Define a pseudo-DB table and assign the custom query handler which queries the remote API server:
  $data['sp_dw_donations_psudo']['table'] = array(
    'group' => t('Springboard DW Donations'),
    'base' => array(
      'title' => t('Springboard DW Donations'),
      'help' => t('Fetches archived donation data from the Data Warehouse rather than the Drupal database.'),
      'query class' => 'springboard_dw_donations_views_plugin_query',
    ),
  );

  // Fields:
  $field_keys = array(
    'unique_id' => t('Unique ID'),
    'donation_id' => t('Donation ID'), 
    'parent_id' => t('Parent ID'),
    'group_id' => t('Group ID'),
    'amount' => t('Donation Amount'), 
    'currency' => t('Currency'),
    'quantity' => t('Quantity'),
    'quick_submit' => t('Quick Submit'),
    'click_submit' => t('Click Submit'),
    'ms' => t('MS'), 
    'cid' => t('cid'),
    'first_name' => t('First Name'),
    'last_name' => t('Last Name'),
    'email' => t('Email Address'),
    'address' => t('Mailing Address'),
    'address_line_2' => t('Mailing Address - Line 2'),
    'city' => t('City'),
    'state' => t('State'),
    'country' => t('Country'),
    'zip' => t('Zip/Postal Code'),
    'referrer' => t('Referred URL'),
    'initial_referrer' => t('Initial Referred URL'),
    'payment_method' => t('Payment Method'),
    'recurs_monthly' => t('Recurs Monthly'),
    'search_engine' => t('Search Engine'),
    'search_string' => t('Search String'),
    'user_agent' => t('User Agent'),
    'device_type' => t('Device Type'),
    'device_name' => t('Device Name'),
    'device_os' => t('Device OS'),
    'device_browser' => t('Device Browser'),
    'form_id' => t('Donation Form ID'),
    'contact_id' => t('Contact ID'),
    'submission_id' => t('Submission ID'),
    'status' => t('Transaction Status'),
    'transaction_id' => t('Transaction ID'),
    'gateway' => t('Gateway'),
    'card_type' => t('Credit Card Type'),
    'card_last_4' => t('Card Last 4'),
    'card_expiration_month' => t('Card Expiration Month'),
    'card_expiration_year' => t('Card Expiration year'),
    'bank_account_mask' => t('Bank Account Mask'),
    'ip_address' => t('IP Address'),
    'created_at' => t('Created At'),
    'updated_at' => t('Updated At'),
    'transaction_date' => t('Transaction Date'),
    'extra_props' => t('Extra Props Data'),
    'form_name' => t('Form Name'),
  );
  foreach ($field_keys as $field_key => $field_label) {
    $data['sp_dw_donations_psudo'][$field_key] = array(
      'title' => $field_label,
      'help' => t('Data warehouse value for key "' . $field_key . '"'),
      'field' => array(
        'handler' => 'springboard_dw_views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
  }

  if (isset($data['sp_dw_donations_psudo']['donation_id'])) {
    $data['sp_dw_donations_psudo']['donation_id']['argument'] = array(
      'title' => t('Donation ID'),
      'help' => t('Fetches a single donation record by Donation ID.'),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ), 
    );
  }

  // Add field-agnostic data warehouse search filter:
  $data['sp_dw_donations_psudo']['dw_global_search'] = array(
    'title' => t('Data Warehouse Search'),
    'help' => t('Keyword search across all donation fields.'),
    'filter' => array(
      'handler' => 'springboard_dw_views_handler_filter',
    ),
  );

  // Add "Payment" link:
  $data['sp_dw_donations_psudo']['payment_link'] = array(
    'title' => t('Payment'),
    'help' => t('Provides a link to a payment, if it currently exists in the Drupal DB.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
      'click sortable' => FALSE,
    ),
  );

  return $data;
}

/**
 * Implements hook_views_plugins().
 *
 * Declare a query plugin to fetch data from the data warehouse vs DB queries.
 */
function springboard_dw_views_plugins() {
  $plugins = array(
    // The query plugin replaces DB queries with API queries:
    'query' => array(
      'springboard_dw_donations_views_plugin_query' => array(
        'title' => t('Springboard Data Warehouse Donations API Request'),
        'help' => t('Uses Guzzle for querying remote donation data'),
        'handler' => 'springboard_dw_donations_views_plugin_query',
        'path' => drupal_get_path('module', 'springboard_dw') . '/includes/views/plugins',
      ),
    ),
  );
  return $plugins;
}

/**
 * Implements hook_views_handlers().
 *
 * Declare custom data warehouse handlers.
 */
function springboard_dw_views_handlers() {
  $handlers = array(
    'info' => array(
      'path' => drupal_get_path('module', 'springboard_dw') . '/includes/views/handlers',
    ),
    'handlers' => array(
      'springboard_dw_views_handler_filter' => array('parent' => 'views_handler_filter'),
    ),
  );
  return $handlers;
}

/**
 * Implements hook_form_alter()
 */
function springboard_dw_form_views_exposed_form_alter(&$form, &$form_state) {
  if (isset($form['#id']) && $form['#id'] == 'views-exposed-form-springboard-dw-donations-report-page') {
    // Attached CSS and jQuery related to the exposed filters:
    $form['#attached']['css'][] = drupal_get_path('module', 'springboard_dw') . '/css/springboard_dw.css';
    $form['#attached']['js'][] = drupal_get_path('module', 'springboard_dw') . '/js/springboard_dw.js';
  }
}
