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
    'allow_recurring' => FALSE,
    // ALL currently supported ubercart gateways use the same field set.
    'form callback' => 'commerce_ogone_fundraiser_commerce_paypal_form',
    'form validate callback' => 'commerce_ogone_fundraiser_commerce_credit_card_submit_form',
    'form submit callback' => 'commerce_ogone_fundraiser_commerce_credit_card_submit_form',
    'form expiration callback' => 'commerce_ogone_fundraiser_commerce_credit_card_expiration_form',
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
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_ogone_fundraiser_commerce_execute_gateway($payment_method, $donation) {
  drupal_goto('fundraiser-commerce/redirect/' . $donation->did);
  // Results and returns to be handled after the redirect return.
}

/**
 * Implements hook_fundraiser_commerce_redirect_form().
 */
function commerce_ogone_fundraiser_commerce_redirect_form($form, &$form_state, $order, $payment_method) {
  // Set feedback URLs
  $settings = array(
    // Return to the previous page when payment is canceled
    'cancel_return' => url('fundraiser-commerce/redirect/' . $order->order_id . '/back/' . $order->data['payment_redirect_key'], array('absolute' => TRUE)),

    // Return to the payment redirect page for processing successful payments
    'return' => url('fundraiser-commerce/redirect/' . $order->order_id . '/return/' . $order->data['payment_redirect_key'], array('absolute' => TRUE)),

    // Specify the current payment method instance ID in the notify_url
    'payment_method' => $payment_method['instance_id'],
  );

  return commerce_ogone_build_redirect_form($form, $form_state, $order, $payment_method['settings'] + $settings);
}