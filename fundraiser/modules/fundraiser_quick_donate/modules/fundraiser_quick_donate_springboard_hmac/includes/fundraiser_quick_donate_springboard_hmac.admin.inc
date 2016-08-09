<?php
/**
 * @file
 * Admin-related functions for Fundraisere Quick Donate Springboard HMAC.
 */

/**
 * Upgrade link generator form.
 */
function fundraiser_quick_donate_springboard_hmac_token_generator($form, $form_state) {
  $key = springboard_hmac_get_key();
  if (empty($key)) {
    drupal_set_message(t('Secure Prepopulate key and initialization vector are not set. Upgrade links will not work.'), 'error');
  }

  $defaults = array();
  $default_defaults = array(
    'uid' => 1,
    'card_id' => 1,
    'gateway_id' => 'authnet_aim|commerce_payment_authnet_aim',
  );
  foreach ($default_defaults as $key => $default) {
    if (isset($_SESSION['qd_token_generate'][$key])) {
      $defaults[$key] = $_SESSION['qd_token_generate'][$key];
    }
    else {
      $defaults[$key] = $default;
    }
  }

  $form['fundraiser_quick_donate_springboard_hmac'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick Donate Token'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('To test quick donation tokens, enter the following field information and press "Generate Token".'),
  );

  $form['fundraiser_quick_donate_springboard_hmac']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#size' => 20,
    '#default_value' => $defaults['uid'],
    '#description' => t('The UID of the donator.'),
  );

  $form['fundraiser_quick_donate_springboard_hmac']['card_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Card ID'),
    '#description' => t('The card ID.'),
    '#default_value' => $defaults['card_id'],
  );

  $gateways = _fundraiser_gateway_info();
  $gateways = array_keys($gateways);
  $allowable = array();
  foreach ($gateways as $gateway) {
    $payment_method = commerce_payment_method_instance_load($gateway);
    if (_commerce_cardonfile_capable_payment_method_check($payment_method)) {
      $allowable[$gateway] = $gateway;
    }
  }
  $gateways = array_intersect($gateways, $allowable);
  foreach ($gateways as $key => $gateway) {
    $gateways[$gateway] = $gateway;
    unset($gateways[$key]);
  }

  $form['fundraiser_quick_donate_springboard_hmac']['gateway_id'] = array(
    '#type' => 'select',
    '#title' => t('Gateway ID'),
    '#description' => t('The gateway ID.'),
    '#options' => $gateways,
    '#default_value' => $defaults['gateway_id'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Token'),
  );

  $form['#submit'][] = 'fundraiser_quick_donate_springboard_hmac_token_generator_validate';
  $form['#submit'][] = 'fundraiser_quick_donate_springboard_hmac_token_generator_submit';

  return $form;
}

/**
 * Store temporary upgrade link values in session.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function fundraiser_quick_donate_springboard_hmac_token_generator_validate($form, $form_state) {
  $_SESSION['qd_token_generate'] = $form_state['values'];
}

/**
 * Create an upgrade link.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function fundraiser_quick_donate_springboard_hmac_token_generator_submit($form, $form_state) {
  $payload = array(
    'uid' => $form_state['values']['uid'],
    'card_id' => $form_state['values']['card_id'],
    'gateway_id' => $form_state['values']['gateway_id'],
  );

  $link = fundraiser_quick_donate_springboard_hmac_create_link($payload);

  drupal_set_message('Quick donate link generated! ' . l($link, $link));
}
