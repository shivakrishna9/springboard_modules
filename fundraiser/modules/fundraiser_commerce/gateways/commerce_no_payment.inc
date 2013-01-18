<?php

/**
 * @file
 * Commerce based hook for commerce_cheque.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_no_payment_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('none'),
    'allow_recurring' => FALSE,
    // ALL currently supported ubercart gateways use the same field set.
    'form callback' => 'commerce_no_payment_fundraiser_commerce_paypal_form',
    //$this_gateway['form validate callback'] = '',
    //$this_gateway['form submit callback'] = '',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_no_payment_fundraiser_commerce_paypal_form($payment_method) {
  $form['check_message'] = array(
    '#markup' => '<div class="payment-method-message">' .
      t('Enjoy your free stuff.') .
      '</div>',
  );
  return $form;
}

/**
 * Implements commerce_no_payment().
 */
function commerce_no_payment_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
  $success = FALSE;
  if ($charge['amount'] <= 0) {
    $result = commerce_no_payment_transaction($payment_method, $order, $charge, $name);
    $success = TRUE;
  }

  // Grab the transaction information so we can return the transaction id.
  // Use the last one in the list cause it's the last one we processed.
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  foreach ($transactions as $transaction) {
    $txn_id = 'commerce_no_payment';
    $message = $transaction->message;
  }

  // And update the result array success field.
  $result = array(
    'success' => $success,
    'comment' => t('No charge: ' . $message),
    'message' => $success ? t('Enjoy your free stuff.') : t('There was an outstanding balance on this order.'),
    'uid' => $order->uid,
    'data' => array('txn_id' => $txn_id),
    'error type' => ($success) ? '' : 'billing',
  );
  return $result;
}