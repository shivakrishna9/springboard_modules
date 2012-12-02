<?php

/**
 * @file
 * Functions and hooks defined by hook_payment_gateway_managed().
 */


/**
 * Payment callback for credit cards on the test gateway.
 */
function gm_test_gateway_credit_charge($order_id, $amount, $data) {
  global $user;
  $order = uc_order_load($order_id);

  // Use 0000000000000000 to test a failed payment, anything else for a good one.
  if ($order->payment_details['cc_number'] == '0000000000000000') {
    $success = FALSE;
  }
  else {
    $success = TRUE;
  }

  if ($success) {
    $context = array(
      'revision' => 'formatted-original',
      'type' => 'amount',
    );
    $message = t('Credit card charged: !amount', array('!amount' => uc_price($amount, $context)));
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }
  else {
    $message = t('Credit card charge failed.');
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }

  $result = array(
    'success' => $success,
    'comment' => t('Card charged, resolution code: 0022548315'),
    'message' => $success ? t('Credit card payment processed successfully.') : t('Credit card charge failed.'),
    'uid' => $user->uid,
    'data' => array('txn_id' => 'TEST-TXNID-001'),
    'error type' => ($success) ? '' : 'billing',
  );

  return $result;
}

/**
 * Webform render callback for credit cards on the test gateway.
 */
function gm_test_gateway_credit_webform_render($form, &$form_state) {
  // Include generic credit card functionality
  module_load_include('inc', 'gateway_manager', 'includes/gateway_manager.credit');
  return gateway_manager_credit_form_render($form, $form_state);
}

/**
 * Payment details callback for credit cards on the test gateway.
 */
function gm_test_gateway_credit_payment_details(&$order, $submitted) {
  // Include generic credit card functionality.
  module_load_include('inc', 'gateway_manager', 'includes/gateway_manager.credit');
  return gateway_manager_credit_payment_details($order, $submitted);
}

/**
 * Validate callback for credit cards on the test gateway.
 */
function gm_test_gateway_credit_webform_validate($form, &$form_state) {
  dpm('yup');
}

/**
 * Payment callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_charge($order_id, $amount, $data) {
  // For demo purposes only.
  drupal_goto('http://www.paypal.com');
}

/**
 * Webform render callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_webform_render($form, &$form_state) {
  $paypal_form['message'] = array(
    '#value' => '<p>After clicking submit you will be redirected to paypal to complete your tranasaction.</p><img src="https://www.paypalobjects.com/webstatic/mktg/logo/AM_mc_vs_dc_ae.jpg" border="0" alt="PayPal Acceptance Mark">',
  );

  // Allow other modules to alter the form via hook_paypal_payment_form_alter()
  drupal_alter('paypal_payment_form', $paypal_form, $form, $form_state);
  return $paypal_form;
}

/**
 * Payment details callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_payment_details(&$order, $submitted) {
  $payment_details =  array(
    'paypal' => t('Order passed to paypal for processing.'),
  );

  // Allow other modules to alter the payment details via hook_payment_details_alter()
  drupal_alter('payment_details', $payment_details, 'paypal', $order);
  return $payment_details;
}

/**
 * Payment callback for direct debit on the test gateway.
 */
function gm_test_gateway_direct_debit($order_id, $amount, $data) {
  global $user;
  $order = uc_order_load($order_id);

  // Use 0 to test a failed payment, anything else for a good one.
  if ($order->payment_details['direct_debit_bank_account_number'] == '0') {
    $success = FALSE;
  }
  else {
    $success = TRUE;
  }

  if ($success) {
    $context = array(
      'revision' => 'formatted-original',
      'type' => 'amount',
    );
    $message = t('Direct debit amount: !amount', array('!amount' => uc_price($amount, $context)));
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }
  else {
    $message = t('Direct debit failed.');
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }

  $result = array(
    'success' => $success,
    'comment' => t('Direct debit'),
    'message' => $success ? t('Direct debit processed successfully.') : t('Direct debit failed.'),
    'uid' => $user->uid,
    'data' => array('txn_id' => $payment_details['direct_debit_bank_name']),
    'error type' => ($success) ? '' : 'billing',
  );

  return $result;
}

/**
 * Webform render callback for direct debit on the test gateway.
 */
function gm_test_gateway_direct_debit_render($form, &$form_state) {
  $direct_debit_form['direct_debit_account_holder'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Holder Name'),
    '#required' => TRUE,
    '#weight' => 0,
  );

  $direct_debit_form['direct_debit_bank_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank Name'),
    '#required' => TRUE,
    '#weight' => 1,
  );

  $direct_debit_form['direct_debit_sort_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Sort Code'),
    '#required' => TRUE,
    '#weight' => 2,
  );

  $direct_debit_form['direct_debit_bank_account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank Account Number'),
    '#required' => TRUE,
    '#weight' => 2,
  );

  $direct_debit_form['direct_debit_bank_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank Street Address'),
    '#required' => TRUE,
    '#weight' => 3,
  );

  $direct_debit_form['direct_debit_bank_city'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank City'),
    '#required' => TRUE,
    '#weight' => 4,
  );

  $direct_debit_form['direct_debit_bank_postal_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Bank Postal Code'),
    '#required' => TRUE,
    '#weight' => 5,
  );

  // Allow other modules to alter the form via hook_direct_debit_payment_form_alter()
  drupal_alter('direct_debit_payment_form', $direct_debit_form, $form, $form_state);
  return $direct_debit_form;
}

/**
 * Payment details callback for direct debit on the test gateway.
 */
function gm_test_gateway_direct_debit_payment_details(&$order, $submitted) {
  // Extract the payment details from the submission array.
  $payment_details =  array(
    'dd_account_number' => $submitted['direct_debit_bank_account_number'],
    'dd_sort_code' => $submitted['direct_debit_sort_code'],
    'dd_account_holder' => $submitted['direct_debit_account_holder'],
    'dd_bank_name' => $submitted['direct_debit_bank_name'],
    'dd_bank_street_address' => $submitted['direct_debit_bank_address'],
    'dd_bank_city' => $submitted['direct_debit_bank_city'],
    'dd_bank_postcode' => $submitted['direct_debit_bank_postal_code'],
  );

  // Allow other modules to alter the payment details via hook_payment_details_alter()
  drupal_alter('payment_details', $payment_details, 'direct_debit', $order);
  return $payment_details;
}
