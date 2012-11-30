<?php

/**
 * @file
 * Payment gateway manager API reference
 */


/**
 * Implements hook_payment_gateway_managed().
 */
function hook_payment_gateway_managed() {
  $gateways[] = array(
    'id' => 'test_gateway',
    'title' => t('Test Gateway'),
    'description' => t('Process credit card payments through the Test Gateway.'),
    'file' => 'gm_test_gateway.gm.inc', // OPTIONAL
    'payment_methods' => array(
      'credit' => array(
        'payment_callback' => 'gm_test_gateway_credit_charge',
        'render_callback' => 'gm_test_gateway_credit_webform_render',
        'payment_details_callback' => 'gm_test_gateway_credit_payment_details',
        'recurring' => FALSE,
      ),
      'paypal' => array(
        'payment_callback' => 'gm_test_gateway_paypal_charge',
        'render_callback' => 'gm_test_gateway_paypal_webform_render',
        'payment_details_callback' => 'gm_test_gateway_paypal_payment_details',
        'recurring' => FALSE,
      ),
    ),
  );

  return $gateways;
}
