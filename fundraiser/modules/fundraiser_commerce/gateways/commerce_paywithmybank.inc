<?php

/**
 * @file
 * Commerce based hook for commerce_paywithmybank.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info()
 */
function commerce_paywithmybank_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('commerce_paywithmybank_bank' => 'bank account'),
    'allow_recurring' => array('bank account'),
    'allow_refund' => array('bank account'),
    'offsite_processing' => array('bank account'),
    'form callback' => 'commerce_paywithmybank_fundraiser_commerce_form',
    'redirect callback' => 'commerce_paywithmybank_fundraiser_commerce_redirect',
    'refund callback' => 'commerce_paywithmybank_fundraiser_commerce_refund',
    'update callback' => 'commerce_paywithmybank_fundraiser_commerce_update',
    'cardonfile callback' => 'commerce_paywithmybank_fundraiser_commerce_cardonfile',
    'expire callback' => 'commerce_paywithmybank_fundraiser_commerce_expire',
    'charge callback' => 'commerce_paywithmybank_fundraiser_commerce_charge',
    'response save callback' => 'commerce_paywithmybank_fundraiser_commerce_response_save',
  );
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_paywithmybank_fundraiser_commerce_form($payment_method) {
  $form = array();

  $method_instance = commerce_payment_method_instance_load('commerce_paywithmybank_bank|commerce_payment_commerce_paywithmybank_bank');
  $access_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_id'] : NULL;

  $api = commerce_paywithmybank_get_api($method_instance);
  $api_endpoint = $api->getEndpoint();

  $form['#attached'] = array(
    'js' => array(
      array(
        'data' => $api_endpoint . '/start/scripts/paywithmybank.js?accessId=' . $access_id,
        'type' => 'external',
      ),
    ),
    'css' => array(
      array(
        'data' => $api_endpoint . '/start/styles/paywithmybank.css?accessId=' . $access_id,
        'type' => 'external',
      ),
    ),
  );

  return $form;
}

/**
 * Callback function, handle the redirect form.
 */
function commerce_paywithmybank_fundraiser_commerce_redirect($method_instance, $donation, $order, $settings) {
  global $base_url;

  $merchant_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_merchant_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_merchant_id'] : NULL;
  $access_id = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_id'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_id'] : NULL;
  $access_key = (isset($method_instance['settings']['commerce_paywithmybank_settings_access_key'])) ? $method_instance['settings']['commerce_paywithmybank_settings_access_key'] : NULL;

  $api = commerce_paywithmybank_get_api($method_instance);
  $api_endpoint = $api->getEndpoint();

  $order_total = $order->commerce_order_total[LANGUAGE_NONE][0];
  $order_amount = substr(commerce_currency_format($order_total['amount'], $order_total['currency_code']), 1);

  // Set up standard payment parameters.
  $payment_parameters = array(
    'accessId' => $access_id,
    'merchantId' => $merchant_id,
    'description' => 'Order from' . variable_get('site_name'),
    'currency' => $order_total['currency_code'],
    'amount' => $order_amount,
    'merchantReference' => $order->order_id,
    'paymentType' => NULL,
  );

  // Set up payment parameters for recurring payments.
  if (isset($donation->submission_data['recurs_monthly']) && !empty($donation->submission_data['recurs_monthly'])) {
    // Start / end dates must be UNIX timestamps in milliseconds.
    $start_time = round(microtime(true) * 1000);

    $recurrence = new stdClass();
    $recurrence->startDate = $start_time;
    $recurrence->endDate = NULL;
    $recurrence->frequency = NULL;
    $recurrence->frequencyUnit = NULL;
    //$recurrence->frequencyUnitType = PayWithMyBank::NULL_RECURRENCE_FREQUENCY_UNIT_TYPE;
    $recurrence->frequencyUnitType = NULL;
    $recurrence->recurringAmount = NULL;
    $recurrence->automaticCapture = 'false';

    $payment_parameters['paymentType'] = 'Recurring';
    $payment_parameters['recurrence'] = $recurrence;

    // Do not make an initial charge on a recurring donation.
    $payment_parameters['amount'] = '0.00';
  }

  $request_signature = commerce_paywithmybank_get_request_signature($access_key, $payment_parameters);

  $establish_data = $payment_parameters;
  $establish_data['returnUrl'] = $settings['return'];
  $establish_data['cancelUrl'] = $base_url . '/springboard/node/' . $donation->nid . '/';
  $establish_data['requestSignature'] = $request_signature;

  $form = array();

  $form['#attached'] = array(
    'js' => array(
      array(
        'data' => $api_endpoint . '/start/scripts/paywithmybank.js?accessId=' . $access_id,
        'type' => 'external',
      ),
      array(
        'data' => 'jQuery(document).ready(function(){ PayWithMyBank.establish(' . json_encode($establish_data) . '); });',
        'type' => 'inline',
      ),
    ),
    'css' => array(
      array(
        'data' => $api_endpoint . '/start/styles/paywithmybank.css?accessId=' . $access_id,
        'type' => 'external',
      ),
    ),
  );

  return $form;
}

