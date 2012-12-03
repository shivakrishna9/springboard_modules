<?php
/**
 * @file
 * Administration page callbacks for the fundraiser module
 */

/**
 * Form for fundraiser confirmation settings.
 */
function fundraiser_confirmation_settings_form(&$form_state, $node) {
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
    '#default_value' => $node->confirmation_page_title,
  );
  $form['confirmation_page']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Confirmation message'),
    '#default_value' => $node->confirmation_page_body,
  );
  $body_format = $node->confirmation_page_format ? $node->confirmation_page_format : FILTER_FORMAT_DEFAULT;
  $form['confirmation_page']['format'] = filter_form($body_format, NULL, array('confirmation_page_format'));

  $form['confirmation_page']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['confirmation_page']['tokens']['help'] = array(
    '#value' => theme('token_help', 'order') . theme('webform_token_help', $node),
  );

  // Redirection settings.
  if (strpos($node->redirect_url, '<') === 0) {
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
    $redirect_url = $node->redirect_url;
  }
  $form['confirmation_page']['redirection'] = array(
    '#type' => 'item',
    '#title' => t('Redirection location'),
    '#theme' => 'webform_advanced_redirection_form',
    '#description' => t('Choose where to redirect the user upon successful submission.'),
  );
  $form['confirmation_page']['redirection']['redirect']= array(
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
    '#title'=> t('Receipt Email Address'),
    '#description' => t('The email address that the receipt email will come from.'),
    '#weight' => -4.6,
    '#maxlength' => 255,
    '#default_value' => $node->receipt_email_address,
    '#required' => TRUE,
  );
  $form['email']['receipt_email_from'] = array(
    '#type' => 'textfield',
    '#title'=> t('Receipt Email From'),
    '#description' => t('The name that is associated with the receipt email address.'),
    '#weight' => -4.5,
    '#default_value' => $node->receipt_email_from,
    '#required' => TRUE,
  );
  $form['email']['receipt_email_subject'] = array(
    '#type' => 'textfield',
    '#title'=> t('Receipt Email Subject'),
    '#description' => t('The subject of the donation receipt email.'),
    '#weight' => -4.4,
    '#default_value' => $node->receipt_email_subject,
    '#required' => TRUE,
  );
  $form['email']['receipt_email_message'] = array(
    '#type' => 'textarea',
    '#title'=> t('Confirmation Email Message'),
    '#weight' => -4.3,
    '#default_value' => $node->receipt_email_message,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 300,
  );

  return $form;
}

