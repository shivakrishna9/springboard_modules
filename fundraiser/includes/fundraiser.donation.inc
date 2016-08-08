<?php
/**
 * @file
 * Donation functions for Fundraiser.
 */

/**
 * Helper function, determine if this node type supports donation forms.
 */
function fundraiser_is_donation_type($type) {
  static $fundraiser_types;
  // Cache fundraiser content types during page execution.
  if (!isset($fundraiser_types[$type])) {
    $fundraiser_types[$type] = variable_get('fundraiser_' . $type, FALSE);
  }
  return $fundraiser_types[$type];
}

/**
 * Helper function, determine if this node type supports donation forms.
 */
function fundraiser_get_donation_types() {
  // For all types, check if they are a fundraiser type.
  $types = node_type_get_types();
  foreach ($types as $type => $type_info) {
    $fundraiser_types[$type] = variable_get('fundraiser_' . $type, FALSE);
  }
  return $fundraiser_types;
}

/**
 * Donation processing, a given donation needs to be commented on.
 */
function fundraiser_donation_comment($donation, $comment, $variables = array(), $severity = WATCHDOG_NOTICE, $link = NULL) {
  // Make a log entry.
  if (variable_get('fundraiser_log_comments', 0)) {
    watchdog('fundraiser_comment', $comment, $variables, $severity, $link);
  }
  // Allow other modules to respond to the cancel. Including Ubercart, etc.
  module_invoke_all('fundraiser_donation_comment', $donation, $comment, $variables, $severity, $link);
}

/**
 * Given a donation id, gather information to display for that donation.
 *
 * To be displayed when needed by other modules.
 */
function fundraiser_donation_information_table($did) {
  $rows = array();
  $donation = fundraiser_donation_get_donation($did);
  if (!$donation) {
    return;
  }

  $rows[] = array('Fundraiser user', t('User #@uid - !name',
    array(
      '@uid' => $donation->uid,
      '!name' => l($donation->user->name, 'user/' . $donation->user->uid))
    ),
  );
  if (isset($donation->node->nid)) {
    $rows[] = array('Fundraiser node', t('Node #@nid - !title', array(
      '@nid' => $donation->nid,
      '!title' => l($donation->node->title, 'node/' . $donation->node->nid),
    )));
  }
  else {
    $rows[] = array('Fundraiser node', t('Node #@nid - !title', array(
      '@nid' => '---',
      '!title' => t('No fundraiser node found.'),
    )));
  }
  $variables = array(
    'header' => array(),
    'rows' => $rows,
  );

  drupal_alter('fundraiser_donation_information_table', $variables, $donation);
  return theme('table', $variables);
}

/**
 * Donation processing, a given donation failed to validate.
 */
function fundraiser_donation_failed_validation($errors) {
  // Handle our own tracking.
  _fundraiser_update_tracking_value($errors['nid'], 'local_failures');
  // Call hook to allow sub modules to respond to the failure as well.
  module_invoke_all('fundraiser_donation_failed_validation', $errors);
}

/**
 * Donation processing, a donation needs to be created prior to processing.
 */
function fundraiser_donation_create($donation) {
  // The gateway is selected by the user, was set during submission. Same as
  // nid, uid, sid etc.
  // Call hooks to create the donation. The glue module handles everything else
  // once handed fundraisers info.
  // This is a good location to update $donation->data for processing.
  module_invoke_all('fundraiser_donation_create', $donation);
  // For now, our creation system is responsible for creating a did.
  // Track the did, nid, uid internally so we can keep things in order.
  // Grab the submission path, if there is no alias, use the standard node path.
  global $base_url;
  $alias = drupal_lookup_path('alias', $_GET['q']);
  $form_url = '';
  if (!empty($alias)) {
    $form_url = $base_url . '/' . $alias;
  }
  else {
    $form_url = $base_url . '/node/' . $donation->nid;
  }

  $donation_record = array(
    // Donation id.
    'did' => $donation->did,
    // Fundraiser node id.
    'nid' => $donation->nid,
    // Submission user id.
    'uid' => $donation->uid,
    // Submission id.
    'sid' => $donation->sid,
    'status' => $donation->status,
    'amount' => $donation->amount,
    'currency' => $donation->currency,
    'txn_id' => '',
    'form_url' => $form_url,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
  );
  if (is_string($donation->gateway)) {
    $donation_record['gateway'] = $donation->gateway;
  }
  elseif (is_array($donation->gateway) && isset($donation->gateway['id']) && !empty($donation->gateway['id'])) {
    $donation_record['gateway'] = $donation->gateway['id'];
  }
  _fundraiser_create_donation($donation_record);
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was created on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // Respond after the creation of a donation. (Insert essentially)
  // At this point an order has been created by the charging module and etc.
  module_invoke_all('fundraiser_donation_post_create', $donation);
}

