<?php
/**
 * Functions for the Commerce Payflow Credit Card payment method
 */

/**
 * Payment method callback: settings form
 */
function fundraiser_commerce_payflow_cc_settings_form($settings = array()) {
  // Grab the default Payflow Link settings.
  $settings += commerce_payflow_link_default_settings();

  // Load the form from the default settings
  $form = commerce_payflow_link_settings_form($settings, commerce_payment_method_load('fundraiser_commerce_payflow_cc'));

  // Update the currency options
  $form['currency_code']['#options'] = commerce_paypal_currencies('payflow_link');

  // Hide the elements we don't need
  $hidden = array(
    'redirect_mode',
    'payment_icons',
    'cancel_link',
    'paypal_ec_instance',
    'paypal_ec_disable',
    'show_payment_instructions',
    'ba_desc',
  );

  foreach ($hidden as $hide) {
    $form[$hide]['#access'] = FALSE;
  }

  return $form;
}

/**
 * Payment method callback: checkout form.
 */
function fundraiser_commerce_payflow_cc_submit_form($payment_method, $pane_values, $checkout_pane, $order) {
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
  return commerce_payment_credit_card_form(array('code' => ''));
}

/**
 * Payment method callback: checkout form validation.
 */
function fundraiser_commerce_payflow_cc_submit_form_validate($payment_method, $pane_form, $pane_values, $order, $form_parents = array()) {
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');

  // Validate the credit card fields.
  $settings = array(
    'form_parents' => array_merge($form_parents, array('credit_card')),
  );

  if (!commerce_payment_credit_card_validate($pane_values['credit_card'], $settings)) {
    return FALSE;
  }
}

/**
 * Payment method callback: checkout form submission.
 */
