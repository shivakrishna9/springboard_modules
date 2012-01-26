<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Form for fundraiser confirmation settings.
 */
function fundraiser_confirmation_settings_form($form, &$form_state, $node) {
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  /* Start edit form */
  $form['confirmation_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation page settings'),
    '#collapsible' => TRUE,
  );
  $form['confirmation_page']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#default_value' => isset($node->confirmation_page_title) ? $node->confirmation_page_title : '',
  );
  $form['confirmation_page']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Confirmation message'),
    '#default_value' => isset($node->confirmation_page_body) ? $node->confirmation_page_body : '',
    '#format' => isset($body_format) ? $body_format : NULL,
    '#parents' =>  array('confirmation_page_format'),
  );

  $form['confirmation_page']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['confirmation_page']['tokens']['help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('node'),
    //theme('token_help', 'order') . theme('webform_token_help', (array) $node),
  );

  // Redirection settings.
  if (isset($node->redirect_url) && strpos($node->redirect_url, '<') === 0) {
    $redirect = trim($node->redirect_url, '<>');
    // Redirection is set to front page.
    if ($redirect == 'front') {
      $redirect = 'url';
      $redirect_url = $node->redirect_url;
    }
    else {
      $redirect_url = '';
    }
  }
  else {
    $redirect = 'url';
    $redirect_url = isset($node->redirect_url) ? $node->redirect_url : '';
  }
  $form['confirmation_page']['redirection'] = array(
    '#type' => 'item',
    '#title' => t('Redirection location'),
    '#theme' => 'webform_advanced_redirection_form',
    '#description' => t('Choose where to redirect the user upon successful submission.'),
  );
  $form['confirmation_page']['redirection']['redirect'] = array(
    '#type' => 'radios',
    '#default_value' => $redirect,
    '#options' => array(
      'confirmation' => t('Confirmation page'),
      'url' => t('Custom URL'),
    ),
  );
  $form['confirmation_page']['redirection']['redirect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect URL'),
    '#description' => t('URL to redirect the user to upon successful submission.'),
    '#default_value' => $redirect_url,
    '#maxlength' => 255,
  );

  $form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation email settings'),
    '#collapsible' => TRUE,
  );
  $form['email']['receipt_email_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email Address'),
    '#description' => t('The email address that the receipt email will come from.'),
    '#weight' => -4.6,
    '#maxlength' => 255,
    '#default_value' => isset($node->receipt_email_address) ? $node->receipt_email_address : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_from'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email From'),
    '#description' => t('The name that is associated with the receipt email address.'),
    '#weight' => -4.5,
    '#default_value' => isset($node->receipt_email_from) ? $node->receipt_email_from : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email Subject'),
    '#description' => t('The subject of the donation receipt email.'),
    '#weight' => -4.4,
    '#default_value' => isset($node->receipt_email_subject) ? $node->receipt_email_subject : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Confirmation Email Message'),
    '#weight' => -4.3,
    '#default_value' => isset($node->receipt_email_message) ? $node->receipt_email_message : '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 300,
  );

  return $form;
}

/**
 * Validation function for fundraiser_confirmation_settings_form().
 */
function fundraiser_confirmation_settings_form_validate($form, &$form_state) {
  // Validate email
  if (!fundraiser_validate_email($form_state['values']['receipt_email_address'])) {
    form_set_error('receipt_email_address', t('Receipt Email Address must be a valid email address.'));
  }

  // Ensure the entered redirect URL is valid.
  if ($form_state['values']['redirect'] == 'url') {
    $redirect_url = trim($form_state['values']['redirect_url']);
    if (empty($redirect_url)) {
      form_error($form['confirmation_page']['redirection']['redirect_url'], t('A valid URL is required for custom redirection.'));
    }
    elseif (strpos($redirect_url, 'http') === 0 && !valid_url($redirect_url, TRUE)) {
      form_error($form['confirmation_page']['redirection']['redirect_url'], t('The entered URL is not a valid address.'));
    }
    else {
      form_set_value($form['confirmation_page']['redirection']['redirect_url'], $redirect_url, $form_state);
    }
  }
  elseif ($form_state['values']['redirect'] == 'confirmation') {
    form_set_value($form['confirmation_page']['redirection']['redirect_url'], '<confirmation>', $form_state);
  }
  else {
    form_set_value($form['confirmation_page']['redirection']['redirect_url'], '<confirmation>', $form_state);
  }
}

