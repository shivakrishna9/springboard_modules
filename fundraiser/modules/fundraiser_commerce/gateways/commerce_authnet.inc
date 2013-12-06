<?php

/**
 * @file
 * Commerce based hook for commerce_authnet.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_authnet_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit'),
    'allow_recurring' => array('credit'),
    'allow_refund' => array('credit'),
    'form callback' => 'commerce_authnet_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_authnet_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_authnet_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_authnet_fundraiser_commerce_expire',
    'validate callback' => 'commerce_authnet_fundraiser_commerce_validate',
    'charge callback' => 'commerce_authnet_fundraiser_commerce_charge',
    'update callback' => 'commerce_authnet_fundraiser_commerce_update',
    'refund callback' => 'commerce_authnet_fundraiser_commerce_refund',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_authnet_fundraiser_commerce_form($payment_method) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_form();
}

/**
 * Callback function, clean up the submission results after submission before they are returned to fundraiser.
 */
function commerce_authnet_fundraiser_commerce_cleanup($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_cleanup($submission_fields);
}

/**
 * Callback function, scrub the data before saving.
 */
function commerce_authnet_fundraiser_commerce_scrub($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_scrub($submission_fields);
}

/**
 * Callback function, return the user entered experation dates for this submission.
 */
function commerce_authnet_fundraiser_commerce_expire($submission_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  return _fundraiser_commerce_credit_card_expire($submission_fields);
}

/**
 * Validate the submitted values with the commerce validate function
 */
function commerce_authnet_fundraiser_commerce_validate($submission_fields, $payment_fields) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $form_parents = array_merge($payment_fields['#parents'], array('credit'));
  _fundraiser_commerce_credit_card_validate($submission_fields, $form_parents);
  return $submission_fields;
}

/**
 * Callback function, charge gateway.
 */
function commerce_authnet_fundraiser_commerce_charge($method_instance, $donation) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

  // Pad single digit exp months for authnet validation
  if (strlen($pane_values['credit_card']['exp_month']) == 1) {
    $pane_values['credit_card']['exp_month'] = '0' . $pane_values['credit_card']['exp_month'];
  }

  // Add fundraiser commerce data to the pane values array
  _fundraiser_commerce_submit_handler_pane_values($pane_values, $donation);

  // For commerce authnet we need to do our own cardonfile storage below, so we set the storage flag to false
  $pane_values['credit_card']['cardonfile_store'] = FALSE;

  // Execute the authnet submit handler
  $result = commerce_authnet_aim_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }

  // For commerce authnet we store a CIM profile for each order, not just per user
  if ($success && module_exists('commerce_cardonfile') && $donation->reference_charge == FALSE) {
    _commerce_authnet_fundraiser_cim_profile_create($pane_values, $method_instance, $order, $donation);
  }

  return $success;
}

/**
 * CIM profile creation so we get a new profile for each order
 *
 * Also see fundraiser_commerce_commerce_authnet_cim_request_alter
 */
