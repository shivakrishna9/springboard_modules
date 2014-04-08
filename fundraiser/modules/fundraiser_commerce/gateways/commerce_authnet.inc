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
    'cardonfile callback' => 'commerce_authnet_fundraiser_commerce_cardonfile',
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
  $pane_values['credit_card']['exp_month'] = sprintf("%02d", $pane_values['credit_card']['exp_month']);

  // Add fundraiser commerce data to the pane values array
  _fundraiser_commerce_submit_handler_pane_values($pane_values, $donation);

  // Execute the authnet submit handler
  $result = commerce_authnet_aim_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }

  // Perform post processing functions
  _fundraiser_commerce_charge_submit_form_process($success, $method_instance, $pane_values, $donation);

  return $success;
}

/**
 * Card on file callback
 *
 * We use a custom callback since we may need to update an existing CIM profile
 */
function commerce_authnet_fundraiser_commerce_cardonfile($donation) {
  // Use standard function to setup the card on file fields
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $cardonfile_fields = _fundraiser_commerce_credit_card_cardonfile_fields($donation);

  // Authorize.net does not check the exp date when checking for duplicate profiles
  // So we need to search locally to see if we have an existing profile for a card number
  // Only perform this when we're in an active non-reference charge
  if (isset($donation->reference_charge) && $donation->reference_charge === FALSE) {
    // Query the db for an existing record
    $card_data = _fundraiser_commerce_cardonfile_exp_date_mismatch($cardonfile_fields);

    // If we have a profile for this user update it in the CIM
    if ($card_data) {
      // Make the update request to authorize.net
      commerce_authnet_fundraiser_commerce_update($donation, $card_data);
    }
  }

  return $cardonfile_fields;
}

/**
 * Callback function, update card data stored at the gateway and locally
 *
 * @param $donation Object
 *    Fundraiser commerce donation object which should include the new/updated card data
 * @param $card_data Object
 *    Card on file entity, this should be the current entity for the donation,
 *    new data should be passed in the donation object
 *
 * @return Object
 *    Card on file entity for the new/updated card data
 */