function fundraiser_confirmation_settings_form_validate($form, &$form_state) {
  // validate email
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
 * Clones a donation form and related fieldmaps
 *
 * @param $nid
 * Integer Node ID
 */
function fundraiser_clone_donation_form($nid) {
  // hackish, gets around issues with nid argument not getting passed by menu handler.
  if (!is_numeric($nid)) {
    $nid = arg(1);
  }

  if (is_numeric($nid)) {
    global $user;
    $node = node_load($nid);
   // drupal_set_message('node to clone:' . print_r($node, TRUE));
    if (isset($node->nid)) {
      $original_node = drupal_clone($node);

      $node->nid = NULL;
      $node->vid = NULL;
      $node->name = $user->name;
      $node->uid = $user->uid;
      $node->created = NULL;
      $node->menu = NULL;
      $node->book['mlid'] = NULL;
      $node->path = NULL;
      $node->files = array();
      $node->title = t('Clone of !title', array('!title' => $node->title));
      $node->internal_name = 'Clone of ' . $original_node->internal_name;
      $node->is_being_cloned = true;

      if (variable_get('clone_reset_'. $node->type, FALSE)) {
        $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
        // Fill in the default values.
        foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
          $node->$key = in_array($key, $node_options);
        }
      }
      node_save($node);
      $new_nid = $node->nid;

      // get profile map for the original donation form
      $result = db_query("SELECT f.nid, f.cid, f.map_id, c.form_key
        FROM {fundraiser_component_map} f
        INNER JOIN {webform_component} c
          ON c.cid = f.cid
          AND c.nid = f.nid
        WHERE f.nid = %d", $nid);

      // update confirmation information
      db_query("UPDATE {fundraiser} SET confirmation_page_title = '%s', confirmation_page_body = '%s', redirect_url = '%s' WHERE nid = %d", $original_node->confirmation_page_title, $original_node->confirmation_page_body, $original_node->redirect_url, $new_nid);

      // loop over mapped components and find new component in cloned donation form
      while ($data = db_fetch_object($result)) {
        $cid = db_result(db_query("SELECT cid FROM {webform_component} WHERE form_key = '%s' AND nid = %d", $data->form_key, $new_nid));

        if ($cid) {
          db_query("INSERT INTO {fundraiser_component_map} (nid, cid, map_id) VALUES (%d, %d, '%s')", $new_nid, $cid, $data->map_id);
        }
      }

      if (module_exists('sf_donation')) {
        sf_donation_clone_map($nid, $new_nid);
       }

       module_invoke_all('fundraiser_clone_donation_form', $original_node->nid, $node->nid);

       drupal_set_message("Donation form [$original_node->title] has been cloned succesfully.");
       drupal_goto('node/'. $new_nid .'/edit');
    }
  }
}


/**
 * Form builder. Configure fundraiser.
 *
 */
function fundraiser_admin_settings() {

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
        if (variable_get('fundraiser_all_countries', 0)) {
    $countries = uc_country_select(uc_get_field_name('country'), $us, NULL, 'name', uc_address_field_required('country'));
          $country_options = array_merge(array('0' => t('- select country -')),$countries['#options']);
        }
        else {
          $country_options = array(
             '0' => t('- select country -'),
             '840' => t('United States'),
             '124' => t('Canada'),
          );
        }

        $form['fundraiser_default_country'] = array(
          '#type' => 'select',
          '#title' => t('Default country'),
          '#description' => t('Select the default value for the billing country field on new donation forms.'),
          '#options' => $country_options,
          '#default_value' => variable_get('fundraiser_default_country', ''),
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

  $form['fundraiser_cc_exp_year_range'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit card expiration year range'),
    '#description' => t('Specify the number of years from now that will be the maximum credit card expiration year.'),
    '#default_value' => variable_get('fundraiser_cc_exp_year_range', 10),
  );

  $form['fundraiser_cc_expiration_message']['fundraiser_sustainers_send_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send expiration email'),
    '#description' => t('If this option is enabled then emails will be sent when a card is about to expire.'),
    '#default_value' => variable_get('fundraiser_sustainers_send_email', TRUE),
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

  $form['fundraiser_http_error'] = array(
    '#type' => 'fieldset',
    '#title' => t('Unsecured Form Error'),
    '#description' => t('Configure how Fundraiser responds when a user attempts to view a donation form unsecured (HTTP)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fundraiser_http_error']['fundraiser_http_error_message'] = array(
    '#type' => t('textarea'),
    '#title' => t('Error Message'),
    '#description' => t('This message will be displayed to users that attempt to view a donation form that is unsecured.'),
    '#default_value' => variable_get('fundraiser_http_error_message', t("We're sorry.

The donation form you are trying to access has not been secured properly. Safeguarding your personal information is extremely importance to us, therefore we cannot accept donations from this form at this time.

An administrator has been notified and will correct the issue as soon as possible.

Thanks for your understanding.")),
  );

  return system_settings_form($form);
}

/*
 * Creates a list of all donation forms on the site
 */
function fundraiser_donation_form_list() {
  global $user;
  $result = db_query("SELECT n.nid, n.uid, n.title, f.internal_name, n.created, n.changed FROM {node} n INNER JOIN {fundraiser} f ON f.nid = n.nid ORDER BY n.changed DESC");

  $header = array('Title', 'Internal Name', 'Last Modified', '', '', '');
  $rows = array();

  while ($data = db_fetch_object($result)) {
    if (user_access('edit any donation form', $user) || ($data->uid == $user->uid && user_access('edit own donation form', $user))) {
      $edit_link = l('edit', 'node/' . $data->nid . '/edit');
    }
    $view_link = l('view', drupal_get_path_alias('node/' . $data->nid));
    if (user_access('clone donation form', $user)) {
      $clone_link = l('clone', 'node/' . $data->nid . '/clone_form');
    }
    $rows[] = array($data->title, $data->internal_name, date('m/d/Y g:i:s a', $data->changed), $edit_link, $view_link, $clone_link);
  }

  $output = theme('table', $header, $rows);
  return $output;
}

function fundraiser_monthly_report() {
  global $base_url;
  if ($_SESSION['fundraiser_report_filters']) {
    $start_date = $_SESSION['fundraiser_report_filters']['from_date'];
    $end_date = $_SESSION['fundraiser_report_filters']['to_date'];
    $sql = "select
              u.order_id,
              u.uid,
              u.order_status,
              s.sfid,
              u.billing_first_name,
              u.billing_last_name,
              u.primary_email,
              u.billing_street1,
              u.billing_street2,
              u.billing_city,
              u.data,
              z.zone_code,
              u.billing_postal_code,
              c.country_iso_code_2,
              u.created as date,
              u.order_total,
              p.received,
              op.nid,
              op.title,
              fd.campaign_id,
              fd.market_source
            from
              uc_orders u inner join fundraiser_webform_order s on s.order_id = u.order_id
              inner join {uc_payment_receipts} p on p.order_id = u.order_id
              inner join {uc_countries} c on c.country_id = u.billing_country
              inner join {uc_zones} z on z.zone_id = u.billing_zone
              inner join {uc_order_products} op on op.order_id = u.order_id
              inner join {fundraiser_webform_order} wo on wo.order_id = u.order_id
              inner join {fundraiser_webform_data} fd on fd.sid = wo.sid and fd.nid = wo.webform_nid
              where u.order_status = 'payment_received' and u.created between %d and %d
            union select
              r.order_id,
              u.uid,
              u.order_status,
              r.sf_opportunity_id,
              u.billing_first_name,
              u.billing_last_name,
              u.primary_email,
              u.billing_street1,
              u.billing_street2,
              u.billing_city,
              u.data,
              z.zone_code,
              u.billing_postal_code,
              c.country_iso_code_2,
              next_charge,
              u.order_total,
              p.received,
              op.nid,
              op.title,
              fd.campaign_id,
              fd.market_source
            from fundraiser_recurring r inner join uc_orders u on u.order_id = r.order_id
              inner join {uc_payment_receipts} p on p.order_id = u.order_id
              inner join {uc_countries} c on c.country_id = u.billing_country
              inner join {uc_zones} z on z.zone_id = u.billing_zone
              inner join {uc_order_products} op on op.order_id = u.order_id
              inner join {fundraiser_webform_recurring_order} wo on wo.order_id = u.order_id
              inner join {fundraiser_webform_data} fd on fd.sid = wo.sid and fd.nid = wo.nid
              where u.order_status = 'payment_received' and r.next_charge between %d and %d order by order_id";
    $result = db_query($sql, $start_date, $end_date, $start_date, $end_date);
  }

  $header = array('Order Id', 'Opportunity Id', 'Uid', 'First Name', 'Last Name', 'Email', 'Address', 'Address 2', 'City', 'Country', 'State/Province', 'Postal Code', 'Payment Date', 'Amount', 'Donation Form', 'URL', 'Market Source', 'Campaign ID', 'CC Last Four', 'CC Type', 'CC Exp Month', 'CC Exp Year');
  $columns = array('order_id', 'sfid', 'billing_first_name', 'billing_last_name', 'primary_email', 'billing_street1', 'billing_street2', 'billing_city', 'country_iso_code_2', 'zone_code', 'received', 'order_total', 'title', 'nid');

  $rows = array();
  while ($data = db_fetch_object($result)) {
    $cc_data = decrypt_credit_card($data->data);
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
      '$'.number_format($data->order_total, 2),
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

    $out .="\n";

    // Add all values in the table
     foreach ($rows as $row) {
       foreach ($row as $i => $value) {
           $out .= '"' . $value . '",';
         }
       $out .="\n";
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
      $output .= theme('table', $header, $rows);
    }
    else {
      $output .= '<h4>To get started, use the filter form to select the dates for your report.</h4>';
    }
    return $output;
  }
}

function fundraiser_report_form() {

  $from_date_default = $_SESSION['fundraiser_report_filters']['from_date'] ? $_SESSION['fundraiser_report_filters']['from_date'] : time();
  $to_date_default = $_SESSION['fundraiser_report_filters']['to_date'] ? $_SESSION['fundraiser_report_filters']['to_date'] : time();

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

  $_SESSION['fundraiser_report_filters'] = array(
    'from_date' => $from_date,
    'to_date' => $to_date,
    'export' => $export,
  );
}

// Utility function to decrypt stored credit card information
function decrypt_credit_card($data) {
  $key = uc_credit_encryption_key();
  $crypt = new uc_encryption_class;
  $cc = unserialize($data);
  $cc_data = unserialize($crypt->decrypt($key, $cc['cc_data']));
  $cc_data['cc_number'] = substr($cc_data['cc_number'], -4);
  return $cc_data;
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
