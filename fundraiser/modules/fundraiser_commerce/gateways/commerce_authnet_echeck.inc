<?php

/**
 * @file
 * Commerce based hook for commerce_authnet EFT.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_authnet_echeck_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('bank account'),
    'form callback' => 'commerce_authnet_echeck_fundraiser_commerce_submit_form',
    'cleanup callback' => 'commerce_authnet_echeck_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_authnet_echeck_fundraiser_commerce_scrub',
    'validate callback' => 'commerce_authnet_echeck_fundraiser_commerce_validate',
    'charge callback' => 'commerce_authnet_echeck_fundraiser_commerce_charge',
  );
}

/**
 * Implements hook_fundraiser_donation_form_config_check().
 */
function commerce_authnet_echeck_fundraiser_donation_form_config_check($node) {
  foreach ($node->gateways as $method => $gateway) {
    if (!empty($gateway['id']) && !empty($gateway['status'])) {
      $gateway_config = _fundraiser_gateway_info($gateway['id']);
      if ($gateway_config['gateway_details']['method_id'] == 'commerce_authnet_echeck' &&
        $gateway_config['gateway_details']['settings']['txn_mode'] != AUTHNET_TXN_MODE_LIVE) {
        return array(t('The authnet EFT payment gateway is currently in !mode mode and will not process live transactions.', array('!mode' => $gateway_config['gateway_details']['settings']['txn_mode'])));
      }
    }
  }
}

function commerce_authnet_echeck_fundraiser_gateway_status_cron_check($details) {
  if ($details['method_id'] == 'commerce_authnet_echeck') {
    if ($details['settings']['txn_mode'] != AUTHNET_TXN_MODE_LIVE) {
      return $details['settings']['txn_mode'];
    }
    else {
      return 'live';
    }
  }
}

/**
 * Returns the form fields for this method.
 *
 * @param string $payment_method
 *   Credit, bank account, paypal, etc.
 * @param array $config
 *   Configuration options for this field.
 */
function commerce_authnet_echeck_fundraiser_commerce_submit_form($payment_method, $config) {
  module_load_include('inc', 'commerce_authnet_echeck', 'includes/commerce_authnet_echeck.echeck');
  $method_instance = commerce_payment_method_instance_load($config['id']);
  $form = commerce_authnet_echeck_submit_form($method_instance);
  // Remove the required flag for the fields
  foreach (element_children($form['bank account']) as $field_name) {
    $form['bank account'][$field_name]['#required'] = FALSE;
  }
  return $form['bank account'];
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_authnet_echeck_fundraiser_commerce_scrub($fields) {
  // Scrub sensitive donation fields if they exists.
  if (isset($fields['bank account']['aba_code'])) {
    $fields['bank account']['aba_code'] = substr($fields['bank account']['aba_code'], -3);
  }
  if (isset($fields['bank account']['acct_num'])) {
    $fields['bank account']['acct_num'] = substr($fields['bank account']['acct_num'], -3);
  }
  // Unset confirmation for account number
  unset($fields['bank account']['confirm_acct_num']);
  return $fields;
}

/**
 * Validate the submitted values with the authnet_commerce validate function.
 */
function commerce_authnet_echeck_fundraiser_commerce_validate($submission_fields, $payment_fields) {
  module_load_include('inc', 'commerce_authnet_echeck', 'includes/commerce_authnet_echeck.echeck');
  $form_parents = array_merge($payment_fields['#parents'], array('bank account'));
  $values = array('bank account' => $submission_fields['payment_fields']['bank account']);
  commerce_authnet_echeck_submit_form_validate(NULL, $payment_fields, $values, NULL, $form_parents);
  return $submission_fields;
}

/**
 * Submit the donation values to the Authnet Echeck charge handler.
 *
 * @param array $method_instance
 *   Commerce loaded method instance.
 * @param object $donation
 *   The donation to charge.
 *
 * @return bool
 *   Whether the charge was successful.
 */
function commerce_authnet_echeck_fundraiser_commerce_charge($method_instance, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();
  // Not actually used anywhere in this system, so ignore it.
  $pane_form = array();
  $pane_values = array(
    'bank account' => $donation->donation['payment_fields']['bank account']
  );
  // Execute call to Authnet, only returns FALSE if the payment failed,
  // returns nothing if successful.
  $success = FALSE;
  $result = commerce_authnet_echeck_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

