<?php

/**
 * @file
 * Commerce based hook for commerce_ogone.
 */

/**
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_ogone_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  // OGone is a REDIRECTION system. THIS MAKES THIS RATHER TRICKY. NOT READY YET.
/*
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  // Call to commerce_authnet_aim_submit_form_submit() based on instance callbacks.
  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.
  $pane_values = array(
    'credit_card' => array(
      'number' => $donation->donation['card_number'],
      'exp_month' => $donation->donation['card_expiration_month'],
      'exp_year' => $donation->donation['card_expiration_year'],
      'code' => $donation->donation['card_cvv'],
    ),
  );

  // Customer data for authnet pulls from the customer profile given on the order, so we can ignore that.
  // If the charge fails, returns false, else nothing.
  $result = commerce_ogone_process_transaction($order, $payment_method, $feedback, $redirect = TRUE);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  // Transactions are made even for failures.
  // Grab the transaction information so we can return the transaction id.
  // Use the last one in the list cause it's the last one we processed.
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  foreach ($transactions as $transaction) {
    $txn_id = $transaction->remote_id;
    $message = $transaction->message;
  }
*/
  // And update the result array success field.
  $result = array(
    'success' => $success,
    'comment' => t('Card charged, resolution code: ' . $message),
    'message' => $success ? t('Credit card payment processed successfully.') : t('Credit card charge failed.'),
    'uid' => $order->uid,
    'data' => array('txn_id' => $txn_id),
    'error type' => ($success) ? '' : 'billing',
  );
  return $result;
}