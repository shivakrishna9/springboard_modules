<?php

/**
 * @file
 * Commerce based hook for commerce_paypal.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_paypal_wps_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('paypal'),
    'offsite_processing' => array('paypal'),
    'form callback' => 'commerce_paypal_wps_fundraiser_commerce_paypal_form',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paypal_wps_fundraiser_commerce_paypal_form($payment_method) {
  $form['paypal_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Paypal email'),
  );
  return $form;
}

/**
 * Implements hook_fundraiser_commerce_execute_redirect().
 */
function commerce_paypal_wps_fundraiser_commerce_execute_redirect($method_instance, $donation, $order, $settings) {
  return drupal_get_form('commerce_paypal_wps_order_form', $order, $method_instance['settings'] + $settings);
}

/**
 * Implements hook_fundraiser_commerce_execute_redirect_validate().
 */
function commerce_paypal_wps_fundraiser_commerce_execute_redirect_validate($method_instance, $donation, $order) {
  $result = commerce_paypal_wps_redirect_form_validate($order, $method_instance);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Implements hook_fundraiser_commerce_execute_redirect_submit().
 */
function commerce_paypal_wps_fundraiser_commerce_execute_redirect_submit($method_instance, $donation, $order) {
  // IMPORTANT: Transaction handling for Paypal is handled on a differnt URL, when Paypal returns the IPN tracsaction ID. 
  // Therefore no additional transaction work occurs here.
  return TRUE;
}
