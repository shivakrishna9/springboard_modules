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
    // Provide the form for display.
    'form callback' => 'commerce_payment_example_fundraiser_commerce_credit_card_form',
    // Massage data as needed to be in the right format.
    'form validate callback' => 'commerce_payment_example_fundraiser_commerce_credit_card_submit_form',
    'form submit callback' => 'commerce_payment_example_fundraiser_commerce_credit_card_submit_form',
    // Retrieve expiration info from form submissions.
    'form expiration callback' => 'commerce_payment_example_fundraiser_commerce_credit_card_expiration_form',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_payment_example_fundraiser_commerce_credit_card_form($payment_method) {
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
function commerce_payment_example_fundraiser_commerce_credit_card_submit_form($submission_fields) {
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
function commerce_payment_example_fundraiser_commerce_credit_card_expiration_form($submission_fields) {
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
function commerce_payment_example_fundraiser_commerce_execute_gateway($payment_method, $donation) {
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
 * Implements hook_fundraiser_commerce_scrub_payment_fields().
 */
function commerce_payment_example_fundraiser_commerce_scrub_payment_fields($payment_method, $fields) {
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