<?php

/**
 * @file
 * Commerce based hook for commerce_litle.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_litle_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    // Create the payment_method array with the commerce gateway
    // method info name as the key.
    'payment_method' => array('commerce_litle_cc' => 'credit', 'commerce_litle_echeck' => 'bank account'),
    // @todo bank account (echeck) recurring is not yet fully implemented.
    'allow_recurring' => array('credit'),
    'allow_refund' => array('credit'),
    'form callback' => 'commerce_litle_fundraiser_commerce_submit_form',
    'cleanup callback' => 'commerce_litle_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_litle_fundraiser_commerce_scrub',
    'validate callback' => 'commerce_litle_fundraiser_commerce_validate',
    'expire callback' => 'commerce_litle_fundraiser_commerce_expire',
    'charge callback' => 'commerce_litle_fundraiser_commerce_charge',
    'refund callback' => 'commerce_litle_fundraiser_commerce_refund',
    'update callback'     => 'commerce_litle_fundraiser_commerce_update',
    'cardonfile callback' => 'commerce_litle_fundraiser_commerce_cardonfile',
  );
}

/**
 * Returns the form fields for this method.
 */
function commerce_litle_fundraiser_commerce_submit_form($payment_method) {
  if ($payment_method == 'credit') {
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    return _fundraiser_commerce_credit_card_form();
  }
  elseif ($payment_method == 'bank account') {
    $form = commerce_litle_echeck_submit_form();
    return $form['echeck'];
  }
}

/**
 * Convert values to ones that Fundraiser Commerce is looking for.
 */
function commerce_litle_fundraiser_commerce_cleanup($submission_fields) {
  if ($submission_fields['payment_method'] == 'credit') {
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    return _fundraiser_commerce_credit_card_cleanup($submission_fields);
  }
  return $submission_fields;
}

/**
 * Callback function.
 *
 * Return the user entered expiration dates for this submission.
 */
function commerce_litle_fundraiser_commerce_expire($submission_fields) {

  if ($submission_fields['payment_method'] == 'bank account') {
    // Pretend expiration date for sustainers.
    $exp_timestamp = strtotime('+12 months');

    return array(
      'month' => date('m', $exp_timestamp),
      'year' => date('Y', $exp_timestamp),
    );
  }

  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_litle_fundraiser_commerce_scrub($submission_fields, $payment_method) {
  if ($payment_method == 'credit') {
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    return _fundraiser_commerce_credit_card_scrub($submission_fields);
  }
  elseif ($payment_method == 'bank account') {
    // Scrub sensitive donation fields if they exist.
    if (isset($submission_fields['bank account']['accNum'])) {
      $submission_fields['bank account']['accNum'] = substr($submission_fields['bank account']['accNum'], -3);
    }
    if (isset($submission_fields['bank account']['account_number'])) {
      $submission_fields['bank account']['accNum'] = substr($submission_fields['bank account']['accNum'], -3);
    }
    return $submission_fields;
  }
}

/**
 * Validate the submitted values with the commerce validate function.
 */
function commerce_litle_fundraiser_commerce_validate($submission_fields, $payment_fields, $payment_method) {
  // Pass the values to the validation scripts.
  if ($payment_method == 'credit') {
    $form_parents = array_merge($payment_fields['#parents'], array('credit'));
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    _fundraiser_commerce_credit_card_validate($submission_fields, $form_parents);
  }
  elseif ($payment_method == 'bank account') {
    $form_parents = array_merge($payment_fields['#parents'], array('bank account'));
    // Pass the values array with the key the litle module expects.
    $values = array('echeck' => $submission_fields['payment_fields']['bank account']);
    commerce_litle_echeck_submit_form_validate(NULL, $payment_fields, $values, NULL, $form_parents);
  }

  return $submission_fields;
}

/**
 * Submit the donation values to the Litle charge handler.
 */
function commerce_litle_fundraiser_commerce_charge($method_instance, $donation) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  // Not actually used anywhere in this system, so ignore it.
  $pane_form = array();

  $pane_values = array();

  // Execute call to Litle depending on what payment method we have.
  if ($donation->donation['payment_method'] == 'credit') {
    $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

    // Add fundraiser commerce data to the pane values array.
    _fundraiser_commerce_submit_handler_pane_values($pane_values, $donation);

    $method_instance['settings']['cardonfile'] = FALSE;

    if (module_exists('commerce_cardonfile')) {
      // Card on file,
      $method_instance['settings']['cardonfile'] = TRUE;
    }

    $result = commerce_litle_cc_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  }
  elseif ($donation->donation['payment_method'] == 'bank account') {
    $pane_values['echeck'] = $donation->donation['payment_fields']['bank account'];

    $method_instance['settings']['cardonfile'] = FALSE;

    if (module_exists('commerce_cardonfile')) {
      // Card on file,
      $method_instance['settings']['cardonfile'] = TRUE;
      // And store it please.
      $pane_values['echeck']['cardonfile_store'] = TRUE;
      $pane_values['cardonfile'] = commerce_litle_get_cardonfile_pane_value($donation->uid, $method_instance['instance_id']);
    }

    $result = commerce_litle_echeck_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  }

  // Returns FALSE if the payment failed, returns nothing if successful.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }

  // Perform post processing functions.
  _fundraiser_commerce_charge_submit_form_process($success, $method_instance, $pane_values, $donation);

  return $success;
}