/**
 * Callback function, submit after redirect.
 */
function commerce_paywithmybank_fundraiser_commerce_redirect_submit($method_instance, $donation, $order) {
  $transaction_id = filter_input(INPUT_GET, 'transactionId');
  $request_signature = filter_input(INPUT_GET, 'requestSignature');

  $valid_request = commerce_paywithmybank_validate_request($transaction_id, $request_signature);

  if ($valid_request) {
    // This is an authorization, so set auth_txn_id.
    $donation->auth_txn_id = $transaction_id;

    $api = commerce_paywithmybank_get_api($method_instance);
    $api_transaction = $api->getTransaction($transaction_id);

    watchdog('commerce_paywithmybank', 'PayWithMyBank redirect submit with transaction: !param', array('!param' => '<pre>' . check_plain(print_r($api_transaction, TRUE)) . '</pre>'), WATCHDOG_DEBUG);

    $success = (($api_transaction->status == PayWithMyBank::TRANSACTION_STATUS_ON_HOLD) || ($api_transaction->status == PayWithMyBank::TRANSACTION_STATUS_AUTHORIZED));

    $pane_values = array();

    if ($success) {
      $pane_values = commerce_paywithmybank_pane_values($donation);

      if (!empty($donation->submission_data['recurs_monthly'])) {
        $donation->donation['recurs_monthly'] = TRUE;

        // Donation is recurring, so create a card on file record.
        $pane_values['bank_account']['cardonfile_store'] = TRUE;

        // Need to manually create card on file data for this donation here.
        commerce_paywithmybank_fundraiser_commerce_update($donation);

        // Need to manually process this donation as successful here so the
        // recurring donation set exists prior to processing the first payment.
        fundraiser_donation_success($donation);

        if ($api_transaction->status == PayWithMyBank::TRANSACTION_STATUS_ON_HOLD) {
          // The transaction has been put into an "on hold" status and must
          // be verified before we can continue.
          $verified_api_transaction = $api->setVerificationStatus($api_transaction->transactionId, $donation->did, TRUE);
          watchdog('commerce_paywithmybank', 'PayWithMyBank verified transaction: !param', array('!param' => '<pre>' . check_plain(print_r($verified_api_transaction, TRUE)) . '</pre>'), WATCHDOG_DEBUG);

          if ($verified_api_transaction->status == PayWithMyBank::TRANSACTION_STATUS_AUTHORIZED) {
            // Transaction has been authorized, so an initial payment can be made.
            commerce_paywithmybank_fundraiser_commerce_charge($method_instance, $donation);
          }
        }
        else if ($api_transaction->status == PayWithMyBank::TRANSACTION_STATUS_AUTHORIZED) {
          // Transaction has been authorized, so an initial payment can be made.
          commerce_paywithmybank_fundraiser_commerce_charge($method_instance, $donation);
        }
      }
    }

    // Save updated donation.
    fundraiser_donation_update($donation);

    // TODO: Review this - may be a work-around that should be fixed.
    $donation->donation['payment_method'] = 'bank account';

    _fundraiser_commerce_charge_submit_form_process($success, $method_instance, $pane_values, $donation, 'bank_account');

    return $success;
  }
  else {
    watchdog('commerce_paywithmybank', 'Redirect form submit failed due to validation error.', array(), WATCHDOG_ERROR);
  }

  return FALSE;
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
 * @return object|NULL
 *   Card on file entity for the new/updated card data.
 *   NULL if no card data was found.
 */
function commerce_paywithmybank_fundraiser_commerce_update($donation, $card_data = NULL) {
  $payment_method = $donation->gateway['gateway_details'];

  // If no card data was passed load the profile from the donation if it's set.
  if (empty($card_data) && !empty($donation->data['cardonfile'])) {
    $card_data = commerce_cardonfile_load($donation->data['cardonfile']);
  }

  // Load the card on file values for this donation.
  $cardonfile_fields = commerce_paywithmybank_cardonfile_fields($donation);

  // Check the account number to see if it matches the current account number.
  $action = 'update';
  if (empty($card_data)) {
    $action = 'new';
  }
  elseif ($cardonfile_fields['card_number'] != $card_data->card_number) {
    // This is a new card for this donation,
    // Check for an existing card on file profile.
    $existing = _fundraiser_commerce_cardonfile_match($cardonfile_fields);
    if (!empty($existing)) {
      // Set the existing card on the donation and return it.
      $donation->data['cardonfile'] = $existing->card_id;
      $card_data = $existing;
    }
    // Nothing stored locally so create a new profile.
    else {
      $action = 'new';
    }
  }

  // Get card expiration date.
  $expires = commerce_paywithmybank_fundraiser_commerce_expire(array());

  if ($action == 'update') {
    $card_data->card_number = $cardonfile_fields['card_number'];
    //$card_data->card_exp_month = $expires['month'];
    //$card_data->card_exp_year = $expires['year'];
    commerce_cardonfile_save($card_data);
    return $card_data;
  }
  // Create a new card on file record.
  elseif ($action == 'new') {
    $card_name = '';
    $order = commerce_order_load($donation->did);
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

    if ($order_wrapper->commerce_customer_billing->value()) {
      $billing_address = $order_wrapper->commerce_customer_billing->commerce_customer_address->value();
      $card_name = $billing_address['name_line'];
    }

    // The remote ID should always be the authorization transaction ID,
    // as this is the transaction that represents the recurring payment.
    if (!empty($donation->auth_txn_id)) {
      $card_data = commerce_cardonfile_new();
      $card_data->uid = $order->uid;
      $card_data->payment_method = $payment_method['method_id'];
      $card_data->instance_id = $payment_method['instance_id'];
      $card_data->remote_id = $donation->auth_txn_id;
      $card_data->card_type = '';
      $card_data->card_name = $card_name;
      $card_data->card_number = $cardonfile_fields['card_number'];
      $card_data->card_exp_month = $expires['month'];
      $card_data->card_exp_year = $expires['year'];
      $card_data->status = 1;

      // Save and log the creation of the new card on file.
      commerce_cardonfile_save($card_data);
      watchdog('commerce_paywithmybank', 'Card on file registered and saved to user @uid.', array('@uid' => $order->uid));

      // Set the value on the donation.
      $donation->data['cardonfile'] = $card_data->card_id;
      return $card_data;
    }
  }
}

/**
 * Card on file callback.
 */
function commerce_paywithmybank_fundraiser_commerce_cardonfile($donation) {
  commerce_paywithmybank_fundraiser_commerce_update($donation);

  $cardonfile_fields = commerce_paywithmybank_cardonfile_fields($donation);
  return $cardonfile_fields;
}

/**
 * Expiration date callback.
 */
function commerce_paywithmybank_fundraiser_commerce_expire($submission_fields) {
  $expires = array();

  if (isset($submission_fields['payment_fields']['bank account'])) {
    $expires['month'] = $submission_fields['payment_fields']['bank account']['card_expiration_month'];
    $expires['year'] = $submission_fields['payment_fields']['bank account']['card_expiration_year'];
  }
  else {
    // One future recurring payment should be created, so set expiration time
    // to one month ahead of current time.
    //$expire_time = time() + 2592000;
    $expire_time = time();

    $expires['month'] = date('m', $expire_time);
    $expires['year'] = date('Y', $expire_time);
  }

  return $expires;
}

/**
 * Callback function, charge the gateway.
 *
 * This function should only ever be used to charge recurring payments.
 * Single payments are handled entirely in:
 * commerce_paywithmybank_fundraiser_commerce_redirect_submit()
 */
function commerce_paywithmybank_fundraiser_commerce_charge($method_instance, $donation) {
  // Translate donation settings to the correct values for the commerce gateway.
  $order = commerce_order_load($donation->did);
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $order_wrapper->commerce_order_total->value();
  $amount = substr(commerce_currency_format($charge['amount'], $charge['currency_code']), 1);

  $recurring_donation = _fundraiser_sustainers_get_recurring_by_did($donation->did);
  $master_donation = fundraiser_donation_get_donation($recurring_donation->master_did);

  watchdog('commerce_paywithmybank', 'Capturing transaction for did @did, master_did @master_did, auth_txn_id @auth_txn_id.', array(
    '@did' => $donation->did,
    '@master_did' => $recurring_donation->master_did,
    '@auth_txn_id' => $master_donation->auth_txn_id,
    )
  );

  // Process the transaction.
  $api = commerce_paywithmybank_get_api($method_instance);
  // Recurring donations are captured using the master donation's authorization
  // transaction ID.
  $api_transaction = $api->captureTransaction($master_donation->auth_txn_id, $order->order_id, $amount);

  // Log the transaction.
  watchdog('commerce_paywithmybank', 'PayWithMyBank capture transaction: !param', array('!param' => '<pre>' . check_plain(print_r($api_transaction, TRUE)) . '</pre>'), WATCHDOG_DEBUG);

  if (!empty($method_instance['settings']['commerce_paywithmybank_settings_log']['request'])) {
    commerce_paywithmybank_log_request('capture', $master_donation->auth_txn_id, $order->order_id, $amount);
  }

  $errors = $api->getLastErrors();
  $success = (empty($errors));

  if ($success) {
    // Save updated donation.
    watchdog('commerce_paywithmybank', 'Updating transaction ID for did @did to @txn_id.', array(
      '@did' => $donation->did,
      '@txn_id' => $api_transaction->transactionId,
      )
    );

    if ($master_donation->did == $donation->did) {
      $master_donation->txn_id = $api_transaction->transactionId;
      fundraiser_donation_update($master_donation);
    }
    else {
      $donation->txn_id = $api_transaction->transactionId;
      fundraiser_donation_update($donation);
    }
  }
  else {
    commerce_paywithmybank_log_errors($master_donation->auth_txn_id, $errors);
  }

  // Update expiration date and generate the next donation.
  $cards_on_file = commerce_cardonfile_load_multiple_by_remote_id($master_donation->auth_txn_id);

  if (!empty($cards_on_file)) {
    $card_data = current($cards_on_file);

    // Move card expiration date ahead one month.
    $new_expire_time = mktime(0, 0, 0, ($card_data->card_exp_month + 1), 1, $card_data->card_exp_year);

    $card_data->card_exp_month = date('m', $new_expire_time);
    $card_data->card_exp_year = date('Y', $new_expire_time);

    // Update card using master donation so transaction ID is available.
    commerce_paywithmybank_fundraiser_commerce_update($master_donation, $card_data);

    $submission_fields = array(
      'payment_fields' => array(
        'bank account' => array(
          'card_expiration_month' => $card_data->card_exp_month,
          'card_expiration_year' => $card_data->card_exp_year,
        ),
      ),
    );

    fundraiser_sustainers_update_billing_info_create_new_donations($master_donation, $donation, $submission_fields);
  }

  return $success;
}

/**
 * Callback function, refund the gateway.
 */
function commerce_paywithmybank_fundraiser_commerce_refund($payment_method, $refund) {
  // Process the refund.
  $api = commerce_paywithmybank_get_api($payment_method);
  $amount = commerce_currency_decimal_to_amount($refund->amount, $refund->currency);
  $formatted_amount = substr(commerce_currency_format($amount, $refund->currency), 1);
  $transaction = $api->refundTransaction($refund->donation->txn_id, $refund->did, $formatted_amount);

  // Log the refund.
  if ($payment_method['settings']['commerce_paywithmybank_settings_log']['request']) {
    commerce_paywithmybank_log_request('refund', $refund->donation->txn_id, $refund->did, $formatted_amount);
  }

  if ($transaction == NULL) {
    commerce_paywithmybank_log_errors($refund->donation->txn_id, $api->getLastErrors());
    return FALSE;
  }

  return TRUE;
}

/**
 * Callback function, process donation post-save.
 */
function commerce_paywithmybank_fundraiser_commerce_response_save($donation) {
  /**
   * Fundraiser automatically applies a "payment received" status to successful
   * donations. PayWithMyBank donations sit in an "authorized" status before
   * the donor is charged, so the status must be overridden here.
   */
  // Get the most recent transaction attached to the donation.
  $result = db_select('commerce_payment_transaction')
    ->fields('commerce_payment_transaction', array('message'))
    ->condition('order_id', $donation->did, '=')
    ->orderBy('transaction_id', 'DESC')
    ->range(0, 1)
    ->execute()
    ->fetchAssoc();

  $new_status = NULL;

  if (!empty($result)) {
    switch ($result['message']) {
      case 'New':
      case 'Pending':
      case 'OnHold':
        $new_status = 'pending_future_payment';
        break;
      case 'Authorized':
        $new_status = 'authorized';
        break;
    }
  }
  else {
    // If a charge occurs and there are no transactions then a notification
    // is pending. The donation should remain pending.
    $new_status = 'pending_future_payment';
  }

  if ($new_status != NULL) {
    _fundraiser_commerce_update_order_status($new_status, $donation->did);

    $donation->status = $new_status;
    $donation->status_label = _fundraiser_commerce_get_label_by_status($new_status);
    $donation->status_charged = _fundraiser_commerce_get_charged_by_status($new_status);

    fundraiser_donation_update($donation);
  }
}

/**
 * Get PayWithyMyBank pane values from a donation.
 *
 * @param object $donation
 *   The donation to grab the pane values from.
 *
 * @return array
 *   The pane values.
 */
function commerce_paywithmybank_pane_values($donation) {
  $pane_values = array();

  $method_instance = commerce_payment_method_instance_load($donation->gateway['id']);

  $transaction_id = (!empty($donation->auth_txn_id)) ? $donation->auth_txn_id : $donation->txn_id;

  if (!empty($transaction_id)) {
    $api = commerce_paywithmybank_get_api($method_instance);
    $api_transaction = $api->getTransaction($transaction_id);

    $errors = $api->getLastErrors();
    if (!empty($errors)) {
      commerce_paywithmybank_log_errors($transaction_id, $errors);
    }

    // Only one valid payment method.
    $pane_values['bank_account'] = array(
      'account_number' => $api_transaction->payment->account->accountNumber,
      'account_name' => $api_transaction->payment->account->name,
    );
  }

  return $pane_values;
}

/**
 * Returns the values need to load a card on file profile for PayWithMyBank.
 *
 * @param object $donation
 *   Fundraiser donation object.
 *
 * @return array
 *   Values ready to be passed into a card on file profile query.
 */
function commerce_paywithmybank_cardonfile_fields($donation) {
  $pane_values = commerce_paywithmybank_pane_values($donation);

  $data = array(
    'card_number' => $pane_values['bank_account']['account_number'],
  );

  return $data + array('instance_id' => $donation->gateway['id'], 'uid' => $donation->uid);
}
