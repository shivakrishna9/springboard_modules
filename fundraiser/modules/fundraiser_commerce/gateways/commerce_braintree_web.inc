<?php

/**
 * @file
 * Commerce based hook for commerce_authnet.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_braintree_web_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit', 'paypal'),
    'allow_recurring' => array('credit', 'paypal'),
    'allow_refund' => array('credit', 'paypal'),
    'form callback' => 'commerce_braintree_web_fundraiser_commerce_form',
    // 'cleanup callback' => 'commerce_authnet_fundraiser_commerce_cleanup',
    // 'scrub callback' => 'commerce_authnet_fundraiser_commerce_scrub',
    // 'expire callback' => 'commerce_authnet_fundraiser_commerce_expire',
    // 'validate callback' => 'commerce_authnet_fundraiser_commerce_validate',
    'charge callback' => 'commerce_braintree_web_fundraiser_commerce_charge',
    // 'update callback' => 'commerce_authnet_fundraiser_commerce_update',
    // 'refund callback' => 'commerce_authnet_fundraiser_commerce_refund',
    // 'cardonfile callback' => 'commerce_authnet_fundraiser_commerce_cardonfile',
    // 'response fields callback' => 'commerce_authnet_fundraiser_commerce_response_fields',
    // 'response save callback' => 'commerce_authnet_fundraiser_commerce_response_save',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_braintree_web_fundraiser_commerce_form($payment_method) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.braintree_credit_card');
  return _fundraiser_commerce_braintree_credit_card_form();
}


/**
 * Callback function, charge gateway.
 */
function commerce_braintree_web_fundraiser_commerce_charge($method_instance, $donation) {

  // Translate from donation settings to the correct values for the plugin.
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  // @TODO this function won't work b/c it is looking at CC field values that
  // do not exist with hosted fields.
  $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

  // This only does stuff when commerce_cardonfile is enabled.
  _fundraiser_commerce_submit_handler_pane_values($pane_values, $donation);

  $result = commerce_braintree_web_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  // Perform post processing functions.
  _fundraiser_commerce_charge_submit_form_process($success, $payment_method, $pane_values, $donation);
  return $success;

}