function commerce_authnet_fundraiser_commerce_update($donation, $card_data = NULL) {
  module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $payment_method = $donation->gateway['gateway_details'];

  // If no card data was passed load the profile from the donation if its set
  if (empty($card_data) && !empty($donation->data['cardonfile'])) {
    $card_data = commerce_cardonfile_load($donation->data['cardonfile']);
  }

  if (empty($card_data)) {
    // Log the missing card data
    watchdog(
      'fundraiser',
      'A card update was attempted on donation @id but no stored card data was found.',
      array('@id' => $donation->did),
      WATCHDOG_CRITICAL
    );

    return;
  }

  // Pull the new credit card values from the donation
  $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);

  // Load the card on file values for this donation
  $cardonfile_fields = _fundraiser_commerce_credit_card_cardonfile_fields($donation);

  // Check the card number to see if it matches the current card data number
  // If it does then we use the update function
  // if its a new card check if we should create a new CIM profile
  $action = 'update';
  if ($cardonfile_fields['card_number'] != $card_data->card_number) {
    // This is a new card for this donation, make sure we don't have an existing profile
    // First check with the card number and date
    if ($existing = _fundraiser_commerce_cardonfile_match($cardonfile_fields)) {
      // Set the existing card on the donation and return it
      $donation->data['cardonfile'] = $existing->card_id;
      $card_data = $existing;
    }
    // Check if we have a mismatched exp date record
    elseif ($mismatch =  _fundraiser_commerce_cardonfile_exp_date_mismatch($cardonfile_fields)) {
      $donation->data['cardonfile'] = $mismatch->card_id;
      $card_data = $mismatch;
    }
    // Nothing stored locally so create a new profile
    else {
      $action = 'new';
    }
  }

  if ($action == 'update') {
    $card_data->card_number = $cardonfile_fields['card_number'];
    $card_data->card_exp_month = $cardonfile_fields['card_exp_month'];
    $card_data->card_exp_year = $cardonfile_fields['card_exp_year'];

    // We're mirroring the commerce_authnet_cim_cardonfile_update function so setup values that function uses, see that function for details
    $form['credit_card']['number']['#default_value'] = '';
    $form_state['values']['credit_card']['number'] = $pane_values['credit_card']['number'];
    $form_state['values']['credit_card']['owner'] = isset($donation->donation['first_name']) ? $donation->donation['first_name'] : '';
    $form_state['values']['credit_card']['owner'] .= isset($donation->donation['last_name']) ? ' ' . $donation->donation['last_name'] : '';
    $form_state['values']['credit_card']['address'] = isset($donation->donation['address']) ? $donation->donation['address'] : '';
    $form_state['values']['credit_card']['address'] .= isset($donation->donation['address_line_2']) ? ' ' . $donation->donation['address_line_2'] : '';
    $form_state['values']['credit_card']['city'] = isset($donation->donation['city']) ? $donation->donation['city'] : '';
    $form_state['values']['credit_card']['state'] = isset($donation->donation['state']) ? $donation->donation['state'] : '';
    $form_state['values']['credit_card']['zip'] = isset($donation->donation['zip']) ? $donation->donation['zip'] : '';
    $form_state['values']['credit_card']['country'] = isset($donation->donation['country']) ? $donation->donation['country'] : '';

    $response = _commerce_authnet_fundraiser_commerce_authnet_cim_cardonfile_update($form, $form_state, $payment_method, $card_data);

    if ($response) {
      commerce_cardonfile_save($card_data);
      return $card_data;
    }
  }
  // Create a new card on file record
  elseif ($action == 'new') {
    // We're using the built in function that requires a payment details array and the order
    $payment_details = array(
      'cardNumber' => $pane_values['credit_card']['number'],
      'expirationDate' => $pane_values['credit_card']['exp_year'] . '-' . sprintf("%02d", $pane_values['credit_card']['exp_month']),
    );

    if (isset($pane_values['credit_card']['code'])) {
      $payment_details['cardCode'] = $pane_values['credit_card']['code'];
    }

    // Load the order
    $order = commerce_order_load($donation->did);
    $response = commerce_authnet_cim_create_customer_profile_request($payment_method, $order, $payment_details);

    // If the Customer Profile creation was a success, store the new card on file data locally.
    if ($response && (string) $response->messages->resultCode == 'Ok') {
      // Build a remote ID that includes the Customer Profile ID and the
      // Payment Profile ID.
      $remote_id = (string) $response->customerProfileId . '|' . (string) $response->customerPaymentProfileIdList->numericString;

      // Get the card type
      $card_type = commerce_payment_validate_credit_card_type($pane_values['credit_card']['number'], array_keys(commerce_payment_credit_card_types()));
      $all_types = commerce_payment_credit_card_types();
      $card_type = !empty($all_types[$card_type]) ? $all_types[$card_type] : 'card';

      // Create a new cardonfile entity
      $new_data = commerce_cardonfile_new();
      $new_data->uid = $order->uid;
      $new_data->payment_method = $payment_method['method_id'];
      $new_data->instance_id = $payment_method['instance_id'];
      $new_data->remote_id = $remote_id;
      $new_data->card_type = $card_type;
      $new_data->card_name = !empty($card_data->card_name) ? $card_data->card_name : '';
      $new_data->card_number = $cardonfile_fields['card_number'];
      $new_data->card_exp_month = $cardonfile_fields['card_exp_month'];
      $new_data->card_exp_year = $cardonfile_fields['card_exp_year'];
      $new_data->status = 1;

      // Save and log the creation of the new card on file.
      commerce_cardonfile_save($new_data);
      // Set the value on the donation
      $donation->data['cardonfile'] = $new_data->card_id;
      watchdog('commerce_authnet', 'CIM Customer Profile @profile_id created and saved to user @uid.', array('@profile_id' => (string) $response->customerProfileId, '@uid' => $order->uid));
      return $new_data;
    }
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
  list($cim_customer_profile_id, $cim_payment_profile_id) = explode('|', $card_data->remote_id);

  if ($form_state['values']['credit_card']['number'] != $form['credit_card']['number']['#default_value']) {
    $number = $form_state['values']['credit_card']['number'];
  }
  else {
    $number = 'XXXX' . $card_data->card_number;
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
          'expirationDate' => $card_data->card_exp_year . '-' . sprintf("%02d", $card_data->card_exp_month),
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

function commerce_authnet_form_rules_ui_edit_element_alter(&$form, &$form_state) {
  if (isset($form['parameter']['payment_method']['settings']['payment_method']['method_id']['#value'])) {
    $id = isset($form['parameter']['payment_method']['settings']['payment_method']['method_id']['#value']);
    if ($id == 'authnet_aim') {
      $rule = rules_config_load('commerce_payment_authnet_aim');
      //if someone were to
      foreach ($rule->actions() as $action) {
        if($action->settings['payment_method']['method_id'] == 'authnet_aim') {
         $settings[] = $action->settings;
        }
      }
      $config = $settings[0]['payment_method']['settings'];
      $form['parameter']['payment_method']['settings']['payment_method']['settings']['value_map'] = array(
          '#type' => 'fieldset',
          '#title' => t('Submission mapping'),
          '#description' => t('If it is required that donor employer and occupation information be recorded with your payment provider, enter the form keys of the webform fields you will create, or have created.'),
          '#tree' => TRUE,
        );
      $available_fields = _commerce_authnet_extra_fields();
      foreach ($available_fields as $field) {
        $form['parameter']['payment_method']['settings']['payment_method']['settings']['commerce_authnet_webform_field_' . $field] = array(
          '#title' => t('Enter webform field key to map to Authorize.net field %i', array('%i' => $field)),
          '#type' => 'textfield',
          '#description' => t('Example: field_name'),
          '#default_value' => !empty($config['commerce_authnet_webform_field_' . $field]) ? $config['commerce_authnet_webform_field_' . $field] : '',
        );
      }
    }
  }
}

function fundraiser_commerce_commerce_authnet_aim_request_alter(&$nvp) {
  $donation = fundraiser_donation_get_donation($nvp['x_invoice_num']);
  $config = $donation->gateway['gateway_details']['settings'];
  if (isset($donation->submission_data[$config['commerce_authnet_webform_field_x_employer']])) {
    $nvp += array(
      'x_employer' => $donation->submission_data[$config['commerce_authnet_webform_field_x_employer']],
    );
  }

  if (isset($donation->submission_data[$config['commerce_authnet_webform_field_x_occupation']])) {
     $nvp += array(
      'x_occupation' => $donation->submission_data[$config['commerce_authnet_webform_field_x_occupation']],
    );
  }
}

function _commerce_authnet_extra_fields() {
  $extra_fields = array(
    'x_employer',
    'x_occupation',
  );
  return $extra_fields;
}
