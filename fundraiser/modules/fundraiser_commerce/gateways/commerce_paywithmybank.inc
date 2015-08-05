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
  $merchant_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_merchant_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_merchant_id'] : NULL;
  $access_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_id'] : NULL;
  $access_key = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_key'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_key'] : NULL;

  $api = commerce_paywithmybank_get_api($method_instance);
  $api_endpoint = $api->getEndpoint();

  $order_total = $order->commerce_order_total[LANGUAGE_NONE][0];
  $order_amount = substr(commerce_currency_format($order_total['amount'], $order_total['currency_code']), 1);

  $payment_parameters = array(
    'accessId' => $access_id,
    'merchantId' => $merchant_id,
    'description' => 'Order from' . variable_get('site_name'),
    'currency' => $order_total['currency_code'],
    'amount' => $order_amount,
    'merchantReference' => $order->order_id,
  );

  $request_signature = commerce_paywithmybank_get_request_signature($access_key, $payment_parameters);

  $establish_data = $payment_parameters;
  $establish_data['returnUrl'] = $settings['return'];
  $establish_data['cancelUrl'] = $settings['cancel_return'];
  $establish_data['requestSignature'] = $request_signature;

  $form = array();

  $form['#attached'] = array(
    'js' => array(
      array(
        'data' => $api_endpoint . '/start/scripts/paywithmybank.js?accessId=' . $access_id,
        'type' => 'external',
      ),
      array(
        'data' => 'jQuery(document).ready(function(){ PayWithMyBank.establish(' . json_encode($establish_data) . '); });',
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
 * Callback function, submit after redirect.
 */
function commerce_paywithmybank_fundraiser_commerce_redirect_submit($method_instance, $donation, $order) {
  $transaction_id = filter_input(INPUT_GET, 'transactionId');
  $request_signature = filter_input(INPUT_GET, 'requestSignature');

  $success = commerce_paywithmybank_validate_request($transaction_id, $request_signature);

  $pane_values = NULL;

  _fundraiser_commerce_charge_submit_form_process($success, $method_instance, $pane_values, $donation);

  return $success;
}
