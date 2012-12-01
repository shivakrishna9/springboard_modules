<?php

/**
 * @file
 * This file includes generic credit card functions applicable to most credit transactions. These functions
 * can be overridden by individual gateway modules as needed.
 */


/** 
 * Generic callback function for the render_callback of the credit payment method. 
 */
function gateway_manager_credit_form_render($form, &$form_state) {
  // Get the generic form for credit card transactions
  $credit_form = gateway_manager_credit_form();
  // Allow other modules to alter the form via hook_gateway_manager_credit_form_alter()
  drupal_alter('gateway_manager_credit_form', $credit_form, $form, $form_state);
  // Return the rendered form
  return $credit_form;
}

/**
 * Generic callback function for the payment_details_callback of the credit payment method.
 *
 * @param $order
 *   The ubercart order.
 * @param $submission
 *   The donation form submission.
 *
 * @return
 *   The payment details array in ubercart format.
 */
function gateway_manager_credit_payment_details($order, $submitted) {

  // Extract the payment details from the submission array.
  $payment_details = array(
    'cc_type' => _fundraiser_get_cc_type($submitted['card_number']),
    'cc_owner' => '',
    'cc_number' => $submitted['card_number'],
    'cc_start_month' => '',
    'cc_start_year' => '',
    'cc_exp_month' => $submitted['card_expiration_month'],
    'cc_exp_year' => $submitted['card_expiration_year'],
    'cc_issue' => '',
    'cc_cvv' => $submitted['card_cvv'],
    'cc_bank' => '',
  );
  
  return $payment_details;
}

/**
 * Constructs a form for collecting basic credit card information.
 */
function gateway_manager_credit_form() {

  $form['card_number'] = array(
    '#title' => t('Card Number'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE,
    '#weight' => 0,
  );

  $form['expiration_date'] = array(
    'card_expiration_month' => _gateway_manager_card_expiration_month_field(),
    'card_expiration_year' => _gateway_manager_card_expiration_year_field(),
    '#theme' => 'fundraiser_credit_card_expiration_date', // TODO - move out of fundraiser
    '#weight' => 1,
  );

  $form['card_cvv'] = array(
    '#title' => t('Card Security Code'),
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#weight' => 2,
  );

  return $form;
}

/**
 * Defines the credit card expiration month field.
 */
function _gateway_manager_card_expiration_month_field() {
  return array(
    '#title' => t('Expiration Month'),
    '#type' => 'select',
    '#options' => array(1 => 'January'),
    '#required' => TRUE,
  );
}

/**
 * Defines the credit card expiration year field.
 */
function _gateway_manager_card_expiration_year_field() {
  return array(
    '#title' => t('Expiration Year'),
    '#type' => 'select',
    '#options' => drupal_map_assoc(array(2012, 2013)),
    '#required' => TRUE,
  );
}