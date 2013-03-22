<?php

/**
 * @file
 * Commerce based hook for commerce_psi
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_psi_credit_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => array('credit'),
    'form callback' => 'commerce_psi_credit_fundraiser_commerce_submit_form',
    'cleanup callback' => 'commerce_psi_credit_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_psi_credit_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_psi_credit_fundraiser_commerce_expire',
    'validate callback' => 'commerce_psi_credit_fundraiser_commerce_validate',
    'charge callback' => 'commerce_psi_credit_fundraiser_commerce_charge',
  );
}

/**
 * Returns the form fields for this method
 */
function commerce_psi_credit_fundraiser_commerce_submit_form($payment_method, $config) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_form();
}

/**
 * Convert values to ones that Fundraiser Commerce is looking for
 */
function commerce_psi_credit_fundraiser_commerce_cleanup($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_cleanup($submission_fields);
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_psi_credit_fundraiser_commerce_expire($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_psi_credit_fundraiser_commerce_scrub($payment_method, $fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_scrub($payment_method, $fields);
}

/**
 * Validate the submitted values with the commerce validate function
 */
function commerce_psi_credit_fundraiser_commerce_validate($submission_fields, $payment_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $form_parents = array_merge($payment_fields['#parents'], array('credit'));
  _fundraiser_commerce_credit_card_validate($submission_fields, $form_parents);
  return $submission_fields;
}

/**
 * Submit the donation values to the PSI credit charge handler
 */
function commerce_psi_credit_fundraiser_commerce_charge($method_instance, $donation) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);
  $pane_values['frequency'] = $donation->donation['recurs_monthly'] == TRUE ? 'Monthly' : NULL;

  // Execute call to PSI, only returns FALSE if the payment failed, returns nothing if successful
  $result = commerce_psi_credit_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}
