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
    'form callback' => 'commerce_paypal_wps_fundraiser_commerce_form',
    'redirect callback' => 'commerce_paypal_wps_fundraiser_commerce_redirect',
    'scrub callback' => 'commerce_paypal_wps_fundraiser_commerce_scrub',
    // Validation callback is commerce_paypal_wps_fundraiser_commerce_redirect_validate
    // Submit callback is commerce_paypal_wps_fundraiser_commerce_redirect_submit
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paypal_wps_fundraiser_commerce_form($payment_method) {
  $form['paypal_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Paypal email'),
  );
  return $form;
}

/**
 * Callback function, handle the redirect form.
 */
function commerce_paypal_wps_fundraiser_commerce_redirect($method_instance, $donation, $order, $settings) {
  return drupal_get_form('commerce_paypal_wps_order_form', $order, $method_instance['settings'] + $settings);
}

/**
 * Callback function, validate after redirect.
 */
function commerce_paypal_wps_fundraiser_commerce_redirect_validate($method_instance, $donation, $order) {
  $result = commerce_paypal_wps_redirect_form_validate($order, $method_instance);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_paypal_wps_fundraiser_commerce_scrub($payment_method, $submission_fields) {
  // Nothing to store for paypal
  return array();
}

/**
 * Callback function, submit after redirect.
 */
function commerce_paypal_wps_fundraiser_commerce_redirect_submit($method_instance, $donation, $order) {
  // IMPORTANT: Transaction handling for Paypal is handled on a differnt URL, when Paypal returns the IPN tracsaction ID. 
  // Therefore no additional transaction work occurs here.
  return TRUE;
}