/**
 * Donation processing, a donation needs to be updated with new information prior to processing.
 * For example, in the course of updating a sustainers information.
 */
function fundraiser_donation_update($donation, $vocal_mode = TRUE) {
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  module_invoke_all('fundraiser_donation_update', $donation);
  // The additional fields $donation->additional, ready to process.
  // For now, our creation system is responsible for creating a did.
  // Track the did, nid, uid internally so we can keep things in order.
  $fundraiser_node = node_load($donation->nid);
  $donation_record = array('did' => $donation->did);
  if (isset($donation->nid) && !empty($donation->nid)) {
    $donation_record['nid'] = $donation->nid;
  }
  if (isset($donation->uid) && !empty($donation->uid)) {
    $donation_record['uid'] = $donation->uid;
  }
  if (isset($donation->sid) && !empty($donation->sid)) {
    $donation_record['sid'] = $donation->sid;
  }
  if (isset($donation->status) && !empty($donation->status)) {
    $donation_record['status'] = $donation->status;
  }
  if (isset($donation->amount) && !empty($donation->amount)) {
    $donation_record['amount'] = $donation->amount;
  }
  elseif (!empty($donation->donation['amount'])) {
    $donation_record['amount'] = $donation->donation['amount'];
    // Was a quantity (of > 1) specified? (legacy forms).
    if (isset($donation->donation['quantity']) && $donation->donation['quantity'] > 1) {
      // Multiply base donation amount by quantity to get the donation total.
      $donation_record['amount'] = number_format($donation->donation['amount'] * $donation->donation['quantity'], 2);
    }
  }
  if (isset($donation->currency) && !empty($donation->currency)) {
    $donation_record['currency'] = $donation->currency;
  }
  if (isset($donation->gateway) && !empty($donation->gateway)) {
    if (is_string($donation->gateway)) {
      $donation_record['gateway'] = $donation->gateway;
    }
    elseif (is_array($donation->gateway) && isset($donation->gateway['id']) && !empty($donation->gateway['id'])) {
      $donation_record['gateway'] = $donation->gateway['id'];
    }
  }
  if (isset($donation->txn_id) && !empty($donation->txn_id)) {
    $donation_record['txn_id'] = $donation->txn_id;
  }
  if (isset($donation->form_url) && !empty($donation->form_url)) {
    $donation_record['form_url'] = $donation->form_url;
  }
  $donation_record['changed'] = REQUEST_TIME;
  _fundraiser_update_donation($donation_record);
  // Refresh the cache. No need to change the donation object passed in, but
  // stored values should be zapped.
  fundraiser_donation_get_donation($donation->did, TRUE);
  // Add a comment.
  if ($vocal_mode) {
    global $user;
    $username = isset($user->name) ? $user->name : 'Anonymous';
    fundraiser_donation_comment($donation, 'This donation was updated on @date by @username.',
      array('@date' => format_date(strtotime('now')), '@username' => $username));
  }
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_donation_post_update', $donation);
}

/**
 * Copy donation object without references to the original object.
 */
function _fundraiser_donation_copy($donation) {
  return unserialize(serialize($donation));
}

/**
 * Donation processing, a given donation needs to be processed.
 */
function fundraiser_donation_process($donation) {
  // Create an array of values for the log.
  $log = array(
    'start' => microtime(TRUE),
    'transaction_type' => 'charge',
  );

  // Pass this down to the processing module.
  module_invoke_all('fundraiser_donation_process', $donation);

  // Mark the end time of the processing.
  $log['end'] = microtime(TRUE);

  // Log the processing details.
  fundraiser_log_donation_processing($donation, $log);

  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_donation_post_process', $donation);
}

/**
 * Donation processing, a given donation was successful.
 */
function fundraiser_donation_success($donation) {
  // Track the success.
  _fundraiser_update_tracking_value($donation->nid, 'conversions');
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message && isset($donation->result['message'])) {
    drupal_set_message($donation->result['message']);
  }
  // Allow other modules to respond to the success.
  module_invoke_all('fundraiser_donation_success', $donation);
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was successfully completed on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
}

/**
 * Donation processing, a given donation was declined.
 */
function fundraiser_donation_decline($donation) {
  // Track this failure.
  _fundraiser_update_tracking_value($donation->nid, 'gateway_failures');
  // Make a log entry.
  watchdog('fundraiser', 'Donation @id was declined by the payment gateway. Reason: @message',
    array('@id' => $donation->did, '@message' => $donation->result['message']), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message($donation->result['message']);
  }
  // Allow other modules to respond to the decline.
  module_invoke_all('fundraiser_donation_decline', $donation);
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was declined at the gateway on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
}

// TODO how does this even get called? Is this functionally different from a decline?
/**
 * Donation processing, a given donation experienced an exception.
 */
