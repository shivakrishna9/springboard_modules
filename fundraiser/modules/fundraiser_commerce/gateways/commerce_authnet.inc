<?php

/**
 * @file
 * Commerce based hook for commerce_authnet.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_authnet_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => TRUE,
  );
}

/**
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_authnet_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  // Call to commerce_authnet_aim_submit_form_submit() based on instance callbacks.
  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.
  $pane_values = array(
    'credit_card' => array(
      'number' => $donation->donation['payment_fields']['credit']['card_number'],
      'exp_month' => $donation->donation['payment_fields']['credit']['card_expiration_month'],
      'exp_year' => $donation->donation['payment_fields']['credit']['card_expiration_year'],
      'code' => $donation->donation['payment_fields']['credit']['card_cvv'],
    ),
  );
  // Account for any CIM if stored on file, we have everything we need here - no need to pull out
  // of the donation object or habitually load this value from the donation. This may need to be
  // revised later, but for now I don't think we need to pull sustainer data with the donation.
  // Commerce needs to execute billing update to call into the gateway to update on asociated CIMs.
  // As a result, sustainer has NO NEED for cim to be loaded into the donation object. Those tokens Go Away.
  // If possible at least.
  if (module_exists('commerce_cardonfile') && !empty($payment_method['settings']['cim_cardonfile'])) {
    $pane_values['cardonfile'] = 'new'; // Make a new CIM unless we already have one on record.
    $stored_cards = commerce_cardonfile_data_load_multiple($donation->uid, $payment_method['instance_id']);
    // Filter out expired cards.
    foreach ($stored_cards as $card_id => $card_data) {
      if ($card_data['card_exp_year'] < date('Y') ||
        $card_data['card_exp_year'] == date('Y') && $card_data['card_exp_month'] < date('m')) {
        unset($stored_cards[$card_id]);
      }
    }
    if (!empty($stored_cards)) {
      // Then grab the first one.
      foreach ($stored_cards as $card_id => $card_data) {
        $pane_values['cardonfile'] =  $card_id;
      }
    }
  }
  // Execute call to commerce_authnet, be it CIM or other, it'll come back with the correct response.
  $result = commerce_authnet_aim_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  // Transactions are made even for failures.
  // Grab the transaction information so we can return the transaction id.
  // Use the last one in the list cause it's the last one we processed.
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  foreach ($transactions as $transaction) {
    $txn_id = $transaction->remote_id;
    $message = $transaction->message;
  }

  // And update the result array success field.
  $result = array(
    'success' => $success,
    'comment' => t('Card charged, resolution code: ' . $message),
    'message' => $success ? t('Credit card payment processed successfully.') : t('Credit card charge failed.'),
    'uid' => $order->uid,
    'data' => array('txn_id' => $txn_id),
    'error type' => ($success) ? '' : 'billing',
  );
  return $result;
}