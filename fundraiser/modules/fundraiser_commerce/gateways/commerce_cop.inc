<?php

/**
 * @file
 * Code for the Bill Me Later feature.
 */

function commerce_cop_fundraiser_commerce_fundraiser_gateway_info() {
  $methods = new stdClass();
  $methods->payment_methods = array();
  rules_invoke_all('commerce_payment_methods', $methods);
  $payment_methods = array();
  foreach ($methods->payment_methods as $id => $info) {
    $this_gateway = array();
    $method_instance = commerce_payment_method_instance_load($id);
    if($method_instance['module'] == 'commerce_cop') {
    list($method_id, $rule_name) = explode('|', $method_instance['instance_id']);
      $payment_methods[$rule_name] = $method_id;
    }
  }
  $gateway_options = array(
    'payment_method' => $payment_methods,
    'form callback' => 'commerce_cop_fundraiser_commerce_submit_form',
    'charge callback' => 'commerce_cop_fundraiser_commerce_charge',
  );
  return $gateway_options;
}


function commerce_cop_fundraiser_commerce_submit_form($payment_method) {
   $form = array();
   $form_state = array();
   $form['#parents'] = array();
  $entity = commerce_payment_transaction_new($payment_method);
  // field_attach_form('commerce_payment_transaction', $entity, $form, $form_state);

  $fields = field_info_instances('commerce_payment_transaction', $payment_method);
  foreach ($fields as $instance) {
    $field = field_info_field($instance['field_name']);
    $field_form = field_default_form('commerce_payment_transaction', $entity, $field, $instance, LANGUAGE_NONE, array(), $form, $form_state);
    $form += (array) $field_form;    
  }
  return $form;
}

/**
 * Callback function, charge the gateway.
 */
function commerce_cop_fundraiser_commerce_charge($payment_method, $donation) {
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  if (TRUE == FALSE) {
    $success = FALSE;
  }
  else {
    $success = TRUE;
    commerce_cop_fundraiser_transaction($payment_method, $order, $charge);
  }
  return $success;
}

function commerce_cop_fundraiser_transaction($payment_method, $order, $charge) {
  dpm($payment_method);
  $transaction = commerce_payment_transaction_new($payment_method['method_id'], $order->order_id);
  $transaction->instance_id = $payment_method['instance_id'];
  $transaction->amount = $charge['amount'];
  $transaction->currency_code = $charge['currency_code'];
  $transaction->status = $payment_method['method_id'];
  commerce_payment_transaction_save($transaction);
}