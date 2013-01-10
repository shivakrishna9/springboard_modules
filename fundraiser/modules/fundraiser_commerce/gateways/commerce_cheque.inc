<?php

/**
 * @file
 * Commerce based hook for commerce_cheque.
 */

/**
 * Implements commerce_cheque().
 */
function commerce_cheque_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $result = commerce_cheque_transaction($payment_method, $order, $charge);

  // Grab the transaction information so we can return the transaction id.
  // Use the last one in the list cause it's the last one we processed.
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  foreach ($transactions as $transaction) {
    $txn_id = 'commerce_cheque';
    $message = $transaction->message;
  }

  // Checks are ALWAYS initially listed as a success.
  // If the check bounces or does not arrive it's up to the donation agency to handle that situation,
  // through the order pane and Salesforce systems.
  // And update the result array success field.
  $result = array(
    'success' => TRUE,
    'comment' => t('Cheque pendings: ' . $message),
    'message' => t('This order is waiting for the cheque to be cashed.'),
    'uid' => $order->uid,
    'data' => array('txn_id' => $txn_id),
    'error type' => '',
  );
  return $result;
}