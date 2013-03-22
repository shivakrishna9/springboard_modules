<?php

/**
 * @file
 * Commerce based hook for commerce_psi
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_psi_check_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('check'),
    'allow_recurring' => array('check'),
    'form callback' => 'commerce_psi_check_fundraiser_commerce_submit_form',
    'cleanup callback' => 'commerce_psi_check_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_psi_check_fundraiser_commerce_scrub',
    'validate callback' => 'commerce_psi_check_fundraiser_commerce_validate',
    'charge callback' => 'commerce_psi_check_fundraiser_commerce_charge',
  );
}

/**
 * Returns the form fields for this method
 * 
 * @param $payment_method String
 *    Credit, check, paypal, etc
 * @param $config Array
 *    Configuration options for this field
 */
function commerce_psi_check_fundraiser_commerce_submit_form($payment_method, $config) {
  $method_instance = commerce_payment_method_instance_load($config['id']);
  $form = commerce_psi_check_submit_form($method_instance);
  // Remove the required flag for the fields
  foreach (element_children($form['bank_account']) as $field_name) {
    $form['bank_account'][$field_name]['#required'] = FALSE;
  }
  return $form['bank_account'];
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_psi_check_fundraiser_commerce_scrub($payment_method, $fields) {
  // Scrub sensitive donation fields if they exists.
  if (isset($fields['check']['routing_number'])) {
    $fields['check']['routing_number'] = substr($fields['check']['routing_number'], -4);
  }
  if (isset($fields['check']['account_number'])) {
    $fields['check']['account_number'] = substr($fields['check']['account_number'], -4);
  }
  // Value added by commerce
  unset($fields['check']['confirm_account_number']);
  return $fields;
}

/**
 * Validate the submitted values with the psi_commerce validate function
 */
function commerce_psi_check_fundraiser_commerce_validate($submission_fields, $payment_fields) {
  $form_parents = array_merge($payment_fields['#parents'], array('check'));
  $values = array('bank_account' => $submission_fields['payment_fields']['check']);
  commerce_psi_check_submit_form_validate(NULL, $payment_fields, $values, NULL, $form_parents);
  return $submission_fields;
}

/**
 * Submit the donation values to the PSI credit charge handler
 * 
 * @param $method_instance Array
 *    Commerce loaded method instance
 * @param $donation Object
 *    Fundraiser donation object
 */
function commerce_psi_check_fundraiser_commerce_charge($method_instance, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  $pane_values = array(
    'bank_account' => $donation->donation['payment_fields']['check'],
    'frequency' => $donation->donation['recurs_monthly'] == TRUE ? 'Monthly' : NULL,
  );

  // Execute call to PSI, only returns FALSE if the payment failed, returns nothing if successful
  $result = commerce_psi_check_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}
