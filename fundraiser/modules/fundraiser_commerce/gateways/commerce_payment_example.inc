<?php

/**
 * @file
 * Commerce based hook for commerce_payment_example.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_payment_example_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => array('credit'),
    'allow_refund' => array('credit'),
    'form callback' => 'commerce_payment_example_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_payment_example_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_payment_example_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_payment_example_fundraiser_commerce_expire',
    'charge callback' => 'commerce_payment_example_fundraiser_commerce_charge',
    'refund callback' => 'commerce_payment_example_fundraiser_commerce_refund',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_payment_example_fundraiser_commerce_form($payment_method) {
  // Move expiration date values up in the array. ONLY relevant for the default credit card fields.
  $form['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit card number'),
    '#size' => 20,
    '#required' => TRUE,
    '#salesforce_label' => t('Credit card last 4'),
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
    '#required' => TRUE,
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
    '#required' => TRUE,
  );
  $form['card_cvv'] = array(
    '#type' => 'textfield',
    '#title' => t('CVV'),
    '#size' => 6,
    '#required' => TRUE,
    '#salesforce_mappable' => FALSE,
  );
  return $form;
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_payment_example_fundraiser_commerce_cleanup($submission_fields) {
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
 * Callback function, scrub the fields before they get saved.
 */
function commerce_payment_example_fundraiser_commerce_scrub($payment_method, $fields) {
  // Scrub sensitive donation fields if they exists.
  if (isset($fields['credit']['card_number'])) {
    $fields['credit']['card_number'] = substr(
      $fields['credit']['card_number'], -4);
  }
  if (isset($fields['credit']['card_cvv'])) {
    $fields['credit']['card_cvv'] = '';
  }
  return $fields;
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_payment_example_fundraiser_commerce_expire($submission_fields) {
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
 * Callback function, charge the gateway.
 */
function commerce_payment_example_fundraiser_commerce_charge($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
  // Use 0000000000000000 to test a failed payment, anything else for a good one.
  if ($donation->donation['payment_fields']['credit']['card_number'] == '0000000000000000') {
    $success = FALSE;
    // TODO Place a false transaction on the transaction set.
  }
  else {
    $success = TRUE;
    // commerce_payment_example has helpfully split submission efforts outside of form submission,
    // so we can directly call it as follows:
    commerce_payment_example_transaction($payment_method, $order, $charge, $name);
  }
  return $success;
}


/**
 * Callback function, refund the gateway.
 */
function commerce_payment_example_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  // Create a new transaction to record the credit.
  $credit_transaction = commerce_payment_transaction_new('commerce_payment_example', $order->order_id);
  $credit_transaction->instance_id = $payment_method['instance_id'];
  $credit_transaction->remote_id = 'remote_id';
  $credit_amount = commerce_currency_decimal_to_amount($refund->amount, $transaction->currency_code);
  $credit_transaction->amount = $credit_amount * -1;
  $credit_transaction->currency_code = $transaction->currency_code;
  $credit_transaction->payload[REQUEST_TIME] = 'success';
  $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
  $credit_transaction->remote_status = 'remote_success';
  $credit_transaction->message = t('Credited back.');
  // Save the credit transaction.
  commerce_payment_transaction_save($credit_transaction);
  return TRUE;
}
