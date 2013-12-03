<?php

/**
 * @file
 * Commerce based hook for commerce_sage_payments
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_sage_payments_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    // Create the payment_method array with the commerce gateway method info name as the key
    'payment_method'    => array('credit'),
    'allow_recurring'   => array('credit'),
    'allow_refund'      => array('credit'),
    'form callback'     => 'commerce_sage_payments_fundraiser_commerce_submit_form',
    'cleanup callback'  => 'commerce_sage_payments_fundraiser_commerce_cleanup',
    'scrub callback'    => 'commerce_sage_payments_fundraiser_commerce_scrub',
    'validate callback' => 'commerce_sage_payments_fundraiser_commerce_validate',
    'expire callback'   => 'commerce_sage_payments_fundraiser_commerce_expire',
    'charge callback'   => 'commerce_sage_payments_fundraiser_commerce_charge',
    'refund callback'   => 'commerce_sage_payments_fundraiser_commerce_refund',
  );
}

/**
 * Returns the form fields for this method
 */
function commerce_sage_payments_fundraiser_commerce_submit_form($payment_method) {
  if ($payment_method == 'credit') {
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    return _fundraiser_commerce_credit_card_form();
  }
}

/**
 * Convert values to ones that Fundraiser Commerce is looking for
 */
function commerce_sage_payments_fundraiser_commerce_cleanup($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_cleanup($submission_fields);
}

/**
 * Callback function, return the user entered expiration dates for this submission.
 */
function commerce_sage_payments_fundraiser_commerce_expire($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_sage_payments_fundraiser_commerce_scrub($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_scrub($submission_fields);
}

/**
 * Validate the submitted values with the commerce validate function
 */
function commerce_sage_payments_fundraiser_commerce_validate($submission_fields, $payment_fields) {
  // Pass the values to the validation scripts
  $form_parents = array_merge($payment_fields['#parents'], array('credit'));
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  _fundraiser_commerce_credit_card_validate($submission_fields, $form_parents);
  return $submission_fields;
}

/**
 * Submit the donation values to the Sage Payments charge handler
 */
function commerce_sage_payments_fundraiser_commerce_charge($method_instance, $donation) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  // Execute call to Sage Payments depending on what payment method we have
  if ($donation->donation['payment_method'] == 'credit') {
    $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

    $method_instance['settings']['cardonfile'] = FALSE;

    if (module_exists('commerce_cardonfile')) {
      $method_instance['settings']['cardonfile'] = TRUE; // Card on file,
      $pane_values['credit_card']['cardonfile_store'] = TRUE; // And store it please.
      $pane_values['cardonfile'] = commerce_sage_payments_get_cardonfile_pane_value($donation->uid, $method_instance['instance_id']);
    }

    $result = commerce_sage_payments_cc_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  }

  // Returns FALSE if the payment failed, returns nothing if successful
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Returns the card_id of an unexpired, active card on file,
 * or 'new' if there isn't one.
 *
 * @param int $uid  User ID
 * @param int $instance_id  Payment method instance ID
 *
 * @return string  The string 'new' or a card_id from cardonfile.
 */
function commerce_sage_payments_get_cardonfile_pane_value($uid, $instance_id) {
  // Make a new token unless we already have one on record.
  $return = 'new';

  $stored_cards = commerce_cardonfile_load_multiple_by_uid($uid, $instance_id);

  if ($stored_cards == FALSE) {
    return $return;
  }

  // Filter out expired cards.
  foreach ($stored_cards as $card_id => $card_data) {
    // special case for echecks
    if ($card_data->card_exp_year == 0 && $card_data->card_exp_month == 0) {
      continue;
    }

    if ($card_data->card_exp_year < date('Y') ||
      $card_data->card_exp_year == date('Y') && $card_data->card_exp_month < date('m')) {
      unset($stored_cards[$card_id]);
    }
  }

  if (!empty($stored_cards)) {
    // Then grab the first one.
    foreach ($stored_cards as $card_id => $card_data) {
      $return = $card_id;
    }
  }

  return $return;
}

/**
 * Refund callback for Sage Payments, wraps the admin form used for refund with our own system.
 */
function commerce_sage_payments_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $refunding_order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $refunding_order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.

  // Set up the values needed for the transaction to go through.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  // Submit the donation form.
  module_load_include('inc', 'commerce_sage_payments', 'commerce_sage_payments.admin');
  return commerce_sage_payments_cc_credit_request($payment_method, $transaction, $refunding_order, $refund->amount);
}
