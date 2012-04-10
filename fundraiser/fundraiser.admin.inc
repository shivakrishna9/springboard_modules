<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Form builder. Configure fundraiser module.
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

  // TODO Check gthis ot make sure these are the right tokens to display given the radical changes in this par tof D7. - SeH
  $form['fundraiser_cc_expiration_message']['tokens']['user'] = array(
    '#markup' => theme('token_tree', array('token_types' => array('user'))),
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

//TODO this may need to be moved into a sustainer module to provide sustainer ui to recurring donations.
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

// TODO move to ubercart.
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

// TODO move to ubercart.
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