function fundraiser_commerce_payflow_cc_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge) {
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Build a name-value pair array for this transaction.
  $nvp = array(
    // A - Authorize, S - Auth and Capture
    'TRXTYPE' => $payment_method['settings']['trxtype'],
    // C - Credit card
    'TENDER'  => 'C',
    // The total price
    'AMT'     => commerce_currency_amount_to_decimal($charge['amount'], $charge['currency_code']),
    // The currency code
    'CURRENCY' => $charge['currency_code'],
    // The credit card number
    'ACCT'    => $pane_values['credit_card']['number'],
    // The expiration date, in format 'mmyy'
    'EXPDATE' => sprintf("%02d", $pane_values['credit_card']['exp_month']) . substr($pane_values['credit_card']['exp_year'], 2),
    // The order number
    'INVNUM' => $order->order_number,
  );
  if (isset($pane_values['credit_card']['code'])) {
    $nvp['CVV2'] = $pane_values['credit_card']['code'];
  }

  // Build a description for the order.
  $description = array();
  foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
    if (in_array($line_item_wrapper->type->value(), commerce_product_line_item_types())) {
      $description[] = round($line_item_wrapper->quantity->value()) . 'x ' . $line_item_wrapper->line_item_label->value();
    }
  }

  $descr = variable_get('site_name');
  if (!empty($description)) {
    $descr .= ': ' . implode(', ', $description);
  }

  $nvp['COMMENT1'] = substr($descr, 0, 127);
  if (strlen($descr) > 127) {
    $nvp['COMMENT2'] = substr($descr, 128, 255);
  }

  // Prepare the billing address for use in the request.
  $billing_address = $order_wrapper->commerce_customer_billing->commerce_customer_address->value();

  if (empty($billing_address['first_name'])) {
    $name_parts = explode(' ', $billing_address['name_line']);
    $billing_address['first_name'] = array_shift($name_parts);
    $billing_address['last_name'] = implode(' ', $name_parts);
  }

  // Add customer information
  $nvp += array(
    'FIRSTNAME'     => substr($billing_address['first_name'], 0, 30),
    'LASTNAME'      => substr($billing_address['last_name'], 0, 30),
    'COMPANYNAME'   => substr($billing_address['organisation_name'], 0, 30),
    'STREET'        => substr($billing_address['thoroughfare'], 0, 30),
    'CITY'          => substr($billing_address['locality'], 0, 30),
    'STATE'         => substr($billing_address['administrative_area'], 0, 30),
    'ZIP'           => substr($billing_address['postal_code'], 0, 12),
    'COUNTRY'       => substr($billing_address['country'], 0, 2),
    'EMAIL'         => substr($order->mail, 0, 255),
    'CUSTCODE'      => $order->uid,
    'CUSTIP'        => ip_address(),
  );

  // Submit the request
  $response = commerce_payflow_api_request($payment_method, 'pro', $nvp, $order);
  
  // Something went wrong with the request. Abort.
  if (!$response) {
    drupal_set_message(t('We are unable to process your request at this time. Please try again.'), 'error');
    return FALSE;  
  }

  // Prepare a transaction object to log the API response.
  $transaction = commerce_payment_transaction_new($payment_method['method_id'], $order->order_id);
  $transaction->instance_id = $payment_method['instance_id'];
  $transaction->remote_id = $response['PNREF'];
  $transaction->amount = $charge['amount'];
  $transaction->currency_code = $charge['currency_code'];
  $transaction->payload[REQUEST_TIME] = $response;

  $trxtype = $payment_method['settings']['trxtype'];
  // Store the type of transaction in the remote status.
  $transaction->remote_status = $trxtype;

  $trxtype_name = commerce_payflow_trxtype_name($trxtype);

  // Build the response message.
  $message = array(
    '<b>Response: @response</b>',
  );

  $transaction->message_variables = array(
    '@response' => $response['RESPMSG'],
    '@action' => $trxtype_name,
  );

  if (intval($response['RESULT']) == 0) {
    // Set the transaction status based on the type of transaction this was.
    switch ($trxtype) {
      case 'S':
        $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
        break;

      case 'A':
      default:
        $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
        break;
    }

    $message[] = '@action - Success';
    $message[] = 'Authcode: @authcode';
    $transaction->message_variables['@authcode'] = $response['AUTHCODE'];
  }
  elseif (intval($response['RESULT']) == 126) {
    $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
    $message[] = '@action - Pending fraud review';
  }
  else {
    $message[] = '@action - REJECTED';
  }

  // Add the CVV response if enabled.
  if (isset($response['CVV2MATCH'])) {
    $message[] = 'CVV2 match: @cvv';
    $transaction->message_variables['@cvv'] = commerce_paypal_cvv_match_message($response['CVV2MATCH']);
  }

  // Add the AVS response if present.
  if (!empty($$response['AVSADDR']) || !empty($response['AVSZIP']) || !empty($response['AVSDATA'])) {
    if (!empty($response['AVSDATA'])) {
      $avsdata[] = commerce_paypal_avs_code_message($response['AVSDATA']);
    }
    else {
      $avsdata = '';

      if (!empty($response['AVSADDR'])) {
        $avsdata[] = commerce_paypal_avs_code_message($response['AVSADDR']);
      }

      // Add this if it isn't the same as above
      if (!empty($response['AVSZIP']) && $response['AVSZIP'] != $response['AVSADDR']) {
        $avsdata[] = commerce_paypal_avs_code_message($response['AVSZIP']);
      }
    }

    $message[] = 'AVS response: @avs';
    $transaction->message_variables['@avs'] = implode(', ', $avsdata);
  }

  $transaction->message = implode('<br />', $message);

  // Store additional data in the data array
  $transaction->data['commerce_payflow']['pnref'] = $response['PNREF'];

  // Save the transaction information.
  commerce_payment_transaction_save($transaction);

  // If the payment failed, display an error and rebuild the form.
  if ($response['RESULT'] != '0') {
    drupal_set_message(t('We received the following error processing your card. Please enter you information again or try a different card.'), 'error');
    drupal_set_message(check_plain($response['RESPMSG']), 'error');
    return FALSE;
  }

  return $transaction->transaction_id;
}
