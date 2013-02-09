<?php

/**
 * @file
 * Commerce based hook for commerce_ogone.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_ogone_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit', 'paypal'),
    'offsite_processing' => array('paypal'),
    'form callback' => 'commerce_ogone_fundraiser_commerce_paypal_form',
    'scrub callback' => 'commerce_ogone_fundraiser_commerce_scrub_payment_fields',
    'cleanup callback' => 'commerce_ogone_fundraiser_commerce_credit_card_submit_form',
    'expire callback' => 'commerce_ogone_fundraiser_commerce_credit_card_expiration_form',
    'redirect callback' => 'commerce_ogone_fundraiser_commerce_execute_redirect',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_ogone_fundraiser_commerce_paypal_form($payment_method) {
  switch ($payment_method) {
    case 'paypal':
      $form['paypal_email'] = array(
        '#type' => 'textfield',
        '#title' => t('Paypal email'),
      );
    break;
    case 'credit':
    default:
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
    break;
  }
  return $form;
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_ogone_fundraiser_commerce_credit_card_submit_form($submission_fields) {
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
function commerce_ogone_fundraiser_commerce_credit_card_expiration_form($submission_fields) {
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
 * Implements hook_fundraiser_commerce_scrub_payment_fields().
 */
function commerce_ogone_fundraiser_commerce_scrub_payment_fields($payment_method, $fields) {
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
 * Implements hook_fundraiser_commerce_execute_redirect().
 */
function commerce_ogone_fundraiser_commerce_execute_redirect($payment_method, $donation, $order, $settings) {
  return drupal_get_form('commerce_ogone_build_redirect_form', $order, $payment_method['settings'] + $settings);
}

/**
 * Implements hook_fundraiser_commerce_execute_redirect_validate().
 */
function commerce_ogone_fundraiser_commerce_execute_redirect_validate($method_instance, $donation, $order) {
  $result = FALSE;// commerce_paypal_wps_redirect_form_validate($order, $method_instance);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Implements hook_fundraiser_commerce_execute_redirect_submit().
 */
function commerce_ogone_fundraiser_commerce_execute_redirect_submit($method_instance, $donation, $order) {
  // IMPORTANT: Transaction handling for Paypal is handled on a differnt URL, when Paypal returns the IPN tracsaction ID. 
  // Therefore no additional transaction work occurs here.
  return TRUE;
}