function fundraiser_donation_exception($donation) {
  // Make a log entry.
  watchdog('fundraiser', 'An error occurred while processing donation @id. Error: @message',
    array('@id' => $donation->did, '@message' => $donation->result['message']), WATCHDOG_ERROR, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message($donation->result['message']);
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation encountered an exception on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Allow other modules to respond to the exception.
  module_invoke_all('fundraiser_donation_exception', $donation);
}

/**
 * Donation processing, a given donation needs to be cancelled.
 * Generally, only available for Authorize.net gateway donations. For example, during sustainer call.
 */
function fundraiser_donation_cancel($donation) {
  // Make a log entry
  watchdog('fundraiser', 'Donation @id was canceled by the user.',
    array('@id' => $donation->did), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message(t('Your payment #:did has been canceled.', array(':did' => $donation->did)));
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was cancelled on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);

  // Allow other modules to respond to the cancel. Including Ubercart, etc.
  module_invoke_all('fundraiser_donation_cancel', $donation);
}

/**
 * Donation processing, a given donation needs to be deleted.
 * Generally, not called, but it does happen.
 */
function fundraiser_donation_delete($donation) {
  // Make a log entry
  watchdog('fundraiser', 'Donation @id was deleted.',
    array('@id' => $donation->did), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message(t('Your payment #:did has been deleted.', array(':did' => $donation->did)));
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was deleted on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Allow other modules to respond to the cancel. Including Ubercart, etc.
  module_invoke_all('fundraiser_donation_delete', $donation);
}

/**
 * Utility function, given a donation id - get everything that goes with it.
 *
 * @param int $did
 *   The ID of the donation
 * @param bool $refresh
 *   If TRUE rebuild the entire donation even if it's statically cached.
 *
 * @return object|bool
 *    The fully loaded donation object or FALSE if no donation is found
 */
function fundraiser_donation_get_donation($did, $refresh = FALSE) {
  // Pull from cached donation unless we are refreshing the cache.
  static $donations;
  if (!$refresh && isset($donations[$did])) {
    return $donations[$did];
  }

  // Grab basic data from our own systems.
  $donation_record = _fundraiser_get_donation_by_did($did);
  if (!$donation_record) {
    return FALSE;
  }

  // If refreshing, or first load, pull all data from database.
  $donation = (object) array();
  $donation->did = $did;
  $donation->nid = $donation_record->nid;
  $donation->node = node_load($donation_record->nid);
  $donation->uid = $donation_record->uid;
  $donation->user = user_load($donation_record->uid);
  $donation->sid = $donation_record->sid;
  // Additional fields.
  $donation->status = $donation_record->status;
  $donation->amount = $donation_record->amount;
  $donation->currency = $donation_record->currency;
  $donation->created = $donation_record->created;
  $donation->changed = $donation_record->changed;

  // Grab gateway information and replace it in the node too if found.
  $gateway = _fundraiser_get_donation_gateway($did);
  if (isset($gateway['id'] )) {
    $donation->gateway = $gateway;
  }
  // Grab the transaction id if it exists, add it here.
  $donation->txn_id = isset($donation_record->txn_id) ? $donation_record->txn_id : '';
  $donation->form_url = $donation_record->form_url;
  // And has refund grab them too
  $donation->refunds = FALSE;
  $donation->refunds = fundraiser_refund_get_refunds_by_did($did, TRUE);

  // Call hooks to re-create the donation info.
  // The glue modules handle everything else once handed fundraisers info.
  module_invoke_all('fundraiser_donation_get_donation', $donation);
  // Store the donation information into the cache.
  $donations[$did] = $donation;

  return $donation;
}

/**
 * Get the first donation ID that matches the given submission ID.
 *
 * Be aware that multiple donations may have the same submission ID.
 * For example, recurring donations will have the initial sid.
 *
 * @param int $sid
 *   The submission ID.
 *
 * @return int|bool
 *   The donation ID, or FALSE.
 */
function fundraiser_donation_get_did_by_sid($sid) {
  return db_query("SELECT MIN(did) FROM {fundraiser_donation} WHERE sid = :sid", array(':sid' => $sid))->fetchField();
}

/**
 * Total donation amount donated across multiple forms.
 *
 * Ecommerce-agnostic lookup of total dollar amount donated via multiple
 *   donation forms.
 *
 * Looks up by multiple nids in the hopes of a single faster IN query.
 *
 * @param array $nids
 *   Node IDs.
 * @param int|bool $start
 *   Start timestamp or FALSE for no range.
 * @param int|bool $end
 *   Ending timestamp or FALSE for no range.
 *
 * @return float|bool
 *   Total successful donation amount or FALSE if none found.
 */
function fundraiser_get_total_donations_by_nids($nids, $start = FALSE, $end = FALSE) {
  $total_donations = module_invoke_all('fundraiser_get_total_donations_by_nids', $nids, $start, $end);
  $total = isset($total_donations[0]) ? $total_donations[0] : FALSE;
  return $total;
}
