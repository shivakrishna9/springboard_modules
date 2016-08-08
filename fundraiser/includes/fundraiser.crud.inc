<?php
/**
 * @file
 * CRUD functions for Fundraiser.
 */

/**
 * Creates a Fundraiser.
 *
 * @param array|object $fundraiser
 *   The fundraiser to create.
 *
 * @see _fundraiser_update_fundraiser()
 */
function _fundraiser_create_fundraiser($fundraiser) {
  _fundraiser_update_fundraiser($fundraiser);
}

/**
 * Updates a fundraiser.
 *
 * @param array|object $fundraiser
 *   An array or object representing a fundraiser.
 */
function _fundraiser_update_fundraiser($fundraiser) {
  $fundraiser = (array) $fundraiser;
  // The key to use in `drupal_write_record`. If creating a fundraiser, this
  // should be `array()`, otherwise the key to update on ("nid").
  $record_key = array();
  // Hook to call, dependent on if fundraiser is being updated or created.
  $alter_hook = 'fundraiser_create_fundraiser';

  // If this fundraiser already exists, merge the new data onto the old record.
  if (isset($fundraiser['nid']) && !empty($fundraiser['nid'])) {
    $fundraiser_data = _fundraiser_get_fundraiser_by_nid($fundraiser['nid']);
    $fundraiser = array_merge((array) $fundraiser_data, $fundraiser);
    $record_key = 'nid';
    $alter_hook = 'fundraiser_update_fundraiser';
  }

  // Implode donation amounds for saving.
  $donation_amounts = array();
  foreach ($fundraiser['donation_amounts'] as $index => $donation_amount) {
    if (isset($donation_amount['weight']) && isset($donation_amount['amount'])) {
      // Settings are coming from the node edit page, each amount should be
      // an associative array with amount, label, and weight keys.
      $donation_amounts[$donation_amount['weight']] = $donation_amount['amount'] . '|' . $donation_amount['label'];
    }
    else {
      // Settings loaded directly from webform component, this means
      // the amount and label are in a numerically indexed array
      // and weight is not available.
      if (isset($donation_amount[0]) && isset($donation_amount[1])) {
        $donation_amounts[] = $donation_amount[0] . '|' . $donation_amount[1];
      }
    }
  }

  $fundraiser['donation_amounts'] = '';
  if (count($donation_amounts)) {
    ksort($donation_amounts);
    $donation_amounts = array_unique($donation_amounts);
    // On getting fundraiser this will be exploded into the correct values.
    $fundraiser['donation_amounts'] = implode('|||', $donation_amounts);
  }

  // Serialize for storage, our configured gateway values for storage in
  // components.
  $fundraiser['gateways'] = serialize('');
  if (!empty($fundraiser['gateways'])) {
    $fundraiser['gateways'] = serialize($fundraiser['gateways']);
  }

  $record = array(
    'receipt_email_from' => variable_get('site_name', ''),
    'receipt_email_address' => variable_get('site_mail', ''),
    'receipt_email_subject' => t('Thank you for your donation'),
    'redirect_url' => '<confirmation>',
  );
  $record = array_merge($record, $fundraiser);

  if (isset($record['default_amount']) && empty($record['default_amount'])) {
    unset($record['default_amount']);
  }

  // Create/update the record.
  drupal_write_record('fundraiser', $record, $record_key);
  // Invoke the hooks for all the other modules to respond.
  drupal_alter($hook, (object) $fundraiser);

  // Fire a hook that other modules can use to add additional processing when a
  // donation form is created/updated.
  // Called in sf_donation.module
  // This can, and should, be replaced with hook_node_insert() implementations.
  // THIS HOOK IS TO BE DEPR.
  // module_invoke_all('fundraiser_form_insert', $node, $components);
}

/**
 * Retrieve a fundraiser by node ID.
 *
 * @param int $nid
 *   The node ID to retrieve the fundraiser for.
 */
function _fundraiser_get_fundraiser_by_nid($nid) {
  // Get fundraiser table information.
  $fundraiser = db_select('fundraiser', 'f')
    ->fields('f')
    ->condition('nid', $nid, '=')
    ->execute();
  if ($fundraiser->rowCount()) {
    $fundraiser = $fundraiser->fetchObject();
    // Explode the donation amounts that were imploded during save.
    $donation_amounts = explode('|||', $fundraiser->donation_amounts);
    $donation_amounts = array_unique($donation_amounts);
    $exploded_amounts = array();
    foreach ($donation_amounts as $donation_amount) {
      $exploded_amounts[] = explode('|', $donation_amount);
    }
    $fundraiser->donation_amounts = $exploded_amounts;
    // Get the unserialized gateway configurations.
    $fundraiser->gateways = unserialize($fundraiser->gateways);
    // Get values from other modules for additional information.
    module_invoke_all('fundraiser_get_fundraiser_alter', $fundraiser);
    return $fundraiser;
  }
  return NULL;
}

