<?php

/**
 * @file import queue admin pages, queue processor, and associated helper functions.
 */


/**
 * Queue admin page.
 */
function sustainer_import_queue() {
  // queue report
  $form['status'] = array(
    '#type' => 'markup',
    '#markup' => sustainer_import_queue_report(),
  );
  // run queue button
  $form['process_queue'] = array(
    '#type' => 'button',
    '#value' => t('Process pending items'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('sustainer_import_process_queue'),
  );
  $form['gateway_map'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment gateway type map'),
    '#description' => t('Map import gateway types to existing payment gateways and donation forms. Each gateway type must be mapped to a payment gateway before the queue can be processed.'),
  );
  $gateway_types = sustainer_import_queue_gateway_types();
  foreach ($gateway_types as $gateway_type) {
    $defaults = variable_get('sustainer_import_gateway_map_' . $gateway_type, array());
    $form['gateway_map'][$gateway_type] = array(
      '#type' => 'fieldset',
      '#title' => t('Map ') . $gateway_type,
    );
      $form['gateway_map'][$gateway_type][$gateway_type . '_id'] = array(
      '#type' => 'select',
      '#title' => t('Select a payment gateway for this type'),
      '#options' => array_merge(array('0' => '- select -'), sustainer_import_list_payment_gateways()),
      '#default_value' => !empty($defaults['gateway_id']) ? $defaults['gateway_id'] : 0,
    );
    $form['gateway_map'][$gateway_type][$gateway_type . '_nid'] = array(
      '#type' => 'select',
      '#title' => t('Select a donation form for this type'),
      '#options' => array_merge(array('0' => t('- select -')), _sustainer_import_list_donation_forms()),
      "#default_value" => !empty($defaults['nid']) ? $defaults['nid'] : 0,
    );
  }
  $form['gateway_map']['save_map'] = array(
    '#type' => 'button',
    '#value' => t('Save gateway map'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('sustainer_import_save_gateway_map'),
  );
  return $form;
}

function sustainer_import_queue_report() {
  // list items in queue
  $status_rows = array();
  $queue_status_counts = sustainer_import_queue_status_counts();
  foreach ($queue_status_counts as $status_count) {
    $actions = '';
    // apply pending label to items with empty status
    if (!$status_count['status']) {
      $status_count['status'] = 'pending';
    }
    if ($status_count['status'] == 'failed') {
      $actions = l('failure report', 'admin/config/development/sustainer_import/queue_failure_report');
    }
    $status_rows[] = array('data' => array($status_count['status'], $status_count['count'], $actions));
  }
  $headers = array(
    array('data' => array('status')),
    array('data' => array('count')),
    array('data' => array('actions')),

  );
  $table = theme('table', array('header' =>$headers, 'rows' =>$status_rows));
 // $pending_queue = sustainer_import_get_pending_queue_items();

  // generate report table
  return $table;
}

/**
 * Primary queue processing callback. This function triggers validation & creation of
 * the following:
 *
 * - associated user account
 * - commerce master order
 * - fundraiser master donation record
 * - cardonfile payment method record
 * - fundraiser sustainer series records
 * - commerce orders for each sustainer series instance
 */
function sustainer_import_process_queue() {
  // get pending queue items
  $queue = sustainer_import_get_pending_queue_items();
  // parse item
  foreach ($queue as $rid => $record) {
    // user record
    // validate
    $errors = array();
    if (_sustainer_import_validate_account_record($record['user_info'], $errors)) {
      // passes validation, create or dedupe.
      $account = _sustainer_import_create_account($record['user_info']);
      $record['uid'] = $account->uid;
    }
    // master order record
    if (!empty($record['uid'])) {
      if(_sustainer_import_validate_master_order($record, $errors)) {
        $donation = _sustainer_import_create_master_order($record, $account, $errors);
      }
    }

    // create
    // sustainer series
    if (!isset($donation->did) && $donation->did && $record['uid']) {
      foreach ($record['payment_schedule'] as $instance_details) {
        if (_sustainer_import_validate_sustainer_series_instance($instance_details, $errors)) {
          _sustainer_import_create_sustainer_series_instance($instance_details, $donation, $errors);
        }
        else {
          // we need to bail out of processing sustainer instances if an error is encountered.
          break;
        }
      }
    }
    $log = sustainer_import_log_value();
    sustainer_import_log_record($rid, $record, $log, $errors);
  }
}



/**
 * Save queue gateway type to payment gatewway mapping.
 */
function sustainer_import_save_gateway_map($form, $form_state) {
  $gateway_types = sustainer_import_queue_gateway_types();

  foreach ($gateway_types as $gateway_type) {
    $settings['gateway_id'] = $form_state['values'][$gateway_type . '_id'];
    $settings['nid'] = $form_state['values'][$gateway_type . '_nid'];

    variable_set('sustainer_import_gateway_map_' . $gateway_type, $settings);
  }
}

/**
 * Creates a user account (including uid to salesforce id mapping) from import
 * record details. Dedupes on email address.
 *
 * @param $account_details
 * An associative array of import record account details.
 * Required values:
 * - email: account email address
 * - sf_account_id (optional): Salesforce ID of the Account record associated with this user.
 * - sf_contact_id (optional): Salesforce ID of the Contact record associated with this user.
 *
 * @return bool|mixed|object|stdClass
 * returns a user oject for the account created, or the existing user object if
 * an account already exists with the specified email address.
 */
function _sustainer_import_create_account($account_details) {
  // create user account or return existing account if mail exists
  $account = user_load_by_mail($account_details['email']);
  if (!$account) {
    // save user
    $user_record = array(
      'name' => _sustainer_import_name_from_mail($account_details['email']),
      'pass' => '',
      'mail' => $account_details['email'],
      'status' => 1,
    );

    $account = user_save((object) $user_record);
    sustainer_import_log_value('uid', $account->uid, 'Created new user account.');
    // if we have SF id's for the account, save them now.
    if ($account_details['sf_contact_id']) {
      _sustainer_import_save_account_sfids($account->uid, $account_details['sf_contact_id'], 'Contact');
    }
    if ($account_details['sf_account_id']) {
      _sustainer_import_save_account_sfids($account->uid, $account_details['sf_account_id'], 'Account');
    }
  }
  else {
    sustainer_import_log_value('uid', $account->uid, 'User account already existed.');
  }
  return $account;
}

/**
 * Validate user account details. Confirms required fields have values and
 * validates the account email via filter_var().
 *
 * @param $account_details
 * Associative array containing user account information.
 * Required fields:
 *  - email
 *  - sf_contact_id
 *  - sf_account_id
 *
 * Note: while the user_info portion of an import record should contain first and last name,
 * this information isn't required to create an account so it is validated elsewhere.
 *
 * @return bool
 * Returns TRUE if account details pass validation, otherwise FALSE.
 */
function _sustainer_import_validate_account_record($account_details, &$errors) {
  // confirm all required fields have data
  $local_errors = array();

  if (empty($account_details['email'])) {
    $local_errors[] = t('Missing email address in user record');
  }
  // confirm email address is valid
  if (!filter_var($account_details['email'], FILTER_VALIDATE_EMAIL)) {
    $local_errors[] = t('Invalid email address: !address', array('!address' => $account_details['email']));
  }

  if (count($local_errors)) {
    $errors = array_merge($errors, $local_errors);
    return FALSE;
  }

  return TRUE;
}

/**
 * Validate record details required by Commerce and Fundraiser to create a sustainer series
 * master order.
 *
 * @param $record
 * Associative array of data required to create a sustainer series mastervariable_set('sustainer_import_gateway_map_' . $gateway_type, $settings); order in commerce and
 * Fundraiser.
 *
 * Required fields:
 * - uid: Drupal user id associated with this donation.
 * - amount (must be numeric) : donation amount
 * - salesforce_opportunity_id : Salesforce ID of the Opportunity record associated with this order.
 * - salesforce_recurring_donation_id: Salesforce ID of the Recurring Donation record associated with this order.
 *
 * - user_info
 *   - first name : user first name
 *   - last name : user last name
 *
 * - billing_info
 *   - address_1 : billing address
 *   - city : billing city
 *   - state : billing state
 *   - zip : billing postal code
 *   - country : billing country
 *
 * - payment_details
 *   - gateway_token : payment gateway reference token, required to process future charges.
 *   - type : card type
 *   - last four : last four digits of card number
 *   - month : card expiration month
 *   - year : card expiration year
 *
 * @param $errors
 * An array used to capture any error messages encountered while validating the master order and associated
 * records.
 *
 * @return bool
 * Returns TRUE if the import record passes validation, FALSE otherwise.
 */
function _sustainer_import_validate_master_order($record, &$errors) {
  $local_errors = array();
  // confirm all required fields have data
  $required_fields = array('uid', 'amount');
  // validate top level fields
  foreach ($required_fields as $field) {
    if (empty($record[$field])) {
      $local_errors[] = t('Missing required field for master order:!field', array('!field' => $field));
    }
    if ($field == 'amount') {
      if (!is_numeric($record['amount'])) {
        $local_errors[] = t('Donation amount is not numeric: !amount', array('!amount' => $record['amount']));
      }
    }
  }

  // hit user_info
  if (empty($record['user_info']['first_name'])) {
    $local_errors[] = t('Missing first_name in user info');
  }
  if (empty($record['user_info']['last_name'])) {
    $local_errors[] = t('Missing last_name in user info');
  }

  // billing info
  $required_fields = array('address_1', 'city', 'state', 'zip', 'country');
  foreach ($required_fields as $field) {
    if (empty($record['billing_info'][$field])) {
      $local_errors[] = t('Missing required billing information field:!field', array('!field' => $field));
    }
  }

  // payment_details
  $required_fields = array('gateway_token', 'gateway_type', 'type', 'last_four', 'month', 'year');
  foreach ($required_fields as $field) {
    if (empty($record['payment_details'][$field])) {
      $local_errors[] = t('Missing required payment details field:!field', array('!field' => $field));
    }
    if ($field == 'last_four' && !is_numeric($record['payment_details']['last_four'])) {
      $local_errors[] = t('Last four are not numeric:!last_four', array('!last_four' => $record['payment_details']['last_four']));
    }
  }

  if (count($local_errors)) {
    $errors = array_merge($errors, $local_errors);
    return FALSE;
  }

  return TRUE;
}

/**
 * @param $record
 * @param $account
 * @param $errors
 *
 * @return stdClass
 */
function _sustainer_import_create_master_order($record, $account, &$errors) {
  // create master order
  $gateway_map = variable_get('sustainer_import_gateway_map_' . $record['payment_details']['gateway_type'], array());

  if (!empty($gateway_map['nid']) && is_numeric($gateway_map['nid'])) {
    $node = node_load($gateway_map['nid']);
  }
  else {
    // log map error and bail
    $errors[] = t('Unable to load node related to the payment gateway type: !type. Either this gateway type has not been mapped or the mapped node has been deleted.');
    return FALSE;
  }

  $donation = _sustainer_import_build_donation_object($record, $account, $node);

  // Before we can save the donation record we need to get an updated SIM from
  // Authorize.net. If for any reason we can't get this value there is no point in
  // importing the donation record as we won't be able to charge until the SIM is updated.

  $payment_method = commerce_payment_method_instance_load($donation->gateway);

  // get updated sim
  $sim = _sustainer_import_update_authnet_sim($payment_method, $record['payment_details']['gateway_token']);
  if ($sim) {
    drupal_set_message('got sim:' . $sim);
    sustainer_import_log_value('sim', $sim, 'Retrieved payment gateway token from authorize.net');
  }
  else {
    $errors[] = t('Unable to update payment gateway token. Import halted.');
    return FALSE;
  }


  fundraiser_donation_create($donation);
  sustainer_import_log_value('did', $donation->did, 'Created the fundraiser master donation');

  // create entry in fundraiser_sustainers for master order
  // write sustainer record.
  $recurring_donation = array(
    'master_did' => $donation->did,
    'did' => $donation->did,
    'next_charge' => $donation->created,
    'sustainer_key' => fundraiser_sustainers_get_sustainer_key_value(),
    'sf_opportunity_id' => $record['salesforce_opportunity_id'], //$instance_details['salesforce_opportunity_id'],
    'sf_recurring_id' => '', // not used
    'sf_sync_status' => '', // not used
    'gateway_resp' => 'success', // has to be successful otherwise no sustainer series
    'attempts' => 1,
    'cancellation_reason' => '', // Not applicable?
  );
  // create entry in {fundraiser_sustainers} for the master order.
  _fundraiser_sustainers_create_recurring($recurring_donation);

  // store cardonfile entry.
  $card_data = _sustainer_import_save_cc_data($sim, $record, $donation, $account);

  // Create a payment transaction and update the order status
  sustainer_import_create_payment_transaction($donation);
  _fundraiser_commerce_update_order_status('payment_received', $donation->did);

  // The master order is exported to Salesforce as 2 objects. We need to add the
  // sync map records here. One for the opportunity and one for the recurring
  // donation object.
  // Opportunity
  $map = array(
    'sfid' => $record['salesforce_opportunity_id'],
    'module' => 'salesforce_donation',
    'delta' => 'donation',
    'drupal_id' => $donation->did,
    'object_type' => 'Opportunity'
  );
  sustainer_import_add_sync_map($map);

  // Recurring donation
  $map['sfid'] = $record['salesforce_recurring_donation_id'];
  $map['object_type'] = 'npe03__Recurring_Donation__c';
  $map['module'] = 'fundraiser_sustainers';
  $map['delta'] = 'recurring_donation';
  sustainer_import_add_sync_map($map);

  // Update Salesforce with new order id.
  sustainer_import_update_opportunity_id($donation, $record['salesforce_opportunity_id']);

  // Maintain a map of ids.
  sustainer_import_map_ids($donation->did, $record['order_id']);

  return $donation;
}

/**
 * Creates a record to map old and new order ids.
 *
 * @param $id
 *   The new id.
 *
 * @param $legacy_id
 *   The old id.
 */
function sustainer_import_map_ids($id, $legacy_id) {
  $record = array(
    'id' => $id,
    'legacy_id' => $legacy_id,
    'created' => time(),
  );

  drupal_write_record('sustainer_import_id_map', $record);
}

/**
 * Creates a queue item to update the order id field on each opportunity.
 *
 * @param $donation
 *   The full donation object.
 *
 * @param $sfid
 *   The Salesforce id of the opportunity.
 */
function sustainer_import_update_opportunity_id($donation, $sfid) {
  $sobject = new stdClass();
  $sobject->Id = $sfid;
  $sobject->type = 'Opportunity';
  $sobject->fields = array(
    'Order_Id__c' => $donation->did,
  );

  $item = array(
    'drupal_id' => $donation->did,
    'module' => 'salesforce_donation',
    'delta' => 'donation',
    'object_type' => 'Opportunity',
    'operation' => 'UPDATE',
    'sobject' => $sobject,
  );

  $queue = salesforce_queue_load();
  $result = $queue->createItem($item);
}


function _sustainer_import_validate_sustainer_series_instance($instance_details, &$errors) {
  $local_errors = array();
  $required_fields = array('year', 'month', 'day');
  $count = 0;
  foreach ($required_fields as $field) {
    ++$count;
    if (empty($instance_details[$field])) {
      $local_errors[] = t('missing required field (!field) on sustainer instance !count', array('!field' => $field, '!count' => $count));
    }
  }
  if (count($local_errors)) {
    $errors = array_merge($errors, $local_errors);
    return FALSE;
  }
  return TRUE;
}

/**
 * Create a single sustainer series instance.
 *
 * @param $instance_details
 * @param $donation
 */
function _sustainer_import_create_sustainer_series_instance($instance_details, $donation) {
  // Stage 1: create the donation and order objects for the sustainer charge.
  $new_donation = clone $donation;
  $new_donation->sid = 0; // Not actually submitted, it's automated, so no sid.
  // save the donation object for the sustainer series instance
  fundraiser_donation_create($new_donation);
  // compile next charge timestamp from month, day, and year.
  $date = $instance_details['month'] . '/' . $instance_details['day'] . '/' . $instance_details['year'];
  $next_charge = strtotime($date);
  fundraiser_donation_comment($new_donation, 'Sustainer donation scheduled to be charged at @next_charge',
    array('@next_charge' => format_date($next_charge)));

  // write sustainer record.
  $recurring_donation = array(
    'master_did' => $donation->did,
    'did' => $new_donation->did,
    'next_charge' => $next_charge,
    'sustainer_key' => fundraiser_sustainers_get_sustainer_key_value(),
    'sf_opportunity_id' => $instance_details['salesforce_opportunity_id'],
    'sf_recurring_id' => '', // field isn't used
    'sf_sync_status' => '', // field isn't used
    'gateway_resp' => $instance_details['status'], // ???
    'attempts' => $instance_details['attempts'], // probably default to 1?
    'cancellation_reason' => '', // Not importing cancelled records
  );
  _fundraiser_sustainers_create_recurring($recurring_donation);

  // Add a payment and update commerce order status.
  if ($instance_details['status'] == 'success') {
    sustainer_import_create_payment_transaction($new_donation);
    _fundraiser_commerce_update_order_status('payment_received', $new_donation->did);
  }
  else if ($instance_details['status'] == 'failed' && $instance_details['attempts'] >= 3) {
   _fundraiser_commerce_update_order_status('failed', $new_donation->did);
  }

  // Add the donation to the sync map table
  $map = array(
    'sfid' => $instance_details['salesforce_opportunity_id'],
    'module' => 'salesforce_donation',
    'delta' => 'donation',
    'drupal_id' => $new_donation->did,
    'object_type' => 'Opportunity'
  );

  sustainer_import_add_sync_map($map);

  // Update Salesforce with new order id.
  sustainer_import_update_opportunity_id($new_donation, $instance_details['salesforce_opportunity_id']);

  // Map original sustainer instance order id to new order id.
  sustainer_import_map_ids($new_donation->did, $instance_details['order_id']);

  sustainer_import_log_value('recurring_instance', $next_charge, t('Created sustainer series instance for !date', array('!date' => $date)));
}

/**
 * Adds a payment transaction to a donation.
 *
 * @param $donation
 *   The donation to which the payment will be added.
 */
function sustainer_import_create_payment_transaction($donation) {
  $gateway_info = explode('|', $donation->gateway);
  $transaction = commerce_payment_transaction_new($gateway_info[0], $donation->did);
  $transaction->instance_id = $donation->gateway;
  $transaction->remote_id = 'UNAVAILABLE';
  $transaction->amount = $donation->donation['amount'] * 100;
  $transaction->currency_code = $donation->donation['currency'];
  $transaction->payload[REQUEST_TIME] = array();
  $transaction->remote_status = 'auth_capture';
  $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
  commerce_payment_transaction_save($transaction);
}

/**
 * Saves a salesforce sync map.
 *
 * @param $map
 *   The map to save.
 */
function sustainer_import_add_sync_map($map) {
  if (module_exists('salesforce_sync')) {
    salesforce_sync_save_map($map);
  }
  else {
    drupal_set_message("Unable to save sync map because the Salesforce Sync module is not enabled.");
  }
}

/**
 * Save salesforce id's associated with a user account.
 *
 * @param $uid
 * Drupal user id of the account
 *
 * @param $sfid
 * Salesforce ID
 *
 * @param $type
 * Salesforce record type (typically Contact or Account)
 */
function _sustainer_import_save_account_sfids($uid, $sfid, $type) {
  drupal_set_message("record save attempt. uid: $uid, sfid:$sfid, type:$type");
  $sf_map = array(
    'sfid' => $sfid,
    'drupal_id' => $uid,
    'module' => 'user',
    'delta' => 'user',
    'object_type' => $type,
  );
  sustainer_import_add_sync_map($sf_map);
  drupal_set_message('saved !type record for !uid', array('!uid' => $uid, '!type' => $type));
}

function _sustainer_import_name_from_mail($mail) {
  $parts = explode('@', $mail);
  return $parts[0];
}


/**
 * Create a commerce order and donation records,
 * update authnet profile id
 * save cardonfile record for payment details
 *
 * @param $record
 * @param $account
 * @param $node
 * @return stdClass
 */
function _sustainer_import_build_donation_object($record, $account, $node) {

  $donation = new stdClass();
  $donation->created = $record['created'];
  $donation->changed = $record['created'];
  $donation->uid = $account->uid;
  $donation->user = $account;
  $donation->mail = $account->mail;

  $donation->sid = 0; // placeholder since no webform submission is created.
  // This payment has been captured, note the commerce order needs to be updated to payment received
  $donation->status = 'payment_received';

  // Get the node from gateway map
  // TODO: implement admin form, map gateway type to existing gateway & donation form.
  //$donation->node = _fundraiser_commerce_order_node_from_line_item($line_item);

  $donation->nid = $node->nid;
  $donation->node = $node;
  $gateway_map = variable_get('sustainer_import_gateway_map_' . $record['payment_details']['gateway_type'], array());


  // TODO: throw error if gateway id is not available.
  $donation->gateway = !empty($gateway_map['gateway_id']) ? $gateway_map['gateway_id'] : '';

  $donation->donation = array();

  // Populate the necessary address values
  $donation->donation['mail'] = $account->mail;
  $donation->donation['country'] = $record['billing_info']['country'];
  $donation->donation['first_name'] = $record['user_info']['first_name'];
  $donation->donation['last_name'] = $record['user_info']['last_name'];
  $donation->donation['state'] = $record['billing_info']['state'];
  $donation->donation['address'] =  $record['billing_info']['address_1'];
  $donation->donation['address_line_2'] = $record['billing_info']['address_2'];
  $donation->donation['city'] = $record['billing_info']['city'];
  $donation->donation['zip'] = $record['billing_info']['zip'];

  // payment details were saved with the original donation
  $donation->donation['payment_method'] = 'credit'; // $data['payment_method'];
  $donation->donation['payment_fields'] = array(
    'credit' => array(
      'card_number' => $record['payment_details']['last_four'],
      'card_expiration_month' => $record['payment_details']['month'],
      'card_expiration_year' => $record['payment_details']['year'],
      'card_cvv' => '', // data unvailable due to PCI requirements
      'card_type' => $record['payment_details']['type'],
    ),
  );
  // From the line item get the donation amount
  // TODO: confirm currency code & amount location in $record.
  $donation->donation['amount'] = commerce_currency_amount_to_decimal($record['amount'] * 100, 'USD');
  $donation->donation['currency'] = 'USD'; //$line_item_wrapper->commerce_total->currency_code->value();

  // Flag the donation recurring or not
  $donation->donation['recurs_monthly'] = TRUE; //_fundraiser_commerce_order_confirm_line_item_recurring($line_item);

  return $donation;
}

/**
 * Lookup payment profile info and return full sim/profile_id string.
 *
 * @param $sim
 *
 */
function _sustainer_import_update_authnet_sim($payment_method, $cim_customer_profile_id) {

  // Build the get payment profile request data.
  $api_request_data = array(
    'customerProfileId' => $cim_customer_profile_id,
  );
  // TODO: add error handling if this breaks.
  $response = commerce_authnet_cim_request($payment_method, 'getCustomerProfileRequest', $api_request_data);
  $payment_profile_id = $response->profile->paymentProfiles->customerPaymentProfileId;

  return $cim_customer_profile_id . '|' . $payment_profile_id;
}

/**
 * Save cardonfile record for the user's credit card.
 *
 * If a record already exists for this card, return that record with no further processing.
 *
 * @param $sim
 * @param $record
 * @param $donation
 * @param $account
 * @return Ambigous
 */
function _sustainer_import_save_cc_data($sim, $record, $donation, $account) {
  $preexisting_cards = commerce_cardonfile_load_multiple_by_uid($account->uid);

  // we want to avoid creating duplicate card entries for a user.
  // dedupe on sim (remote id in {commerce_cardonfile})
  if ($preexisting_cards) {
    foreach ($preexisting_cards as $card) {
      if ($card->remote_id == $sim) {
        return $card;
      }
    }
  }
  $payment_method = commerce_payment_method_instance_load($donation->gateway);

  $card_data = commerce_cardonfile_new();
  $card_data->uid = $account->uid;
  $card_data->payment_method = $payment_method['method_id'];
  $card_data->instance_id = $payment_method['instance_id'];
  $card_data->remote_id = $sim;
  $card_data->card_type = $record['payment_details']['type'];
  $card_data->card_name = $record['user_info']['first_name'] . ' ' . $record['user_info']['last_name'];
  $card_data->card_number = $record['payment_details']['last_four'];
  $card_data->card_exp_month = $record['payment_details']['month'];
  $card_data->card_exp_year = $record['payment_details']['year'];
  $card_data->status = 1;
  $card_data->instance_default = 1; // set as default since this is the only record we have.

  // Save and log the creation of the new card on file.
  commerce_cardonfile_save($card_data);
  return $card_data;

}

/**
 * @param bool $key
 * @param bool $value
 * @param bool $message
 * @return mixed
 */
function sustainer_import_log_value($key = FALSE, $value = FALSE, $message = FALSE) {
  static $log = array();
  if (!$key && !$value && !$message) {
    return $log;
  }
  if (empty($log[$key])) {
    $log[$key] = array(
      'value' => $value,
      'message' => $message,
    );
  }
  elseif ($key == 'recurring_instance') {
    // do a little dance since we usually have several of these.
    if (count($log[$key]) == 1) {
      $original = $log[$key];
      $log[$key] = array();
      $log[$key][] = $original;
      $log[$key][] = array(
        'value' => $value,
        'message' => $message,
      );
    }
    else {
      $log[$key][] = array(
        'value' => $value,
        'message' => $message,
      );
    }
  }
}
