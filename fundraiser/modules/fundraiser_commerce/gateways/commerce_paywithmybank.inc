<?php

/**
 * @file
 * Commerce based hook for commerce_paywithmybank.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info()
 */
function commerce_paywithmybank_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('paywithmybank'),
    'offsite_processing' => array('paywithmybank'),
    'form callback' => 'commerce_paywithmybank_fundraiser_commerce_form',
    'refund callback' => '',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paywithmybank_fundraiser_commerce_form($payment_method, $config) {
  module_load_include('inc', 'commerce_paywithmybank', 'includes/commerce_paywithmybank.forms');

  // TODO: Get order.
  $order = NULL;

  $form = commerce_paywithmybank_checkout_form($payment_method, $order);
  return $form;
}