/**
 * Retrieves a fundraiser based on the node IDs.
 *
 * @param array $nids
 *   The array of node IDs to retrieve the fundraisers for.
 */
function _fundraiser_get_fundraiser_by_nids($nids) {
  // Get fundraiser table information.
  $fundraisers = array();
  foreach ($nids as $nid) {
    $fundraisers[$nid] = _fundraiser_get_fundraiser_by_nid($nid);
  }
  return $fundraisers;
}

/**
 * Removes a fundraiser record from the database.
 *
 * @param int $nid
 *   The node ID of the fundraiser to remove.
 */
function _fundraiser_delete_fundraiser($nid) {
  db_delete('fundraiser')
    ->condition('nid', $nid)
    ->execute();
  // And invoke the hooks for all the other modules to respond.
  module_invoke_all('fundraiser_delete_fundraiser_alter', $nid);
}

/**
 * Removes multiple fundraisers from the database.
 *
 * @param array $nodes
 *   An array of fundraiser node IDs to remove.
 */
function _fundraiser_delete_fundraisers($nodes) {
  foreach ($nodes as $node) {
    _fundraiser_delete_fundraiser($node->nid);
  }
}

/**
 * Create a donation.
 *
 * @param array|object $donation
 *   The donation to create.
 */
function _fundraiser_create_donation($donation) {
  // Cast donation just in case.
  $donation = (array) $donation;
  // Check for old data.
  $donation_data = FALSE;
  if (isset($donation['did'])) {
    $donation_data = _fundraiser_get_donation_by_did($donation['did']);
  }
  if (!$donation_data) {
    $record = array_merge((array) $donation_data, $donation);
    drupal_write_record('fundraiser_donation', $record);
  }
  else {
    _fundraiser_update_donation($donation);
  }
}

/**
 * Retrieve all donations or a specific donation.
 *
 * @param int $did
 *   An optional donation ID to return that specific donation.
 *
 * @return array
 *   The donation(s).
 */
function _fundraiser_get_donations($did = NULL) {
  $query = db_select('fundraiser_donation', 'fd');
  if (!empty($did)) {
    $query->condition('did', $did, '=');
  }
  return $query->execute()->fetchAll();
}

/**
 * Retrieve a donation by donation ID.
 *
 * @param int $did
 *   The donation ID.
 *
 * @return array
 *   The donation.
 *
 * @see _fundraiser_get_donations()
 */
function _fundraiser_get_donation_by_did($did) {
  return _fundraiser_get_donations($did);
}

/**
 * CRUD style DB function for fundraiser_donation.
 */
function _fundraiser_update_donation($donation) {
  // Cast donation just in case.
  $donation = (array) $donation;
  // Check for old data.
  $donation_data = FALSE;
  if (isset($donation['did'])) {
    $donation_data = _fundraiser_get_donation_by_did($donation['did']);
  }
  if (!$donation_data) {
    _fundraiser_create_donation($donation);
  }
  else {
    $record = array_merge((array) $donation_data, $donation);
    drupal_write_record('fundraiser_donation', $record, 'did');
  }
}

/**
 * CRUD style DB function for fundraiser_donation.
 */
function _fundraiser_delete_donation($did) {
  db_delete('fundraiser_donation')->condition('did', $did)->execute();
}

/**
 * CRUD style DB function for fundraiser_refund.
 */
function _fundraiser_create_refund($refund) {
  // Cast donation just in case.
  $refund = (array) $refund;
  // Check for old data.
  $refund_data = FALSE;
  if (isset($refund['rid'])) {
    $refund_data = _fundraiser_get_refund_by_rid($refund['rid']);
  }
  if (!$refund_data) {
    drupal_write_record('fundraiser_refund', $refund);
  }
  else {
    $refund = _fundraiser_update_refund($refund);
  }
  return (object) $refund;
}

/**
 * CRUD style DB function for fundraiser_refund.
 */
function _fundraiser_get_refund_by_rid($rid) {
  return db_query('SELECT * FROM {fundraiser_refund} ' .
    'WHERE rid = :rid',
    array(':rid' => $rid))->fetchObject();
}

/**
 * CRUD style DB function for fundraiser_refund.
 */
function _fundraiser_update_refund($refund) {
  // Cast donation just in case.
  $refund = (array) $refund;
  // Check for old data.
  $refund_data = FALSE;
  if (isset($refund['rid'])) {
    $refund_data = _fundraiser_get_refund_by_rid($refund['rid']);
  }
  if (!$refund_data) {
    $refund = _fundraiser_create_refund($refund);
  }
  else {
    $record = array_merge((array) $refund_data, $refund); // Merge data together so we get everything in the record.
    drupal_write_record('fundraiser_refund', $refund, 'rid');
  }
  return (object) $refund;
}

