<?php

/**
 * @file
 * Commerce based hook for commerce_cybersource.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_cybersource_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => array('credit'),
    'allow_refund' => array('credit'),
    'form callback' => 'commerce_cybersource_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_cybersource_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_cybersource_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_cybersource_fundraiser_commerce_expire',
    'charge callback' => 'commerce_cybersource_fundraiser_commerce_charge',
    'update callback' => 'commerce_cybersource_fundraiser_commerce_update',
    'refund callback' => 'commerce_cybersource_fundraiser_commerce_refund',
    'cardonfile callback' => 'commerce_cybersource_fundraiser_commerce_cardonfile',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_cybersource_fundraiser_commerce_form($payment_method) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_form();
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_cybersource_fundraiser_commerce_cleanup($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_cleanup($submission_fields);
}

/**
 * Callback function, scrub data.
 */
function commerce_cybersource_fundraiser_commerce_scrub($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_scrub($submission_fields);
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_cybersource_fundraiser_commerce_expire($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Callback function, charge gateway.
 */
function commerce_cybersource_fundraiser_commerce_charge($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $pane_form = array(); // Spare, no values here used during submission. Ignore.
  $pane_values = array(
    'credit_card' => array(
      'number' => $donation->donation['payment_fields']['credit']['card_number'],
      'exp_month' => $donation->donation['payment_fields']['credit']['card_expiration_month'],
      'exp_year' => $donation->donation['payment_fields']['credit']['card_expiration_year'],
      'code' => $donation->donation['payment_fields']['credit']['card_cvv'],
    ),
  );

  // Customer data for charge pulls from the customer profile given on the order, so we can ignore that.
  // If the charge fails, returns false, else nothing.
  $result = commerce_cybersource_soap_cc_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}


/**
 * Card on file callback
 */
function commerce_cybersource_fundraiser_commerce_cardonfile($donation) {

}

/**
 * Callback function, update card data stored at the gateway and locally
 *
 * @param $donation Object
 *    Fundraiser commerce donation object which should include the new/updated card data
 * @param $card_data Object
 *    Card on file entity, this should be the current entity for the donation,
 *    new data should be passed in the donation object
 *
 * @return Object
 *    Card on file entity for the new/updated card data
 */
function commerce_cybersource_fundraiser_commerce_update($donation, $card_data = NULL) {
  
}

/**
 * Refund callback for cybersource, wraps the admin form used for refund with our own system.
 */
function commerce_cybersource_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $refunding_order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $refunding_order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.

  $form = array();
  $form_state = array();
  // Set up the values needed for the transaction to go through.
  $form_state['transaction'] = $transaction;
  $form_state['values']['amount'] = $refund->amount;
  $form_state['order'] = $refunding_order;
  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);
  // Submit the donation form.

   module_load_include('inc', 'commerce_cybersource', '/includes/commerce_cybersource.admin');
   $result = commerce_cybersource_soap_capture_form_submit($form, $form_state);

  // At this point the card has been saved on the user profile.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}
