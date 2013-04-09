<?php

/**
 * @file
 * Commerce based hook for commerce_cybersource.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_cybersource_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'form callback' => 'commerce_cybersource_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_cybersource_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_cybersource_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_cybersource_fundraiser_commerce_expire',
    'charge callback' => 'commerce_cybersource_fundraiser_commerce_charge',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_cybersource_fundraiser_commerce_form($payment_method) {
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
function commerce_cybersource_fundraiser_commerce_cleanup($submission_fields) {
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
 * Callback function, scrub data.
 */
function commerce_cybersource_fundraiser_commerce_scrub($payment_method, $fields) {
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
function commerce_cybersource_fundraiser_commerce_expire($submission_fields) {
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
 * Callback function, charge gateway.
 */
function commerce_cybersource_fundraiser_commerce_charge($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  $pane_form = array(); // Spare, no values here used during submission. Ignore.
  $pane_values = array(
    'credit_card' => array(
      'number' => $donation->donation['payment_fields']['credit']['card_number'],
      'exp_month' => $donation->donation['payment_fields']['credit']['card_expiration_month'],
      'exp_year' => $donation->donation['payment_fields']['credit']['card_expiration_year'],
      'code' => $donation->donation['payment_fields']['credit']['card_cvv'],
    ),
  );

  // Customer data for charge pulls from the customer profile given on the order, so we can ignore that.
  // If the charge fails, returns false, else nothing.
  $result = commerce_cybersource_soap_cc_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}