function _commerce_authnet_fundraiser_cim_profile_create($pane_values, $method_instance, $order, $donation) {
  $payment_details = array(
    'cardNumber' => $pane_values['credit_card']['number'],
    'expirationDate' => $pane_values['credit_card']['exp_year'] . '-' . $pane_values['credit_card']['exp_month'],
  );

  if (isset($pane_values['credit_card']['code'])) {
    $payment_details['cardCode'] = $pane_values['credit_card']['code'];
  }

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $billing_address = $order_wrapper->commerce_customer_billing->commerce_customer_address->value();

  if ($response = commerce_authnet_cim_create_customer_profile_request($method_instance, $order, $payment_details)) {
   // If the Customer Profile creation was a success, store the new card on
   // file data locally.
   if ((string) $response->messages->resultCode == 'Ok') {
      // Build a remote ID that includes the Customer Profile ID and the
      // Payment Profile ID.
      $remote_id = (string) $response->customerProfileId . '|' . (string) $response->customerPaymentProfileIdList->numericString;
      $card_data = commerce_cardonfile_new();
      $card_data->uid = $order->uid;
      $card_data->payment_method = $method_instance['method_id'];
      $card_data->instance_id = $method_instance['instance_id'];
      $card_data->remote_id = $remote_id;
      $card_data->card_type = !empty($pane_values['credit_card']['card_type']) ? $pane_values['credit_card']['card_type'] : 'card';
      $card_data->card_name = !empty($billing_address['name_line']) ? $billing_address['name_line'] : '';
      $card_data->card_number = substr($pane_values['credit_card']['number'], -4);
      $card_data->card_exp_month = $pane_values['credit_card']['exp_month'];
      $card_data->card_exp_year = $pane_values['credit_card']['exp_year'];
      $card_data->status = 1;

      // Save and log the creation of the new card on file.
      commerce_cardonfile_save($card_data);

      // Save this to the donation data array
      $donation->data['cardonfile'] = $card_data->card_id;

      watchdog('commerce_authnet', 'CIM Customer Profile @profile_id created and saved to user @uid.', array('@profile_id' => (string) $response->customerProfileId, '@uid' => $order->uid));
    }
  }
}

/**
 * Callback function, update data stored at the gateway.
 */
function commerce_authnet_fundraiser_commerce_update($payment_method, $donation) {
  // Get the card.
  $stored_cards = commerce_cardonfile_load_multiple_by_uid($donation->uid, $payment_method['instance_id']);
  // Filter out expired cards.
  foreach ($stored_cards as $card_id => $card_data) {
    if ($card_data->card_exp_year < date('Y') ||
      $card_data->card_exp_year == date('Y') && $card_data->card_exp_month < date('m')) {
      unset($stored_cards[$card_id]);
    }
  }
  $card_data = '';
  if (!empty($stored_cards)) {
    // Then grab the first one.
    foreach ($stored_cards as $card_id => $this_card) {
      $card_data = $this_card;
    }
  }

  // Donation card data.
  $additional_data = array(
    'card_number' => isset($donation->donation['payment_fields']['credit']['card_number']) ?
    $donation->donation['payment_fields']['credit']['card_number'] : '',
    'card_exp_month' => isset($donation->donation['payment_fields']['credit']['card_expiration_month']) ?
    $donation->donation['payment_fields']['credit']['card_expiration_month'] : '',
    'card_exp_year' => isset($donation->donation['payment_fields']['credit']['card_expiration_year']) ?
    $donation->donation['payment_fields']['credit']['card_expiration_year'] : '',
  );
  $card_data = array_merge($card_data, $additional_data);
  if (strlen($card_data['card_exp_month']) == 1) {
    $card_data['card_exp_month'] = '0' . $card_data['card_exp_month']; // Format the date value.
  }
  // Set additional values.
  $form['credit_card']['number']['#default_value'] = '';
  $form_state['values']['credit_card']['number'] = isset($donation->donation['payment_fields']['credit']['card_number']) ?
    $donation->donation['payment_fields']['credit']['card_number'] : '';
  $form_state['values']['credit_card']['owner'] = isset($donation->donation['first_name']) ? $donation->donation['first_name'] : '';
  $form_state['values']['credit_card']['owner'] .= isset($donation->donation['last_name']) ? ' ' . $donation->donation['last_name'] : '';
  // Additional fields.
  $form_state['values']['credit_card']['address'] = isset($donation->donation['address']) ? $donation->donation['address'] : '';
  $form_state['values']['credit_card']['address'] .= isset($donation->donation['address_line_2']) ? ' ' . $donation->donation['address_line_2'] : '';
  $form_state['values']['credit_card']['city'] = isset($donation->donation['city']) ? $donation->donation['city'] : '';
  $form_state['values']['credit_card']['state'] = isset($donation->donation['state']) ? $donation->donation['state'] : '';
  $form_state['values']['credit_card']['zip'] = isset($donation->donation['zip']) ? $donation->donation['zip'] : '';
  $form_state['values']['credit_card']['country'] = isset($donation->donation['country']) ? $donation->donation['country'] : '';

  if (!empty($card_data)) {
    // Calling commerce_authnet_cim_cardonfile_update.
    _commerce_authnet_fundraiser_commerce_authnet_cim_cardonfile_update($form, $form_state, $payment_method, $card_data);
  }
}

