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
    // ALL currently supported ubercart gateways use the same field set.
    'form callback' => 'commerce_authnet_fundraiser_commerce_credit_card_form',
    'form validate callback' => 'commerce_authnet_fundraiser_commerce_credit_card_submit_form',
    'form submit callback' => 'commerce_authnet_fundraiser_commerce_credit_card_submit_form',
    'form expiration callback' => 'commerce_authnet_fundraiser_commerce_credit_card_expiration_form',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_authnet_fundraiser_commerce_credit_card_form($payment_method) {
  // Move expiration date values up in the array. ONLY relevant for the default credit card fields.
  $form['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit card number'),
    '#size' => 20,
  );
  $form['expiration_date']['#theme'] = 'fundraiser_credit_card_expiration_date';
  $months[1] = 'January';
  $months[2] = 'February';
  $months[3] = 'March';
  $months[4] = 'April';
  $months[5] = 'May';
  $months[6] = 'June';
  $months[7] = 'July';
  $months[8] = 'August';
  $months[9] = 'September';
  $months[10] = 'October';
  $months[11] = 'November';
  $months[12] = 'December';
  $form['expiration_date']['card_expiration_month'] = array(
    '#type' => 'select',
    '#options' => $months,
    '#title' => t('Expiration month'),
    '#default_value' => date('n'),
  );
  $this_year = date('Y');
  $years[$this_year] = $this_year;
  for ($i = 1; $i <= 15; $i++) {
    $years[$this_year + $i] = $this_year + $i;
  }
  $form['expiration_date']['card_expiration_year'] = array(
    '#type' => 'select',
    '#options' => $years,
    '#title' => t('Expiration year'),
    '#default_value' => date('Y'),
  );
  $form['card_cvv'] = array(
    '#type' => 'textfield',
    '#title' => t('CVV'),
    '#size' => 6,
  );
  return $form;
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_authnet_fundraiser_commerce_credit_card_submit_form($submission_fields) {
  if (isset($submission_fields['payment_fields']['credit']['expiration_date'])) {
    if (isset($submission_fields['payment_fields']['credit']['expiration_date']['card_expiration_month'])) {
      $submission_fields['payment_fields']['credit']['card_expiration_month'] =
        $submission_fields['payment_fields']['credit']['expiration_date']['card_expiration_month'];
    }
    if (isset($submission_fields['payment_fields']['credit']['expiration_date']['card_expiration_year'])) {
      $submission_fields['payment_fields']['credit']['card_expiration_year'] =
        $submission_fields['payment_fields']['credit']['expiration_date']['card_expiration_year'];
    }
    unset($submission_fields['payment_fields']['credit']['expiration_date']);
  }
  return $submission_fields;
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_authnet_fundraiser_commerce_credit_card_expiration_form($submission_fields) {
  $expires = array();
  if (isset($submission_fields['payment_fields']['credit']['card_expiration_month'])) {
    $expires['month'] = $submission_fields['payment_fields']['credit']['card_expiration_month'];
  }
  if (isset($submission_fields['payment_fields']['credit']['card_expiration_year'])) {
    $expires['year'] = $submission_fields['payment_fields']['credit']['card_expiration_year'];
  }
  return $expires;
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