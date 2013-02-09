<?php

/**
 * @file
 * Commerce based hook for commerce_sagepay_direct.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_sagepay_direct_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'charge callback' => 'commerce_sagepay_direct_fundraiser_commerce_execute_gateway',
    'scrub callback' => 'commerce_sagepay_direct_fundraiser_commerce_scrub_payment_fields',
    'form callback' => 'commerce_sagepay_fundraiser_commerce_credit_card_form',
    'cleanup callback' => 'commerce_sagepay_fundraiser_commerce_credit_card_submit_form',
    'expire callback' => 'commerce_sagepay_fundraiser_commerce_credit_card_expiration_form',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_sagepay_fundraiser_commerce_credit_card_form($payment_method) {
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
function commerce_sagepay_fundraiser_commerce_credit_card_submit_form($submission_fields) {
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
function commerce_sagepay_fundraiser_commerce_credit_card_expiration_form($submission_fields) {
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
function commerce_sagepay_direct_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $pane_form = array(); // Spare, no values here used during submission. Ignore.
  $pane_values = array(
    'credit_card' => array(
      'number' => $donation->donation['payment_fields']['credit']['card_number'],
      'exp_month' => $donation->donation['payment_fields']['credit']['card_expiration_month'],
      'exp_year' => $donation->donation['payment_fields']['credit']['card_expiration_year'],
      'start_month' => '',
      'start_year' => '',
      'issue' => '',
      'code' => $donation->donation['payment_fields']['credit']['card_cvv'],
      'type' => '',
    ),
  );

  // Customer data for charge pulls from the customer profile given on the order, so we can ignore that.
  // If the charge fails, returns false, else nothing.
  $result = commerce_sagepay_direct_submit_form_submit($payment_method, $pane_form, $pane_values, &$order, $charge);
  // No false result returned, we need to use the order status to check for success.
  // ALERT: The submission process here includes a drupal_goto. (whhhhhy!)
  // So we need to copy and paste some work over from the regular module, to avoid that.
  // THIS IS NOT READY FOR PRIMETIME.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Callback, scrubs the fields for a given payment method.
 */
function commerce_sagepay_direct_fundraiser_commerce_scrub_payment_fields($payment_method, $fields) {
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