/**
 * COPY: commerce_authnet_cim_cardonfile_update
 * Card on file callback: updates the associated customer payment profile.
 *
 * The original function at commerce_authnet_cim_cardonfile_update(), doesn't update the user
 * address fields - instead referencing the cim values saved, so not even updating the customer profile
 * in advance would do it. So we need to copy this over and make the changes we need for fall through.
 */
function _commerce_authnet_fundraiser_commerce_authnet_cim_cardonfile_update($form, &$form_state, $payment_method, $card_data) {
  // Extract the Customer Profile and Payment Profile IDs from the remote_id.
  list($cim_customer_profile_id, $cim_payment_profile_id) = explode('|', $card_data['remote_id']);

  if ($form_state['values']['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    $number = $form_state['values']['credit_card']['number'];
  }
  else {
    $number = 'XXXX' . $card_data['card_number'];
  }

  // Load the payment profile so that billTo can be updated.
  $payment_profile_xml = commerce_authnet_cim_get_customer_payment_profile_request($payment_method, $cim_customer_profile_id, $cim_payment_profile_id);
  $billto = $payment_profile_xml->paymentProfile->billTo;
  $first_name = (string) $billto->firstName;
  $last_name = (string) $billto->lastName;
  $address = (string) $billto->address;
  $city = (string) $billto->city;
  $state = (string) $billto->state;
  $zip = (string) $billto->zip;
  $country = (string) $billto->country;

  // Extract the first and last name from form values.
  if (!empty($form_state['values']['credit_card']['owner'])) {
    $name_parts = explode(' ', $form_state['values']['credit_card']['owner']);
    $first_name = array_shift($name_parts);
    $last_name = implode(' ', $name_parts);
    $address = $form_state['values']['credit_card']['address'];
    $city = $form_state['values']['credit_card']['city'];
    $state = $form_state['values']['credit_card']['state'];
    $zip = $form_state['values']['credit_card']['zip'];
    $country = $form_state['values']['credit_card']['country'];
  }

  // Build the base profile update data.
  $api_request_data = array(
    'customerProfileId' => $cim_customer_profile_id,
    'paymentProfile' => array(
      'billTo' => array(
        'firstName' => substr($first_name, 0, 50),
        'lastName' => substr($last_name, 0, 50),
        'company' => (string) $billto->company,
        'address' => $address,
        'city' => $city,
        'state' => $state,
        'zip' => $zip,
        'country' => $country,
      ),
      'payment' => array(
        'creditCard' => array(
          'cardNumber' => $number,
          'expirationDate' => $card_data['card_exp_year'] . '-' . $card_data['card_exp_month'],
        ),
      ),
      'customerPaymentProfileId' => $cim_payment_profile_id,
    ),
  );

  // Fetch the response from the API server and let Card on File know if the update was successful.
  $xml_response = commerce_authnet_cim_request($payment_method, 'updateCustomerPaymentProfileRequest', $api_request_data);
  return (string) $xml_response->messages->message->code == 'I00001';
}

/**
 * Refund callback for authnet, wraps the admin form used for refund with our own system.
 */
function commerce_authnet_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $refunding_order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $refunding_order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.

  $form = array();
  $form_state = array();
  // Set up the values needed for the transaction to go through.
  $form_state['transaction'] = $transaction;
  $form_state['values']['amount'] = $refund->amount;
  $form_state['order'] = $refunding_order;
  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);
  // Submit the donation form.
  module_load_include('inc', 'commerce_authnet', '/includes/commerce_authnet.admin');
  $result = commerce_authnet_aim_credit_form_submit($form, $form_state);
  // At this point the card has been saved on the user profile.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}
