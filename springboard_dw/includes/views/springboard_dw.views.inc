<?php

/**
 * @file
 * Defines Springboard Data Warehouse Views.
 */

/**
 * Implements hook_views_data().
 *
 * Defines pseudo-DB tables for creating views that fetch their data from the remote data warehouse.
 */
function springboard_dw_views_data() {
  // Defines a view type which displays lifetime performance of a given donation form from the warehouse:
  $data['sp_dw_donation_stats_psudo']['table'] = array(
    'group' => t('Springboard DW Donation Stats'),
    'base' => array(
      'title' => t('Springboard DW Donation Stats'),
      'help' => t('Fetches lifetime performance statistic from the data warehouse for a single, contextually filtered donation form.'),
      'query class' => 'springboard_dw_donation_stats_views_plugin_query', 
    ),
  );
  // Define the fields:
  $data['sp_dw_donation_stats_psudo']['raised'] = array(
    'title' => t('Total Amount Donated'),
    'help' => t('Fetches the total amount donated from this donation form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['gifts'] = array(
    'title' => t('Total Donations'),
    'help' => t('Fetches the total number of donations submitted from this donation form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['sustainers'] = array(
    'title' => t('Total Recurring Donations'),
    'help' => t('Fetches the total number of recurring donations submitted from this donation form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['first_time_donors'] = array(
    'title' => t('Total First Time Donations'),
    'help' => t('Fetches the total number of first-time donations submitted from this donation form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['first_donation_id'] = array(
    'title' => t('First Donation ID'),
    'help' => t('Fetches the Donation ID of the oldest donation submitted to the DW from this form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['latest_donation_id'] = array(
    'title' => t('Latest Donation ID'),
    'help' => t('Fetches the Donation ID of the most recent donation submitted to the DW from this form.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['first_submission_date'] = array(
    'title' => t('First Donation Date'),
    'help' => t('Fetches the date when the oldest donation to this form was submitted.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['latest_submission_date'] = array(
    'title' => t('Latest Donation Date'),
    'help' => t('Fetches the date when the most recent donation to this form was submitted.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['donation_id'] = array(
    'title' => t('Donation ID'),
    'help' => t('The ID of the donation form from which statistics will be fetched.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['sp_dw_donation_stats_psudo']['latest_fetch'] = array(
    'title' => t('Latest fetch'),
    'help' => t('Displays a message detailing how long ago this data was fetched from the warehouse.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),    
  );
  $data['sp_dw_donation_stats_psudo']['node_status'] = array(
    'title' => t('Node Status'),
    'help' => t('The current donation form\'s node status.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );
  // End DW Life Donation Stats view type

  // Defines a view type which displays donations data from the warehouse:
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

  // Add exposed filter support to the donation_id field:
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

  // Add support for date filtering:
  $data['sp_dw_donations_psudo']['transaction_date']['field'] = array(
    'handler' => 'springboard_dw_views_handler_date_field',
    'click sortable' => TRUE,
    'is_date' => TRUE,
  );
  $data['sp_dw_donations_psudo']['transaction_date']['filter'] = array(
    'handler' => 'springboard_dw_views_handler_date_filter',
    'empty_field_name' => 'Undated',
    'is_date' => TRUE,
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
  // End DW donations view type

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
      'springboard_dw_donation_stats_views_plugin_query' => array(
        'title' => t('Springboard Data Warehouse Donation Statistics API Request'),
        'help' => t('Uses Guzzle for querying remote donation data'),
        'handler' => 'springboard_dw_donation_stats_views_plugin_query',
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
    drupal_add_css('//maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.css', array('type' => 'external'));
    $form['#attached']['css'][] = drupal_get_path('module', 'springboard_dw') . '/css/springboard_dw.css';
    $form['#attached']['js'][] = drupal_get_path('module', 'springboard_dw') . '/js/springboard_dw.js';
  }

  // Alter date filter value format:
  if (isset($form['transaction_date']['min']['#date_format'])) {
    $form['transaction_date']['min']['#date_format'] = 'm/d/Y';
  }
  if (isset($form['transaction_date']['max']['#date_format'])) {
    $form['transaction_date']['max']['#date_format'] = 'm/d/Y';
  }
}

/**
 * Implements hook_views_post_render().
 *
 * Alter presentation for the single donation display:
 */
function springboard_dw_views_post_render(&$view, &$output, &$cache) {
  if ($view->name != 'springboard_dw_donations_report' || !isset($view->args[0]) || !is_numeric($view->args[0])) {
    return;
  }

  $output = '<div id="dw-donation-detail">';
  if (!isset($view->result[0])) {
    $output = 'No data available.';
    return;
  }
  $data = $view->result[0];
  dpm($data);

  $fields_groups = array(
    'order' => array(
      'label' => t('Transaction'),
      'fields' => array(
        'amount_formatted' => t('Donation Amount'),
        'donation_id' => t('Donation ID'),
        'transaction_id' => t('Transaction ID'), 
        'transaction_date' => t('Transaction Date'),
        'form_id' => t('Form ID'),
        'status' => t('Transaction Status'),
      ),
    ),
    'contact' => array(
      'label' => t('Contact'),
      'fields' => array(
        'contact_id' => t('Contact ID'),
        'email' => t('Email Address'),
        'first_name' => t('First Name'),
        'last_name' => t('Last Name'),
        'address' => t('Address'),
        'address_line_2' => t('Address 2'),
        'city' => t('City'),
        'state' => t('State'),
        'zip' => t('Postal Code'),
        'country' => t('Country'),
      ),
    ),
    'payment' => array(
      'label' => t('Payment'),
      'fields' => array(
        'gateway' => t('Gateway'),
        'payment_method' => t('Payment Method'),
        'card_type' => t('Card Type'),
        'card_last_4' => t('Card Last 4'),
        'card_expiration_month' => t('Card Exp. Month'),
        'card_expiration_year' => t('Card Exp. Year'),
        'bank_account_mask' => t('Bank Account Mask'),
      ),   
    ),
  );
  foreach ($fields_groups as $group_key => $field_group) {
    $output .= '<div class="dw-field-group-container dw-field-group-container-' . $group_key . '">';
    $output .= '<h2>' . $field_group['label'] . '</h2>';
    $output .= '<table class="dw-field-group dw-field-group-' . $group_key . '"><tbody>';
    foreach ($field_group['fields'] as $field_key => $field_label) {
      if (isset($data->$field_key)) {
        $output .= '<tr><td class="label">' . $field_label . ':</td><td class="value"> ' . $data->$field_key . '</td></tr>';
      }
    }
    $output .= '</tbody></table></div>';
  }
  $output .= '</div>';
  drupal_add_css(drupal_get_path('module', 'springboard_dw') . '/css/springboard_dw.css');
}