/**
 * CRUD style DB function for fundraiser_refund.
 */
function _fundraiser_delete_refund($rid) {
  db_delete('fundraiser_refund')->condition('rid', $rid)->execute();
}

/**
 * DB function for fundraiser_refund.
 */
function _fundraiser_get_refunds_by_did($did) {
  return db_query('SELECT * FROM {fundraiser_refund} ' .
    'WHERE did = :did',
    array(':did' => $did))->fetchAll();
}

/**
 * DB function for fundraiser_refund.
 */
function _fundraiser_get_refund_by_txn_id($txn_id) {
  return db_query('SELECT * FROM {fundraiser_refund} ' .
    'WHERE txn_id = :txn_id',
    array(':txn_id' => $txn_id))->fetchAll();
}

/**
 * DB Function, Check if the given order was made on a payment gateway that supports refunds
 */
function _fundraiser_get_donation_gateway($did) {
  // First check the donation table. It trumps the fundraiser table in case the FR has changed since.
  $found_gateway = db_query('SELECT d.gateway FROM {fundraiser_donation} d ' .
    'WHERE d.did = :did', array(':did' => $did))->fetchField();
  // Now ask if any other module wants to override this result.
  module_invoke('fundraiser_get_donation_gateway_alter', $did, $found_gateway);
  // Now translate that into something we can use by getting the rest of the dateway info.
  if (isset($found_gateway)) {
    return _fundraiser_gateway_info($found_gateway);
  }
  // If nothing has been found, return nothing.
  return array();
}

/**
 * CRUD style DB function for fundraiser_tracking.
 */
function _fundraiser_create_tracking($tracking) {
  $tracking = (array) $tracking;
  $tracking_data = FALSE;
  if (isset($tracking['nid'])) {
    $tracking_data = _fundraiser_get_tracking_by_nid($tracking['nid']);
  }
  if (!$tracking_data) {
    $record = array(
      'pageviews' => 1,
      'conversions' => 0,
      'local_failures' => 0,
      'gateway_failures' => 0,
      'latest_load_time' => 0,
    );
    $tracking = array_merge($record, $tracking);
    drupal_write_record('fundraiser_tracking', $tracking);
  }
  else {
    _fundraiser_update_tracking($tracking);
  }
}

/**
 * CRUD style DB function for fundraiser_tracking.
 */
function _fundraiser_get_tracking_by_nid($nid) {
  return db_query('SELECT * FROM {fundraiser_tracking} WHERE nid = :nid', array(':nid' => $nid))->fetchObject();
}

/**
 * CRUD style DB function for fundraiser_tracking.
 */
function _fundraiser_update_tracking($tracking) {
  /**
   * Code commented out on 2/18/2016 by pcave for performance reasons.
   * When a donation form encounters significant traffic, updating a single row
   * in this table over and over during a request can cause lock contentions.
   * This eventually cascades to other database operations thus slowing down
   * the entire response.
   *
   * $tracking = (array) $tracking;
   * $tracking_data = FALSE;
   * if (isset($tracking['nid'])) {
   *   $tracking_data = _fundraiser_get_tracking_by_nid($tracking['nid']);
   * }
   * if (!$tracking_data) {
   *   _fundraiser_create_tracking($tracking);
   * }
   * else {
   *   $tracking = array_merge((array) $tracking_data, $tracking);
   *   drupal_write_record('fundraiser_tracking', $tracking, 'nid');
   * }
   */
}

/**
 * CRUD style DB function for fundraiser_tracking.
 */
function _fundraiser_delete_tracking($nid) {
  /**
   * Code commented out on 2/18/2016 by pcave for performance reasons.
   * When a donation form encounters significant traffic, updating a single row
   * in this table over and over during a request can cause lock contentions.
   * This eventually cascades to other database operations thus slowing down
   * the entire response.
   *
   * db_delete('fundraiser_tracking')->condition('nid', $nid)->execute();
   */
}

/**
 * DB function for updating the stats in fundraiser_tracking.
 */
function _fundraiser_update_tracking_value($nid, $field) {
   /**
   * Code commented out on 2/18/2016 by pcave for performance reasons.
   * When a donation form encounters significant traffic, updating a single row
   * in this table over and over during a request can cause lock contentions.
   * This eventually cascades to other database operations thus slowing down
   * the entire response.
   *
   * $tracking = _fundraiser_get_tracking_by_nid($nid);
   * // No tracking data? Create one.
   * if (!$tracking) {
   *   $tracking = new stdClass();
   *   $tracking->nid = $nid;
   *   _fundraiser_create_tracking($tracking);
   *   $tracking = _fundraiser_get_tracking_by_nid($nid);
   * }
   * // Check to make sure we weren't handed a non-existing track.
   * if (isset($tracking->$field)) {
   *   $tracking->$field++;
   *   _fundraiser_update_tracking($tracking);
   * }
   */
  }
