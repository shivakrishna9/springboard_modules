<?php

/**
 * @file
 * Commerce based hook for commerce_payment_example.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_payment_example_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => array('credit'),
    'offsite_recurring' => array('credit'),
    'allow_refund' => array('credit'),
    'form callback' => 'commerce_payment_example_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_payment_example_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_payment_example_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_payment_example_fundraiser_commerce_expire',
    'charge callback' => 'commerce_payment_example_fundraiser_commerce_charge',
    'refund callback' => 'commerce_payment_example_fundraiser_commerce_refund',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_payment_example_fundraiser_commerce_form($payment_method) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_form();
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_payment_example_fundraiser_commerce_cleanup($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_cleanup($submission_fields);
}

/**
 * Callback function, scrub the fields before they get saved.
 */
function commerce_payment_example_fundraiser_commerce_scrub($submission_fields, $payment_method) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_scrub($submission_fields);
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_payment_example_fundraiser_commerce_expire($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Callback function, charge the gateway.
 */
function commerce_payment_example_fundraiser_commerce_charge($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
  // Use 0000000000000000 to test a failed payment, anything else for a good one.
  if ($donation->donation['payment_fields']['credit']['card_number'] == '0000000000000000') {
    $success = FALSE;
    // TODO Place a false transaction on the transaction set.
  }
  else {
    $success = TRUE;
    // commerce_payment_example has helpfully split submission efforts outside of form submission,
    // so we can directly call it as follows:
    commerce_payment_example_transaction($payment_method, $order, $charge, $name);
  }
  return $success;
}


/**
 * Callback function, refund the gateway.
 */
function commerce_payment_example_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  // Create a new transaction to record the credit.
  $credit_transaction = commerce_payment_transaction_new('commerce_payment_example', $order->order_id);
  $credit_transaction->instance_id = $payment_method['instance_id'];
  $credit_transaction->remote_id = 'remote_id';
  $credit_amount = commerce_currency_decimal_to_amount($refund->amount, $transaction->currency_code);
  $credit_transaction->amount = $credit_amount * -1;
  $credit_transaction->currency_code = $transaction->currency_code;
  $credit_transaction->payload[REQUEST_TIME] = 'success';
  $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
  $credit_transaction->remote_status = 'remote_success';
  $credit_transaction->message = t('Credited back.');
  // Save the credit transaction.
  commerce_payment_transaction_save($credit_transaction);
  return TRUE;
}
