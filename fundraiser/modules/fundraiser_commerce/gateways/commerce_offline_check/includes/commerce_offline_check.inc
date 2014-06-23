<?php

/**
 * @file
 * commerce hooks
 */

function commerce_offline_check_submit_form($payment_method, $pane_values) {
  $form = array();
  // Merge in values from the order.
  if (!empty($order->data['offline_check'])) {
    $pane_values += $order->data['offline_check'];
  }

  // Merge in default values.
  $pane_values += array(
    'check_number' => '',
    'message' => ''
  );

  $form['check_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Check Number'),
    '#description' => t('Check number'),
    '#default_value' => $pane_values['check_number'],
    '#required' => FALSE,
    '#maxlength'=> 14,

  );

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#description' => t('Comments'),
    '#default_value' => $pane_values['message'],
    '#required' => FALSE,
  );
  $form['#attached']['js'][] = drupal_get_path('module', 'commerce_offline_check') . '/js/commerce_offline_check.js';
  return $form;
}


/**
 * Callback function, charge the gateway.
 */
function commerce_offline_check_fundraiser_commerce_charge($payment_method, $donation) {
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  if (TRUE == FALSE) {
    $success = FALSE;
  }
  else {
    $success = TRUE;
    commerce_offline_check_transaction($payment_method, $order, $charge, $donation);
  }
  return $success;
}

function commerce_offline_check_transaction($payment_method, $order, $charge, $donation) {
  $transaction = commerce_payment_transaction_new('offline_check', $order->order_id);
  $transaction->instance_id = $payment_method['instance_id'];
  $transaction->amount = $charge['amount'];
  $transaction->currency_code = $charge['currency_code'];
  $transaction->status = 'offline_check';
  commerce_payment_transaction_save($transaction);
  // $data = array(
  //   'tid' => $transaction->transaction_id,
  //   'did' => $donation->did,
  //   'oid' => $order->order_id,
  //   'message' => $donation->donation['payment_fields']['offline_check']['message'],
  //   'check_number' => $donation->donation['payment_fields']['offline_check']['check_number'],

  // );
  // drupal_write_record('commerce_offline_check', $data);
}