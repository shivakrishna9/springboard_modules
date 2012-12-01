<?php

/**
 * @file
 * This file includes generic credit card functions applicable to most credit transactions. These functions
 * can be overridden by individual gateway modules as needed.
 */


/** 
 * Generic callback function for the render_callback of the credit payment method. 
 */
function gateway_manager_credit_form_render(&$form, &$form_state) {
  // Get the generic form for credit card transactions
  $credit_form = gateway_manager_credit_form();
  // Assign a validation handler (TODO: is there a better place to assign this? feels ugly.)
  $form['#validate'][] = 'gateway_manager_credit_form_validate';
  // Allow other modules to alter the form via hook_credit_payment_form_alter()
  drupal_alter('credit_payment_form', $credit_form, $form, $form_state);
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
  
  // Allow other modules to alter the payment details via hook_payment_details_alter()
  drupal_alter('payment_details', $payment_details, 'credit', $order);
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
 * Validation handler for gateway_manager_credit_form.
 *
 * TODO: Figure out the correct paths to use in form_set_error.
 */
function gateway_manager_credit_form_validate($form, &$form_state) {
  $card_number = $form_state['values']['card_number'];
  $card_expiration_month = $form_state['values']['expiration_date']['card_expiration_month'];
  $card_expiration_year = $form_state['values']['expiration_date']['card_expiration_year'];
  $card_cvv = $form_state['values']['card_cvv'];

  // Validate credit card number
  if (!empty($card_number) && ((variable_get('uc_credit_validate_numbers', TRUE) && !_valid_card_number($card_number)) || !ctype_digit($card_number))) {
    form_set_error('submitted][payment_fields][card_number', t('You have entered an invalid credit card number.'));
  }
    
  // Validate the card expiration date
  if (!empty($card_expiration_month) && !empty($card_expiration_month) && !_valid_card_expiration($card_expiration_month, $card_expiration_year)) {
    form_set_error('submitted][payment_fields][expiration_date][card_expiration_month', t('The credit card you entered has expired.'));
    form_set_error('submitted][payment_fields][expiration_date][card_expiration_year', ' ');
  }

  // Validate the CVV number
  if (!empty($card_cvv) && variable_get('uc_credit_cvv_enabled', TRUE) && !_valid_cvv($card_cvv)) {
    form_set_error('submitted][payment_fields][card_cvv',t('You have entered an invalid CVV number.'));
  }
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