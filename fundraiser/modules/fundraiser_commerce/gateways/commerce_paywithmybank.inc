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
    'redirect callback' => 'commerce_paywithmybank_fundraiser_commerce_redirect',
    'refund callback' => '',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paywithmybank_fundraiser_commerce_form($payment_method, $config) {
  $form = array();

  $method_instance = commerce_payment_method_instance_load('commerce_paywithmybank_bank|commerce_payment_commerce_paywithmybank_bank');
  $access_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_id'] : NULL;

  $api = commerce_paywithmybank_get_api($method_instance);
  $api_endpoint = $api->getEndpoint();

  $form['widget'] = array(
    '#type' => 'container',
    '#id' => 'paywithmybank-widget',
    '#attributes' => array(
      'class' => array(
        'paywithmybank-compact',
      ),
    ),
  );

  $form['widget']['image'] = array(
    '#type' => 'markup',
    '#markup' => '<img src="https://paywithmybank.com/start/images/paywithmybanklogo.jpg" />',
  );

  $form['#attached'] = array(
    'js' => array(
      array(
        'data' => $api_endpoint . '/start/scripts/paywithmybank.js?accessId=' . $access_id,
        'type' => 'external',
      ),
      array(
        'data' => 'try { PayWithMyBank.widget( "paywithmybank-widget" ,{\'showBenefits\' : true, \'showInstructions\' : true, \'actionButtonLabel\' : \'Continue\' , \'eventType\' : \'click\'}); } catch(e){}',
        'type' => 'inline',
      ),
    ),
    'css' => array(
      array(
        'data' => $api_endpoint . '/start/styles/paywithmybank.css?accessId=' . $access_id,
        'type' => 'external',
      ),
    ),
  );

  return $form;
}

/**
 * Callback function, handle the redirect form.
 */
function commerce_paywithmybank_fundraiser_commerce_redirect($method_instance, $donation, $order, $settings) {
  $form = array();
  return $form;
}
