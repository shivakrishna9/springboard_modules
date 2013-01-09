<?php

/**
 * @file
 * Commerce based hook for commerce_payment_example.
 */

/**
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_payment_example_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  $result = array(
    'success' => FALSE,
    'message' => 'This payment failed during gateway execution.'
  );
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
  // Use 0000000000000000 to test a failed payment, anything else for a good one.
  if ($donation->donation['card_number'] == '0000000000000000') {
    $success = FALSE;
  }
  else {
    $success = TRUE;
    // commerce_payment_example has helpfully split submission efforts outside of form submission,
    // so we can directly call it as follows:
    commerce_payment_example_transaction($payment_method, $order, $charge, $name);
  }
  // If a user is created in this process, make sure to map the order ownership.
  // If a transaction, make sure to map those values into result array.
  // And update the result array success field.
  $result = array(
    'success' => $success,
    'comment' => t('Card charged, resolution code: 0022548315'),
    'message' => $success ? t('Credit card payment processed successfully.') : t('Credit card charge failed.'),
    'uid' => $order->uid,
    'data' => array('txn_id' => 'TEST-TXNID-001'),
    'error type' => ($success) ? '' : 'billing',
  );
  return $result;
}