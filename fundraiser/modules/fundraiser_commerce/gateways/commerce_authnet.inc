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
    'form callback' => 'commerce_authnet_fundraiser_commerce_form',
    'cleanup callback' => 'commerce_authnet_fundraiser_commerce_cleanup',
    'scrub callback' => 'commerce_authnet_fundraiser_commerce_scrub',
    'expire callback' => 'commerce_authnet_fundraiser_commerce_expire',
    'charge callback' => 'commerce_authnet_fundraiser_commerce_charge',
    'update callback' => 'commerce_authnet_fundraiser_commerce_update',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_authnet_fundraiser_commerce_form($payment_method) {
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
function commerce_authnet_fundraiser_commerce_cleanup($submission_fields) {
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
 * Callback function, scrub the data before saving.
 */
function commerce_authnet_fundraiser_commerce_scrub($payment_method, $fields) {
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
function commerce_authnet_fundraiser_commerce_expire($submission_fields) {
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
function commerce_authnet_fundraiser_commerce_charge($payment_method, $donation) {
  // This gateway allows recurring data.
  // Based on that - presume that we would normally to use the 'card on file' features provided
  // by commerce_authnet.
  // But card on file does not encrypt things as we need it to, so we're going to need to track our
  // own data and charge systems here. It's a bit of a pain and means we can't readily use the standard
  // submission anymore. But that's as things go.
  // The commerce_authnet module needs to be extensively refactored to allow for automatic posts.
  // TODO: do this as a patch and submit to maintainer.
  if (module_exists('commerce_cardonfile')) {
    return _commerce_authnet_fundraiser_commerce_charge_with_cim($payment_method, $donation);
  }
  else {
    return _commerce_authnet_fundraiser_commerce_charge_without_cim($payment_method, $donation);
  }
}

/**
 * Easy mode, submit to Authnet without CIM.
 */
function _commerce_authnet_fundraiser_commerce_charge_without_cim($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  // Call to commerce_authnet_aim_submit_form_submit() based on instance callbacks.
  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.
  $pane_values = array(
    'credit_card' => array(
      'number' => isset($donation->donation['payment_fields']['credit']['card_number']) ? 
        $donation->donation['payment_fields']['credit']['card_number'] : '',
      'exp_month' => isset($donation->donation['payment_fields']['credit']['card_expiration_month']) ?
        $donation->donation['payment_fields']['credit']['card_expiration_month'] : '',
      'exp_year' => isset($donation->donation['payment_fields']['credit']['card_expiration_year']) ?
        $donation->donation['payment_fields']['credit']['card_expiration_year'] : '',
      'code' => isset($donation->donation['payment_fields']['credit']['card_cvv']) ?
        $donation->donation['payment_fields']['credit']['card_cvv'] : '',
    ),
  );
  $payment_method['settings']['cim_cardonfile'] = FALSE;
  // Execute call to commerce_authnet, without CIM enabled (in case card on file is turned on).
  $result = commerce_authnet_aim_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Hard mode, without card on file, we need to replicate the work of it in order to re-use the same values
 * from regular CIM. For now those, let's just use CIM to establish the behavior before we replicate.
 */
function _commerce_authnet_fundraiser_commerce_charge_with_cim($payment_method, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  $order = commerce_order_load($donation->did);
  $charge = $order->commerce_order_total[LANGUAGE_NONE][0];
  // Call to commerce_authnet_aim_submit_form_submit() based on instance callbacks.
  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.
  $pane_values = array(
    'credit_card' => array(
      'number' => isset($donation->donation['payment_fields']['credit']['card_number']) ? 
        $donation->donation['payment_fields']['credit']['card_number'] : '',
      'exp_month' => isset($donation->donation['payment_fields']['credit']['card_expiration_month']) ?
        $donation->donation['payment_fields']['credit']['card_expiration_month'] : '',
      'exp_year' => isset($donation->donation['payment_fields']['credit']['card_expiration_year']) ?
        $donation->donation['payment_fields']['credit']['card_expiration_year'] : '',
      'code' => isset($donation->donation['payment_fields']['credit']['card_cvv']) ?
        $donation->donation['payment_fields']['credit']['card_cvv'] : '',
    ),
  );
  if (strlen($pane_values['credit_card']['exp_month']) == 1) {
    $pane_values['credit_card']['exp_month'] = '0' . $pane_values['credit_card']['exp_month'];
  }
  // Account for any CIM if stored on file, we have everything we need here - no need to pull out
  // of the donation object or habitually load this value from the donation. This may need to be
  // revised later, but for now I don't think we need to pull sustainer data with the donation.
  // Commerce needs to execute billing update to call into the gateway to update on asociated CIMs.
  // As a result, sustainer has NO NEED for cim to be loaded into the donation object. Those tokens Go Away.
  // If possible at least.
  // TODO - add encryption?
  $payment_method['settings']['cim_cardonfile'] = TRUE; // Card on file,
  $pane_values['credit_card']['cardonfile_store'] = TRUE; // And store it please.
  $pane_values['cardonfile'] = 'new'; // Make a new CIM unless we already have one on record.
  $stored_cards = commerce_cardonfile_data_load_multiple($donation->uid, $payment_method['instance_id']);
  // Filter out expired cards.
  foreach ($stored_cards as $card_id => $card_data) {
    if ($card_data['card_exp_year'] < date('Y') ||
      $card_data['card_exp_year'] == date('Y') && $card_data['card_exp_month'] < date('m')) {
      unset($stored_cards[$card_id]);
    }
  }
  if (!empty($stored_cards)) {
    // Then grab the first one.
    foreach ($stored_cards as $card_id => $card_data) {
      $pane_values['cardonfile'] =  $card_id;
    }
  }
  // Gathered pane values now submit.
  $result = commerce_authnet_aim_submit_form_submit($payment_method, $pane_form, $pane_values, $order, $charge);
  // At this point the card has been saved on the user profile.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Callback function, update data stored at the gateway.
 */
function commerce_authnet_fundraiser_commerce_update($payment_method, $donation) {
  // Get the card.
  $stored_cards = commerce_cardonfile_data_load_multiple($donation->uid, $payment_method['instance_id']);
  // Filter out expired cards.
  foreach ($stored_cards as $card_id => $card_data) {
    if ($card_data['card_exp_year'] < date('Y') ||
      $card_data['card_exp_year'] == date('Y') && $card_data['card_exp_month'] < date('m')) {
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
        'firstName' =>  substr($first_name, 0, 50),
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
