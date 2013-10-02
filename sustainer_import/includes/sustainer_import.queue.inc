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
  return $form;
}

function sustainer_import_queue_report() {
  // list items in queue
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
      $record['uid'] = $account->uid;
    }
  // master order record
  if ($record['uid']) {
    if(_sustainer_import_validate_master_order($record)) {
      // create master order
    }
  }
    // create
  // sustainer series
    //foreach
      // validate
      // create
  // update log
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
 * Associative array of data required to create a sustainer series master order in commerce and
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

function _sustainer_import_create_master_order($record) {
  // create master order
}

function _sustainer_import_validate_sustainer_series_instance($instance_details) {
  // confirm all required fields have data
  // de-dupe??
}

function _sustainer_import_create_sustainer_series_instance($instance_details) {
  // create sustainer series instance.
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