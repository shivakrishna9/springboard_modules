<?php

/**
 * @file
 * Forms for use with the Commerce PayWithMyBank payment method.
 */

/**
 * Commerce PayWithMyBank checkout form.
 */
function commerce_paywithmybank_checkout_form($payment_method, $order, $show_widget = FALSE) {
  global $base_url;

  $settings = $payment_method['settings'];

  $merchant_id = (isset($settings['commerce_paywithmybank_settings_merchant_id'])) ? $settings['commerce_paywithmybank_settings_merchant_id'] : NULL;
  $access_id = (isset($settings['commerce_paywithmybank_settings_access_id'])) ? $settings['commerce_paywithmybank_settings_access_id'] : NULL;
  $access_key = (isset($settings['commerce_paywithmybank_settings_access_key'])) ? $settings['commerce_paywithmybank_settings_access_key'] : NULL;

  $api = commerce_paywithmybank_get_api($payment_method);
  $api_endpoint = $api->getEndpoint();

  $order_total = $order->commerce_order_total[LANGUAGE_NONE][0];
  $order_amount = substr(commerce_currency_format($order_total['amount'], $order_total['currency_code']), 1);

  $payment_parameters = array(
    'accessId' => $access_id,
    'merchantId' => $merchant_id,
    'description' => "Amount: $" . $order_amount,
    'currency' => $order_total['currency_code'],
    'amount' => $order_amount,
    'merchantReference' => $order->order_id,
  );

  $request_signature = commerce_paywithmybank_get_request_signature($access_key, $payment_parameters);

  $establish_data = $payment_parameters;
  $establish_data['returnUrl'] = $base_url . '/paywithmybank/return';
  $establish_data['cancelUrl'] = $base_url . '/checkout/' . $order->order_id . '/review';
  $establish_data['requestSignature'] = $request_signature;

  $form = array();

  if ($show_widget) {
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
  }

  $form['#attached'] = array(
    'js' => array(
      array(
        'data' => $api_endpoint . '/start/scripts/paywithmybank.js?accessId=' . $access_id,
        'type' => 'external',
      ),
      array(
        'data' => drupal_get_path('module', 'commerce_paywithmybank') . '/js/checkout.js',
        'type' => 'file',
      ),
      array(
        'data' => array(
          'paywithmybank_establish_data' => $establish_data,
        ),
        'type' => 'setting',
      )
    ),
    'css' => array(
      array(
        'data' => $api_endpoint . '/start/styles/paywithmybank.css?accessId=' . $access_id,
        'type' => 'external',
      ),
    ),
  );

  if ($show_widget) {
    $form['#attached']['js'][] = array(
      'data' => 'try { PayWithMyBank.widget( "paywithmybank-widget" ,{\'showBenefits\' : true, \'showInstructions\' : true, \'actionButtonLabel\' : \'Continue\' , \'eventType\' : \'click\'}); } catch(e){}',
      'type' => 'inline',
    );
  }

  return $form;
}