/**
 * Callback function, update card data stored at the gateway and locally.
 *
 * @param object $donation
 *   Fundraiser commerce donation object which should include the
 *   new/updated card data.
 * @param object $card_data
 *   Card on file entity, this should be the current entity for the donation,
 *   new data should be passed in the donation object.
 *
 * @return object
 *   Card on file entity for the new/updated card data.
 */
function commerce_litle_fundraiser_commerce_update($donation, $card_data = NULL) {

  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $payment_method = $donation->gateway['gateway_details'];

  // If no card data was passed load the profile from the donation if its set.
  if (empty($card_data) && !empty($donation->data['cardonfile'])) {
    $card_data = commerce_cardonfile_load($donation->data['cardonfile']);
  }

  if (empty($card_data)) {
    // Log the missing card data.
    watchdog(
      'fundraiser',
      'A card update was attempted on donation @id but no stored card data was found.',
      array('@id' => $donation->did),
      WATCHDOG_CRITICAL
    );
    return;
  }

  // Pull the new credit card values from the donation.
  $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

  // Load the card on file values for this donation.
  $cardonfile_fields = _fundraiser_commerce_credit_card_cardonfile_fields($donation);

  // Check the card number to see if it matches the current card data number.
  // If it does then we use the update function.
  // if its a new card check if we should create a new vault profile.
  $action = 'update';
  if ($cardonfile_fields['card_number'] != $card_data->card_number) {
    // This is a new card for this donation,
    // make sure we don't have an existing profile.
    // First check with the card number and date.
    if ($existing = _fundraiser_commerce_cardonfile_match($cardonfile_fields)) {
      // Set the existing card on the donation and return it.
      $donation->data['cardonfile'] = $existing->card_id;
      $card_data = $existing;
    }
    // Check if we have a mismatched exp date record.
    elseif ($mismatch = _fundraiser_commerce_cardonfile_exp_date_mismatch($cardonfile_fields)) {
      $donation->data['cardonfile'] = $mismatch->card_id;
      $card_data = $mismatch;
    }
    // Nothing stored locally so create a new profile.
    else {
      $action = 'new';
    }
  }

  if ($action == 'update') {
    $card_data->card_number = $cardonfile_fields['card_number'];
    $card_data->card_exp_month = $cardonfile_fields['card_exp_month'];
    $card_data->card_exp_year = $cardonfile_fields['card_exp_year'];

    $account_number = $pane_values['credit_card']['number'];
    $card_validation_number = $pane_values['credit_card']['code'];
    $response = commerce_litle_cc_vault_cardonfile_register_token($payment_method, $account_number, $card_validation_number);

    $token = commerce_litle_get_token_from_response($response);
    if (!empty($token)) {
      $card_data->remote_id = $token;
      commerce_cardonfile_save($card_data);
      return $card_data;
    }

  }
  // Create a new card on file record.
  elseif ($action == 'new') {
    $order = commerce_order_load($donation->did);

    $account_number = $pane_values['credit_card']['number'];
    $card_validation_number = $pane_values['credit_card']['code'];
    $response = commerce_litle_cc_vault_cardonfile_register_token($payment_method, $account_number, $card_validation_number, $order->id);

    $token = commerce_litle_get_token_from_response($response);

    // If the Vault token creation was a success,
    // store the new card on file data locally.
    if (!empty($token)) {

      // Get the card type.
      $card_type = commerce_payment_validate_credit_card_type($pane_values['credit_card']['number'], array_keys(commerce_payment_credit_card_types()));
      $all_types = commerce_payment_credit_card_types();
      $card_type = !empty($all_types[$card_type]) ? $all_types[$card_type] : 'card';

      // Store the token with cardonfile.
      $card_data = commerce_cardonfile_new();
      $card_data->uid = $order->uid;
      $card_data->payment_method = $payment_method['method_id'];
      $card_data->instance_id = $payment_method['instance_id'];
      $card_data->remote_id = $token;
      $card_data->card_type = !empty($card_type) ? $card_type : 'card';
      $card_data->card_name = !empty($billing_address['name_line']) ? $billing_address['name_line'] : '';
      $card_data->card_number = substr($pane_values['credit_card']['number'], -4);
      $card_data->card_exp_month = $pane_values['credit_card']['exp_month'];
      $card_data->card_exp_year = $pane_values['credit_card']['exp_year'];
      $card_data->status = 1;

      // Save and log the creation of the new card on file.
      commerce_cardonfile_save($card_data);
      watchdog('commerce_litle', 'GUID @guid registered and saved to user @uid.', array('@guid' => $guid, '@uid' => $order->uid));

      // Set the value on the donation.
      $donation->data['cardonfile'] = $card_data->card_id;
      return $card_data;
    }
  }
}