/**
 * Submit function for fundraiser_confirmation_settings_form().
 */
function fundraiser_confirmation_settings_form_submit($form, &$form_state) {
  $node = node_load($form_state['values']['nid']);
  $node->confirmation_page_title = check_plain($form_state['values']['title']);
  $node->confirmation_page_body = $form_state['values']['body'];
  $node->confirmation_page_format = $form_state['values']['confirmation_page_format'];
  $node->receipt_email_from = check_plain($form_state['values']['receipt_email_from']);
  $node->receipt_email_address = $form_state['values']['receipt_email_address'];
  $node->receipt_email_subject = check_plain($form_state['values']['receipt_email_subject']);
  $node->receipt_email_message = $form_state['values']['receipt_email_message'];
  $node->redirect_url = $form_state['values']['redirect_url'];
  node_save($node);
}

/**
 * Form builder. Configure fundraiser.
 *
 */
function fundraiser_admin_settings($form, &$form_state) {

  $description = 'Use this option if you want donations to be exported to Salesforce.';
  $attributes = array();
  if (!module_exists('sf_donation')) {
    $description .= ' <span class="admin-disabled">The Salesforce Management Donations module must be enabled in order to use this option.</span>';
    $attributes['disabled'] = 'disabled';
  }

  $form['fundraiser_development_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Development mode'),
    '#default_value' => variable_get('fundraiser_development_mode', 0),
    '#description' => t('Development mode allows you to access donation forms over HTTP.'),
  );

  $form['fundraiser_default_minimum'] = array(
    '#type' => 'textfield',
    '#title' => t('Default minimum donation amount'),
    '#default_value' => variable_get('fundraiser_default_minimum', 10.00),
    '#description' => t('Enter the default minimum donation amount that will be used on new donation forms.'),
  );

  $form['fundraiser_salesforce_integration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Integrate with Salesforce'),
    '#default_value' => variable_get('fundraiser_salesforce_integration', 0),
    '#description' => t($description),
    '#attributes' => $attributes,
  );

  $form['fundraiser_all_countries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Insert full list of countries into new donation forms'),
    '#default_value' => variable_get('fundraiser_all_countries', 0),
    '#description' => t('When checked, all countries will be inserted into the billing country field on new donations forms. When unchecked, on U.S and Canda will be inserted.'),
  );

  $form['fundraiser_gateway_messages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display gateway messages'),
    '#default_value' => variable_get('fundraiser_gateway_messages', 1),
    '#description' => t('When checked, the message that is returned from the payment gateway will be displayed in the Drupal status message area of the theme.'),
  );

  $form['fundraiser_receipt_bcc'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation receipt bcc email address'),
    '#default_value' => variable_get('fundraiser_receipt_bcc', ''),
    '#description' => t('The email address of the user that will receive a bcc copy of all donation receipt emails.'),
  );

  $form['fundraiser_cc_expiration_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credit card expiration email'),
    '#collapsible' => TRUE,
  );

  $form['fundraiser_cc_expiration_message']['fundraiser_cc_exp_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email subject'),
    '#default_value' => variable_get('fundraiser_cc_exp_subject', 'Your credit card is about to expire'),
  );

  $form['fundraiser_cc_expiration_message']['fundraiser_cc_exp_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Email body'),
    '#default_value' => variable_get('fundraiser_cc_exp_body', 'Your card ending in [cc_last_4] is about to expire on [exp_month]/[exp_year].'),
    '#description' => t('The body of the email. Available tokens: [exp_month], [exp_year], [order_total], [cc_last_4], [cc_type], [amount]'),
  );

  $form['fundraiser_cc_expiration_message']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Donor replacement tokens'),
    '#description' => t('The following tokens are available for donor'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // TODO Please change this theme call to use an associative array for the $variables parameter.
  $form['fundraiser_cc_expiration_message']['tokens']['user'] = array(
    '#value' => theme('token_help', 'user'),
  );

  $form['fundraiser_cron'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cron settings'),
    '#description' => t('Configure cron settings.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['fundraiser_cron']['fundraiser_standalone_cron_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable standalone cron.'),
    '#description' => t('If this option is enabled all fundraiser related cron tasks will be removed from the standard cron run. These tasks will need to be cronned separately via sitename/fundraiser_cron'),
    '#default_value' => variable_get('fundraiser_standalone_cron_enabled', 0),
  );
  $form['#submit'] =  array('fundraiser_update_cron_settings');

  if (module_exists('form_layouts')) {
    $form['fundraiser_form_layouts'] = array(
      '#type' => 'fieldset',
      '#title' => t('Form layouts settings'),
      '#description' => t('Configure form layouts.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form_layouts = array();
    foreach (_form_layouts_template_list() as $template) {
      $form_layouts[$template['theme']] = $template['name'];
    }
    $form['fundraiser_form_layouts']['fundraiser_form_layouts_default'] = array(
      '#type' => 'select',
      '#title' => t('Default form layout'),
      '#description' => t('Select the default form layout.'),
      '#default_value' => variable_get('fundraiser_form_layouts_default', 'one_column'),
      '#options' => $form_layouts,
    );
  }
  return system_settings_form($form);
}

/**
 * Creates a list of all donation forms on the site
 */
function fundraiser_donation_form_list() {
  global $user;
  $result = db_query('SELECT n.nid, n.uid, n.title, f.internal_name, n.created, n.changed FROM {node} n '.
    'INNER JOIN {fundraiser} f ON f.nid = n.nid '.
    'ORDER BY n.changed DESC');
  $header = array('Title', 'Internal Name', 'Last Modified', '', '', '');
  $rows = array();
  foreach ($result as $data) {
    if (user_access('edit any donation form', $user) || ($data->uid == $user->uid && user_access('edit own donation form', $user))) {
      $edit_link = l(t('edit'), 'node/' . $data->nid . '/edit');
    }
    $view_link = l(t('view'), drupal_get_path_alias('node/' . $data->nid));
    if (user_access('clone donation form', $user)) {
      $clone_link = l(t('clone'), 'node/' . $data->nid . '/clone_form');
    }
    $rows[] = array($data->title, $data->internal_name, date('m/d/Y g:i:s a', $data->changed), $edit_link, $view_link, $clone_link);
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

/**
 * Create a monthly report. TODO get rid of this, drop monthly support feature.
 */
function fundraiser_monthly_report() {
  global $base_url;
  if ($_SESSION['fundraiser_report_filters']) {
    $start_date = $_SESSION['fundraiser_report_filters']['from_date'];
    $end_date = $_SESSION['fundraiser_report_filters']['to_date'];
    $result = module_invoke_all('fundraiser_monthly_report', $start_date, $end_date);
    $result = $result[0];
  }

  $header = array('Order Id', 'Opportunity Id', 'Uid', 'First Name', 'Last Name', 'Email', 'Address', 'Address 2', 'City', 'Country', 'State/Province', 'Postal Code', 'Payment Date', 'Amount', 'Donation Form', 'URL', 'Market Source', 'Campaign ID', 'CC Last Four', 'CC Type', 'CC Exp Month', 'CC Exp Year');
  $columns = array('order_id', 'sfid', 'billing_first_name', 'billing_last_name', 'primary_email', 'billing_street1', 'billing_street2', 'billing_city', 'country_iso_code_2', 'zone_code', 'received', 'order_total', 'title', 'nid');

  $rows = array();
  foreach ($result as $data) {
    $cc_data = module_invoke_all('fundraiser_decrypt_credit', $data->data);
    $cc_date = $cc_date[0];
    $rows[] = array(
      $data->order_id,
      $data->sfid ? $data->sfid : 'Not Exported',
      $data->uid,
      $data->billing_first_name,
      $data->billing_last_name,
      $data->primary_email,
      $data->billing_street1,
      $data->billing_street2,
      $data->billing_city,
      $data->country_iso_code_2,
      $data->zone_code,
      $data->billing_postal_code,
      date('m/d/Y g:i:s a', $data->received),
      '$' . number_format($data->order_total, 2),
      $data->title,
      $base_url . '/node/' . $data->nid,
      $data->market_source,
      $data->campaign_id,
      $cc_data['cc_number'],
      $cc_data['cc_type'],
      $cc_data['cc_exp_month'],
      $cc_data['cc_exp_year'],
    );
  }

  if ($_SESSION['fundraiser_report_filters']['export']) {
    $_SESSION['fundraiser_report_filters']['export'] = 0;

    foreach ($header as $i => $value) {
      $out .= '"' . $value . '",';
    }
    $out .= "\n";
    // Add all values in the table
    foreach ($rows as $row) {
      foreach ($row as $i => $value) {
        $out .= '"' . $value . '",';
      }
      $out .= "\n";
    }
    // Output to browser with appropriate mime type, you choose ;)
    header("Content-type: text/x-csv");
    header("Content-Disposition: attachment;filename=orders.csv");
    echo $out;
    exit;
  }
  else {
    $output = drupal_get_form('fundraiser_report_form');
    if ($_SESSION['fundraiser_report_filters']) {
      $output .= '<h3>Date Range: ' . date('m/d/Y', $start_date) . ' - ' . date('m/d/Y', $end_date) . '</h3>';
      $output .= theme('table', array('header' => $header, 'rows' => $rows));
    }
    else {
      $output .= '<h4>To get started, use the filter form to select the dates for your report.</h4>';
    }
    return $output;
  }
}

/**
 * Create a form to filter reporting by month.
 */
function fundraiser_report_form($form) {

  $from_date_default = $_SESSION['fundraiser_report_filters']['from_date'] ? $_SESSION['fundraiser_report_filters']['from_date'] : REQUEST_TIME;
  $to_date_default = $_SESSION['fundraiser_report_filters']['to_date'] ? $_SESSION['fundraiser_report_filters']['to_date'] : REQUEST_TIME;

  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter'),
    '#collapsible' => TRUE,
  );

  $form['filters']['from_date'] = array(
    '#title' => t('From'),
    '#type' => 'date',
    '#description' => t('Select the beginning date of the report.'),
    '#weight' => 1,
    '#default_value' => array(
      'month' => date('n', $from_date_default),
      'day' => date('j', $from_date_default),
      'year' => date('Y', $from_date_default),
    ),
  );

  $form['filters']['to_date'] = array(
    '#title' => t('To'),
    '#type' => 'date',
    '#description' => t('Select the end date of the report.'),
    '#weight' => 2,
    '#default_value' => array(
      'month' => date('n', $to_date_default),
      'day' => date('j', $to_date_default),
      'year' => date('Y', $to_date_default),
    ),
  );

  $form['filters']['export'] = array(
    '#type' => 'checkboxes',
    '#options' => array(1 => 'Export to .csv file.'),
    '#weight' => 3,
  );

  $form['filters']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 4,
  );

  return $form;
}

/**
 * Submit form reporting.
 */
function fundraiser_report_form_submit($form, &$form_state) {

  $export = array_shift($form_state['values']['export']);

  $from_year = $form_state['values']['from_date']['year'];
  $to_year = $form_state['values']['to_date']['year'];

  $from_month = $form_state['values']['from_date']['month'];
  $to_month = $form_state['values']['to_date']['month'];

  $from_day = $form_state['values']['from_date']['day'];
  $to_day = $form_state['values']['to_date']['day'];

  $from_date = mktime(0, 0, 0, $from_month, $from_day, $from_year);
  $to_date = mktime(23, 59, 59, $to_month, $to_day, $to_year);
  // TODO - why are we using session here?
  $_SESSION['fundraiser_report_filters'] = array(
    'from_date' => $from_date,
    'to_date' => $to_date,
    'export' => $export,
  );
}

/**
 * Submit handler updates cron menu handler.
 */
function fundraiser_update_cron_settings($form, $form_state) {
  $cron_enabled = variable_get('fundraiser_standalone_cron_enabled', 0);
  if ($cron_enabled != $form_state['values']['fundraiser_standalone_cron_enabled']) {
    variable_set('fundraiser_standalone_cron_enabled', $form_state['values']['fundraiser_standalone_cron_enabled']);
    // Refresh menu cache.
    menu_rebuild();
  }
}


/**
 * User mapping form.
 */
function fundraiser_mapping_form($form, &$form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'mapping' => array(),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['info'] = array(
    '#value' => 'This form allows you to map the submitted form values to Drupal user and profile fields. When the ' .
      'donation form is submitted, the user\'s profile will be updated with the values from the form.',
    '#weight' => 0,
  );

  foreach ($node->webform['components']   as $cid => $component) {
    if ($component['type'] != 'fieldset') {
      $form['mapping']['cid'][$cid] = array(
      '#type' => 'item',
      '#description' => t($component['name'] . " - (" . $component['type'] . ")"),
      );
      $options = array('' => '--Select One--');
      if (module_exists('profile')) {
        $cat_array = profile_user_categories();
        foreach ($cat_array as $cat) {
          $result = _profile_get_fields($cat['name']);
          foreach( $result as $this_row) {
            $row = (array)$this_row;
            $options['Drupal Profile Fields'][$row['name']] = $row['title'];
          }
        }
      }
      $options['User Fields'] = array(
        'mail' => t("Email Address"),
      );
      $form['mapping']['type'][$cid] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => isset($node->map[$cid]) ? $node->map[$cid] : '',
      );
      $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
    }
  }
  return $form;
}

function fundraiser_mapping_form_submit($form, &$form_state) {
  db_delete('fundraiser_component_map')->condition('nid', $form['#node']->nid)->execute();
  foreach ($form_state['values']['mapping']['type'] as $key => $value) {
    if (!empty($value)) {
      $component_map = array('nid' => $form['#node']->nid, 'cid' => $key, 'map_id' => $value);
      drupal_write_record('fundraiser_component_map', $component_map);
    }
  }
  drupal_set_message(t('Profile mapping has been saved'));
}

function fundraiser_recurring_admin() {
// TODO come back to this, we should be using a theme table here, not constructing our own HTML. -SeH 1/11/12
// TODO signifigant security issues in the previous version of this query, needs to be reviewed on the 6.x branch. - SeH
  $output = "<h3><a href='/admin/fundraiser/recurring/logs'>View Logs</a></h3><table border=1>
  <tr> 
  <th>OrderID</th>
  <th>User Name</th>
  <th>Amount</th>
  <th>Last Charged</th>
  </tr>
  ";

  $results = db_query('SELECT fr.* FROM {fundraiser_recurring} fr '.
    'WHERE fr.order_id = :order_id'.
    'ORDER BY fr.order_id DESC',
  array(':order_id' => arg(3)));

  foreach ($results as $result) {
    // Get the order data from our order handler (Ubercart).
    $order = module_invoke_all('fundraiser_get_order', $result->order_id);
    $order = $order[0];
    $user = user_load($order->uid);
    $fundraiser = (array) $result;
    $fundraiser['fee_amount'] = number_format($fundraiser['fee_amount'], 2, '.', '');
    $last_charge = date("Y-m-d H:i:s", strtotime('-' . '1 months', $fundraiser['next_charge']));
    $output .= "<tr>  
    <td>$fundraiser[order_id]</td>
    <td>$user[name]</td>
    <td>$fundraiser[fee_amount]</td>
    <td>$last_charge</td>
    </tr>";
  }
  $output .= "</table>";
  return $output;
}

function fundraiser_recurring_edit_forms($master_order_id) {
  if (is_numeric($master_order_id)) {
    return _recurring_donation_edit_forms($master_order_id);
  }
}

function _recurring_donation_edit_forms($master_order_id) {
  // we need an unprocessed child order for billing information
  $child_order_id = db_query('SELECT order_id FROM {fundraiser_recurring} '.
    'WHERE master_order_id = :order_id and gateway_resp IS NULL LIMIT 1',
    array(':order_id' => $master_order_id))->fetchColumn();

  // Get the order data from our order handler (Ubercart).
  $order = module_invoke_all('fundraiser_get_order', $order_id);
  $order = $order[0];
  $nid = $order->products[0]->nid;
  $output = '<div>' . _get_billing_information($master_order_id, $order) . '</div>';

  // Render multiple forms
  $output .= drupal_get_form('fundraiser_donation_amount_form', $master_order_id, $nid);

  // 4/5/2010 - commented out for time being because authorize.net does not support updating
  //             CIM profiles at this time
  // $output .= drupal_get_form('fundraiser_billing_update_form', $master_order_id, $order);

  $output .= drupal_get_form('fundraiser_cancel_form', $master_order_id);

  return $output;
}

function _get_billing_information($master_order_id, $order) {
  $state = module_invoke_all('fundraiser_get_country_zone', $order->billing_zone);
  $state = $state[0];
  $orders_remaining = _remaining_recurring_charge_count($master_order_id);
  $orders_processed = _processed_recurring_charge_count($master_order_id);
  $donation_amount = '$' . money_format('%i', $order->order_total);
// TODO come back to this - shouldn't this be in a themed template display? - SeH 1/11/12
  $billing_str = "";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Bill to:</div>";
  $billing_str .= "$order->billing_first_name $order->billing_last_name<br />";
  $billing_str .= "$order->billing_street1<br />";
  if (!empty($order->billing_street2)) {
    $billing_str .= "$order->billing_street2<br />";
  }
  $billing_str .= "$order->billing_city, $state $order->billing_postal_code</div>";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Customer Info:</div>";
  $billing_str .= "Customer Number: <a href='/user/$order->uid'>$order->uid</a><br />";
  $billing_str .= "Primary email:<br />$order->primary_email</div>";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Recurring Payment Info:</div>";
  $billing_str .= "Recurring Donation Amount: $donation_amount<br />";
  $billing_str .= "Recurring Charges Processed: $orders_processed <br />";
  $billing_str .= "Recurring Charges Remaining: $orders_remaining <br /></div>";
  $billing_str .= "<div style='clear:both;'>&nbsp;</div>";
  return $billing_str;
}

function fundraiser_recurring_master_order_overview($order_id) {
// TODO come back to this, we should be using a theme table here, not constructing our own HTML. -SeH 1/11/12
  $output = "<h2>Recurring Orders</h2>";
  $output .= "<table><thead>
  <tr>
  <th>Master Order ID</th>
  <th>OrderID</th>
  <th>User Name</th>
  <th>Amount</th>
  <th>Charge Date</th>
  </tr></thead>
  ";
  if (empty($rows[1])) {
    //we're actually looking at the order id and not the master_order_id
    $results = db_query('SELECT fr.* FROM {fundraiser_recurring} fr ' .
      'WHERE master_order_id = (SELECT fr2.master_order_id from {fundraiser_recurring} fr2 where fr2.order_id=:order_id) ' .
      'ORDER BY fr.order_id ASC',
    array(':order_id' => $order_id));
    unset($rows);
    foreach ($results as $result) {
      // Get the order data from our order handler (Ubercart).
      $order = module_invoke_all('fundraiser_get_order', $result->order_id);
      $order = $order[0];
      $user = user_load($order->uid);
      $result->order_total = $order->order_total;
      $result->name = $user->name;
      $rows[] = (array) $result;
    }
  }

  for ($i = 1; $i < count($rows); $i++) {
    if (($i % 2) == 0) {
      $class = 'even';
    }
    else {
      $class = 'odd';
    }
    $rows[$i]['fee_amount'] = number_format($rows[$i]['order_total'], 2, '.', '');
    if ($rows[$i]['gateway_resp'] == 'canceled') {
      $next_charge = 'Canceled';
    }
    else {
      $next_charge = date("Y-m-d H:i:s", $rows[$i]['next_charge']);
    }
    $output .= "<tr class='$class'>
    <td><a href='/admin/store/orders/" . $rows[$i]['master_order_id'] . "/recurring/edit'>" . $rows[$i]['master_order_id'] . "</a></td>
    <td>" . $rows[$i]['order_id'] . "</td>
    <td>" . $rows[$i]['name'] . "</td>
    <td>" . $rows[$i]['fee_amount'] . "</td>
    <td>$next_charge</td>
    </tr>";
  }
  $output .= "</table>";
  return $output;
}

/**
 * Refund tab on Orders
 */
function fundraiser_show_refund_form($order_id) {
  $base = drupal_get_path('module', 'fundraiser');
  drupal_add_css($base . '/css/refund.css');
  drupal_add_js($base . '/js/refund.js');
  return drupal_get_form('fundraiser_refund', $order_id);
}

/**
 * Refund form
 */
function fundraiser_refund(&$form_state, $order_id) {
  $form = array();
  $order = module_invoke_all('fundraiser_get_order', $order_id);
  $order = $order[0];
  $form['#order_id'] = $order->order_id;

  // Determine how much has been paid, less previous refunds.
  $total_paid = module_invoke_all('fundraiser_get_order_paid', $order->order_id);
  $total_paid = $total_paid[0];

  // Get payment info
  $payment_info = module_invoke_all('fundraiser_get_order_payment_info', $order->order_id);
  $payment_info = $payment_info[0];

  // Get refund info
  $previous_refunds = module_invoke_all('fundraiser_get_order_refund_info', $order->order_id);
  $previous_refunds = $previous_refunds[0];

  $form['payment_summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Summary'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  if (!$payment_info) {
    $form['payment_summary']['no_payments'] = array(
      '#value' => 'No payments have been made on this order yet',
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );

  }
  else {
    $form['payment_summary']['payment_number'] = array(
      '#type' => 'item',
      '#title' => t('Payment #'),
      '#value' => $payment_info['receipt_id'],
    );
    $formated_price = module_invoke_all('fundraiser_format_price', $payment_info['received']);
    $form['payment_summary']['payment_date'] = array(
      '#type' => 'item',
      '#title' => t('Payment Date'),
      '#value' => $formated_price[0],
    );
    $formated_price = module_invoke_all('fundraiser_format_price', $payment_info['amount']);
    $form['payment_summary']['payment_total'] = array(
      '#type' => 'item',
      '#title' => t('Payment Total'),
      '#value' => $formated_price[0],
    );
    if ($previous_refunds < 0) {
      $formated_price = module_invoke_all('fundraiser_format_price', $previous_refunds);
      $form['payment_summary']['total_refunded'] = array(
        '#type' => 'item',
        '#title' => t('Total Previously Refunded'),
        '#value' => $formated_price[0],
      );
    }
  }

  $form['refund_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Refund Options'),
    '#description' => t('Note that refunds can only be applied to payments that have been settled in the payment processor. This typically ' .
     'happens every night; if the payment was made today, you will probably need to wait until tomorrow to issue a refund.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Only show the refund form if the payment was made through a payment gateway that can handle refunds
  $can_refund = fundraiser_gateway_can_refund_order($order);
  if (!$can_refund || !$payment_info) {
    // They've made a payment, but it cannot be refunded
    if ($payment_info) {
      $no_refund_text = 'Refunds cannot be made on this order. The payment was made on a payment gateway that does not support refunds.';
    }
    // They have NOT made any payment yet, so there's nothing to refund
    else {
      $no_refund_text = 'No payments have been processed on this order, so no refunds can be made yet.';
      // Even once the payment is made, they won't be able to refund it
      if (!$can_refund) {
        $no_refund_text .= ' However, because this order was made on a payment gateway that does not support refunds, you will not be able to ' .
          'make refunds after payments are processed on this order.';
      }
    }
    $form['refund_options']['no_refund_explanation'] = array(
      '#value' => $no_refund_text,
      '#prefix' => '<strong>',
      '#suffix' => '</strong>',
    );
  }
  // Refund options
  else {
    $formated_price = module_invoke_all('fundraiser_format_price', $total_paid);
    $form['refund_options']['full_refund'] = array(
      '#type' => 'radio',
      '#title' => t('Full Refund (%amount)', array('%amount' => $formated_price[0])),
      '#return_value' => 'full',
      '#parents' => array('refund_type'),
    );
    $form['refund_options']['partial_refund'] = array(
      '#type' => 'radio',
      '#title' => t('Partial Refund'),
      '#return_value' => 'partial',
      '#parents' => array('refund_type'),
    );

    $form['refund_options']['amount'] = array(
      '#type' => 'textfield',
      '#title' => t('Amount to Refund'),
      '#size' => 40,
      '#maxlength' => 255,
    );
    $form['refund_options']['refund_notes'] = array(
      '#type' => 'textarea',
      '#title' => t('Refund Notes'),
      '#description' => t('Please provide reference information about this refund (Requestor, reason, etc.)'),
      '#cols' => 60,
      '#rows' => 5,
      '#required' => TRUE,
    );
    $form['refund_options']['issue_refund'] = array(
      '#type' => 'submit',
      '#value' => t('Issue Refund'),
      '#required' => TRUE,
    );

    $form['total_paid'] = array(
      '#type' => 'hidden',
      '#value' => number_format($total_paid, 2),
    );
  }

  return $form;
}


/**
 * Validate the form
 */
function fundraiser_refund_validate($form, &$form_state) {
  // Make sure they chose a refund type
  if (empty($form_state['values']['refund_type'])) {
    form_set_error('refund_type', t('Choose a refund type.'));
  }
  // If it isn't a full refund, make sure they entered a valid value
  elseif ($form_state['values']['refund_type'] != 'full') {
    $amount = (float) $form_state['values']['amount'];
    if (!($amount > 0)) {
      form_set_error('amount', t('Enter a refund amount.'));
    }
    else {
      // Make sure they don't try to refund more than was paid
      $order_id = $form['#order_id'];
      // Determine how much has been paid, less previous refunds
      $total_paid = module_invoke_all('fundraiser_get_order_paid', $order_id);
      $total_paid = $total_paid[0];
      if ($amount > $total_paid) {
        $context = array(
          'revision' => 'formatted-original',
          'type' => 'amount',
        );
        $formated_price = module_invoke_all('fundraiser_format_price',$total_paid);
        form_set_error('amount', t('The refund amount entered is too high. Only @paid in payments have been made, and no ' .
          'more than that can be refunded.', array('@paid' => $formated_price[0])));
      }
    }
  }
}

/**
 * Submit the refund form
 */
function fundraiser_refund_submit($form, &$form_state) {
  // Get the order.
  $order_id = $form['#order_id'];

  // Determine how much has been paid, less previous refunds
  $total_paid = module_invoke_all('fundraiser_get_order_paid', $order_id);
  $total_paid = $total_paid[0];

  // Partial refund
  if ($form_state['values']['refund_type'] != 'full') {
    $new_order_status = 'partially_refunded';
    $amount = (float) $form_state['values']['amount'];
    // Can't refund more than they've paid
    // Use >= so that the status is set properly if they choose Partial Refund
    // but refund the entire amount
    if ($amount >= $total_paid) {
      $amount = $total_paid;
      $new_order_status = 'refunded';
    }
  }
  // Full refund
  else {
    $new_order_status = 'refunded';
    $amount = $total_paid;
  }

  // Make the charge amount negative
  $amount = abs($amount) * -1;

  // Get details on the original payment so we can load the payment gateway details
  $payment_details = db_query('SELECT gateway, txn_id FROM {fundraiser_webform_order} ' .
    'WHERE order_id = :order_id ' .
    'UNION '.
    'SELECT gateway, txn_id FROM {fundraiser_recurring} '. 
    'WHERE order_id = :order_id', array(':order_id' => $order_id))->fetchObject();

  if (module_exists('gateway_manager')) {
    $info = gateway_manager_gateway_info($payment_details->gateway);
    $refund_function = $info['refund_function'];
    $data['config'] = $info['config'];
  }
  else {
    $refund_function = fundraiser_refund_function($node->gateway);
  }

  // call the refund function and store the result
  $result = $refund_function($order_id, $amount, $data);

  if ($result['success']) {

    $receipt_id = module_invoke_all('fundraiser_order_refund', $order_id, $amount, $new_order_status, $result['data']['txn_id'], $form_state['values']['refund_notes']);
    $receipt_id = $receipt_id[0];

    // Save refund data for Fundraiser.
    $refund = new stdClass();
    $refund->order_id = $order_id;
    $refund->receipt_id = $receipt_id;
    $refund->amount = $amount;
    $refund->txn_id = $result['data']['txn_id'];
    $refund->reason = $reason;
    drupal_write_record('fundraiser_refund', $refund);

    // If using the Salesforce queue, queue up the updates
    // This could be on hook couldn't it? - SeH
    if (module_exists('queue_api')) {
      // Update the original order
      sf_queue_insert($order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
      // Create the refund
      sf_queue_insert($refund->refund_id, FUNDRAISER_DONATION_REFUND_TYPE, 'create');
    }

    drupal_set_message(t('Refund has been issued.'), 'status');
    drupal_goto('admin/store/orders/' . $order_id);
  }
  else {
    // failed to process credit cart
    $log_error = 'Refund on order ' . $order_id . ' failed gateway validation. Reason: ' . $result['message'];
    watchdog('fundraiser', $log_error, NULL, WATCHDOG_DEBUG, NULL);
    drupal_set_message($result['message']);
    $form_state['rebuild'] = TRUE;
    $form_state['values']['abort'] = TRUE;
  }
}
