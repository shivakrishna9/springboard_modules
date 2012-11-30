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
}

/**
 * Payment details callback for credit cards on the test gateway.
 */
function gm_test_gateway_credit_payment_details(&$order, $submitted) {
}

/**
 * Payment callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_charge($order_id, $amount, $data) {
}

/**
 * Webform render callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_webform_render($form, &$form_state) {
}

/**
 * Payment details callback for PayPal on the test gateway.
 */
function gm_test_gateway_paypal_payment_details(&$order, $submitted) {
}
