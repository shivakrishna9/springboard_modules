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
  dsm($gateway_types, 'gateway types');
  foreach ($gateway_types as $gateway_type) {
    dsm($gateway_type);
    $defaults = variable_get('sustainer_import_gateway_map_' . $gateway_type, array());
    dsm($defaults, 'defaults');
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
    // apply pending label to items with empty status
    if (!$status_count['status']) {
      $status_count['status'] = 'pending';
    }
    $status_rows[] = array('data' => array($status_count['status'], $status_count['count']));
  }
  $headers = array(
    array('data' => array('status')),
    array('data' => array('count')),

  );
  $table = theme('table', array('header' =>$headers, 'rows' =>$status_rows));
 // $pending_queue = sustainer_import_get_pending_queue_items();

  // generate report table
  return $table;
}

function sustainer_import_process_queue() {
  // get pending queue items
  $queue = sustainer_import_get_pending_queue_items();
  // parse item
  foreach ($queue as $record) {
    dsm($record);
    // user record
    // validate
    if (_sustainer_import_validate_account_record($record['user_info'])) {
      // passes validation, create or dedupe.
      $account = _sustainer_import_create_account($record['user_info']);
      dsm($account, 'account');
      $record['uid'] = $account->uid;
    }
    else {
      // TODO: log error with user account creation and continue to next record.
    }
  // master order record
  if ($record['uid']) {
    if(_sustainer_import_validate_master_order($record)) {
      $donation = _sustainer_import_create_master_order($record, $account);
      dsm($donation, 'master_donation');
    }
  }
  else {
    // TODO: log error with order/donation/sim/cardonfile creation and continue to next record.
  }
    // create
  // sustainer series
  if ($donation->did && $record['uid']) {
    foreach ($record['payment_schedule'] as $instance_details) {
      if (_sustainer_import_validate_sustainer_series_instance($instance_details)) {
        _sustainer_import_create_sustainer_series_instance($instance_details, $donation);
      }
      else {
        // TODO: throw error and bail
      }
    }
    // TODO: if bail, log error and continue to next record.
  }
  // TODO: log errors/status/id's for this record
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

    // if we have SF id's for the account, save them now.
    if ($account_details['sf_contact_id']) {
      _sustainer_import_save_account_sfids($account->uid, $account_details['sf_contact_id'], 'Contact');
    }
    if ($account_details['sf_account_id']) {
      _sustainer_import_save_account_sfids($account->uid, $account_details['sf_account_id'], 'Account');
    }
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
function _sustainer_import_validate_account_record($account_details) {
  // confirm all required fields have data
  $errors = array();
  $required_fields = array('email', 'sf_contact_id', 'sf_account_id');
  foreach ($required_fields as $field) {
    if (empty($account_details[$field])) {
      $errors[] = t('Missing !field in user record', array('!field' => $field));
    }
    // confirm email address is valid
    if ($field == 'email' && !filter_var($account_details['email'], FILTER_VALIDATE_EMAIL)) {
      $errors[] = t('Invalid email address: !address', array('!address' => $account_details['email']));
    }
  }

  if (count($errors)) {
    // TODO: log transaction errors, update queue record and bail
    drupal_set_message(implode(', ', $errors), 'error');
    return FALSE;
  }
  dsm($account_details, 'user record passes validation');
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
 * @return bool
 * Returns TRUE if the import record passes validation, FALSE otherwise.
 */
function _sustainer_import_validate_master_order($record) {
  $errors = array();
  // confirm all required fields have data
  $required_fields = array('uid', 'amount', 'salesforce_opportunity_id', 'salesforce_recurring_donation_id');
  // validate top level fields
  foreach ($required_fields as $field) {
    if (empty($record[$field])) {
      $errors[] = t('Missing required field for master order:!field', array('!field' => $field));
    }
    if ($field == 'amount') {
      if (!is_numeric($record['amount'])) {
        $errors[] = t('Donation amount is not numeric: !amount', array('!amount' => $record['amount']));
      }
    }
  }

  // hit user_info
  if (empty($record['user_info']['first_name'])) {
    $errors[] = t('Missing first_name in user info');
  }
  if (empty($record['user_info']['last_name'])) {
    $errors[] = t('Missing last_name in user info');
  }

  // billing info
  $required_fields = array('address_1', 'city', 'state', 'zip', 'country');
  foreach ($required_fields as $field) {
    if (empty($record['billing_info'][$field])) {
      $errors[] = t('Missing required billing information field:!field', array('!field' => $field));
    }
  }

  // payment_details
  $required_fields = array('gateway_token', 'type', 'last_four', 'month', 'year');
  foreach ($required_fields as $field) {
    if (empty($record['payment_details'][$field])) {
      $errors[] = t('Missing required payment details field:!field', array('!field' => $field));
    }
    if ($field == 'last_four' && !is_numeric($record['payment_details']['last_four'])) {
      $errors[] = t('Last four are not numeric:!last_four', array('!last_four' => $record['payment_details']['last_four']));
    }
    // TODO: add numeric checks for month and year
  }

  if (count($errors)) {
    // TODO: log transaction errors, update queue record and bail
    drupal_set_message(implode(', ', $errors), 'error');
    return FALSE;
  }
  dsm($record, 'master order details pass validation');
  return TRUE;
}

function _sustainer_import_create_master_order($record, $account) {
  // create master order
  $gateway_map = variable_get('sustainer_import_gateway_map_' . $record['payment_details']['gateway_type'], array());
  dsm($gateway_map, 'gateway map during order creation');
  if (!empty($gateway_map['nid']) && is_numeric($gateway_map['nid'])) {
    $node = node_load($gateway_map['nid']);
  }
  else {
    // TODO: log map error and bail
    $node = FALSE; // temporary measure.
  }
  dsm($node, 'node during donation creation');
  $donation = _sustainer_import_build_donation_object($record, $account, $node);
  fundraiser_donation_create($donation);
  dsm($donation, 'donation after create');
  // update commerce order status to payment received.
  if (!empty($donation->did)) {
    db_query('UPDATE {commerce_order} SET status = :status WHERE order_id = :order_id', array(':status' => 'payment_received', ':order_id' => $donation->did));
  }
  // save payment method details (cardonfile)
  $payment_method = commerce_payment_method_instance_load($donation->gateway);

  // get updated sim
  // TODO: this needs error handling. If authnet falls down trying to update the profile id
  // we have to hault the import since future orders won't be billable until this is resolved.
  $sim = _sustainer_import_update_authnet_sim($payment_method, $record['payment_details']['gateway_token']);

  // store cardonfile entry.
  $card_data = _sustainer_import_save_cc_data($sim, $record, $donation, $account);
  return $donation;
}

function _sustainer_import_validate_sustainer_series_instance($instance_details) {
  // confirm all required fields have data
  // de-dupe??
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
  // Create the new donation object, but do not process it. Processing doesn't occur till cron.
  // New donation has all of the old donation information already loaded.
  fundraiser_donation_create($new_donation);
  $next_charge = strtotime($instance_details['month'] . '/' . $instance_details['day'] . '/' . $instance_details['year']);
  drupal_set_message('next charge:' . $next_charge);
  /*
  fundraiser_donation_comment($new_donation, 'Sustainer donation scheduled to be charged at @next_charge',
    array('@next_charge' => format_date($next_charge)));
  // After this, the donation did and donation data is set.
  // The new donation object, with new did, needs to be saved to recurring so we can grab it later.
  $recurring_donation = array(
    'master_did' => $donation->did,
    'did' => $new_donation->did,
    'next_charge' => $next_charge,
    'sustainer_key' => $sustainer_key,
  );
  // Update the recurring table.
  _fundraiser_sustainers_create_recurring($recurring_donation);
  */
  // Stage 2: update the sustainer record with status, sf id,
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
    'mid' => NULL,
    'sfid' => $sfid,
    'drupal_id' => $uid,
    'module' => 'user',
    'delta' => 'user',
    'object_type' => $type,
  );
  salesforce_sync_save_map($sf_map);
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
  dsm($record, 'record during donation build');
 // $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
 // $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);

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
  $donation->donation['amount'] = commerce_currency_amount_to_decimal($record['amount'], 'USD');
  $donation->donation['currency'] = 'USD'; //$line_item_wrapper->commerce_total->currency_code->value();

  // Flag the donation recurring or not
  $donation->donation['recurs_monthly'] = TRUE; //_fundraiser_commerce_order_confirm_line_item_recurring($line_item);

   dsm($donation, 'donation');
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
  dsm($payment_method, 'method loaded during cc save');

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