/**
 * Card on file callback.
 *
 * We use a custom callback since we may need to update an existing vault
 * profile.
 */
function commerce_litle_fundraiser_commerce_cardonfile($donation) {
  // Use standard function to setup the card on file fields.
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $cardonfile_fields = _fundraiser_commerce_credit_card_cardonfile_fields($donation);

  // @todo Does litle care about expiration date?
  // So we need to search locally to see if we have an existing profile for
  // a card number.
  // Only perform this when we're in an active non-reference charge.
//  if (isset($donation->reference_charge) && $donation->reference_charge === FALSE) {
//    // Query the db for an existing record
//    $card_data = _fundraiser_commerce_cardonfile_exp_date_mismatch($cardonfile_fields);
//
//    // If we have a profile for this user update it in the CIM
//    if ($card_data) {
//      // Make the update request to authorize.net
//      commerce_authnet_fundraiser_commerce_update($donation, $card_data);
//    }
//  }

  return $cardonfile_fields;
}

/**
 * Returns the card_id of an unexpired, active card on file.
 *
 * @deprecated
 *
 * If there isn't one, returns 'new'.
 *
 * @param int $uid
 *   User ID.
 * @param int $instance_id
 *   Payment method instance ID.
 *
 * @return string
 *   The string 'new' or a card_id from cardonfile.
 */
function commerce_litle_get_cardonfile_pane_value($uid, $instance_id) {
  // Make a new token unless we already have one on record.
  $return = 'new';

  $stored_cards = commerce_cardonfile_load_multiple_by_uid($uid, $instance_id);

  if ($stored_cards == FALSE) {
    return $return;
  }

  // Filter out expired cards.
  foreach ($stored_cards as $card_id => $card_data) {
    // Special case for echecks.
    if ($card_data->card_exp_year == 0 && $card_data->card_exp_month == 0) {
      continue;
    }

    if ($card_data->card_exp_year < date('Y') ||
      $card_data->card_exp_year == date('Y') && $card_data->card_exp_month < date('m')) {
      unset($stored_cards[$card_id]);
    }
  }

  if (!empty($stored_cards)) {
    // Then grab the first one.
    foreach ($stored_cards as $card_id => $card_data) {
      $return = $card_id;
    }
  }

  return $return;
}

/**
 * Refund callback for Litle.
 *
 * Wraps the admin form used for refund with our own system.
 */
function commerce_litle_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $refunding_order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $refunding_order->order_id));
  // The first one is the original donation.
  $transaction = array_shift($transactions);

  // Set up the values needed for the transaction to go through.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  // Submit the donation form.
  module_load_include('inc', 'commerce_litle', 'commerce_litle.admin');
  return commerce_litle_cc_credit_request($payment_method, $transaction, $refunding_order, $refund->amount);
}
