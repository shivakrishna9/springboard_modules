<?php

/**
 * @file
 * Commerce based hook for commerce_paypal.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_paypal_wpp_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('paypal'),
    'charge callback' => 'commerce_paypal_fundraiser_commerce_execute_gateway',
    'form callback' => 'commerce_paypal_wpp_fundraiser_commerce_paypal_form',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paypal_wpp_fundraiser_commerce_paypal_form($payment_method) {
  $form['paypal_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Paypal email'),
  );
  return $form;
}

/**
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_paypal_fundraiser_commerce_execute_gateway($payment_method, $donation) {
// Paypal has two payment methods and one of them is a outsource and redirect. This is more complicated, and needs work.
}