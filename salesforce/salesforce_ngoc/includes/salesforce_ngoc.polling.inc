<?php
/**
 * @file
 * Functions for polling SF for changes to batch_upload processing status.
 */

/**
 * Implements hook_salesforce_ngoc_batch_status_change().
 */
function salesforce_ngoc_salesforce_ngoc_batch_status_change(&$batch_upload_sobjects, &$changed_records) {
  // Create {salesforce_sync_map} records that reference the Contact, Account,
  // and Opportunities created during processing of batch upload objects.
  salesforce_ngoc_create_sync_map_records($batch_upload_sobjects, $changed_records);
  // Release child records from hold queue.
  salesforce_ngoc_release_children_from_hold_queue($changed_records);
}

/**
 * Creates {salesforce_sync_map} records from updated batch upload objects.
 *
 * Creates records for Contacts, Accounts, and Opportunities that have been
 * created during the processing of batch upload records in SF.
 *
 * @param array $batch_upload_sobjects
 *   An assoc array containing SObject objects, keyed by the objects SF Id.
 * @param array $changed_records
 *   Array of salesforce_ngoc_sync_map records with their new values.
 */
function salesforce_ngoc_create_sync_map_records(&$batch_upload_sobjects, &$changed_records) {
  foreach ($changed_records as $changed_record) {
    if (isset($changed_record->contact_sfid)) {
      $record = array(
        'sfid' => $changed_record->contact_sfid,
        'drupal_id' => $batch_upload_sobjects[$changed_record->batch_upload_sfid]->fields->SB_User_Id__c,
        'module' => 'user',
        'delta' => 'user',
        'object_type' => 'Contact',
      );
      salesforce_sync_save_map($record);
    }
    if (isset($changed_record->account_sfid)) {
      $record = array(
        'sfid' => $changed_record->account_sfid,
        'drupal_id' => $batch_upload_sobjects[$changed_record->batch_upload_sfid]->fields->SB_User_Id__c,
        'module' => 'user',
        'delta' => 'user',
        'object_type' => 'Account',
      );
      salesforce_sync_save_map($record);
    }
    if (isset($changed_record->opportunity_sfid)) {
      $record = array(
        'sfid' => $changed_record->opportunity_sfid,
        'drupal_id' => $batch_upload_sobjects[$changed_record->batch_upload_sfid]->fields->SB_Donation_Id__c,
        'module' => 'salesforce_donation',
        'delta' => 'donation',
        'object_type' => 'Opportunity',
      );
      salesforce_sync_save_map($record);
    }
  }
}

/**
 * List hold_queue_items with drupal_ids matching changed_records.
 *
 * Givin an array of objects that represents the batch upload objects that have
 * changed in a polling cycle, return a list of hold_queue items that match the
 * dids and uids of the changed records.
 *
 * @param array $changed_records
 *   An array of objects that represents the batch upload objects that have
 *   changed.
 *
 * @return object
 *   A db_query result set object with zero or more results.
 */
function salesforce_ngoc_get_child_hold_queue_items($changed_records) {
  $ngoc_sync_map_uids = array_map(function($o) { return $o->uid; }, $changed_records);
  $ngoc_sync_map_dids = array_map(function($o) { return $o->did; }, $changed_records);

  // @todo: if either uids or dids are empty, we get a PDO exception. Need to
  // construct this query dynamically?

  // Find {salesforce_ngoc_hold_queue_items} that have parent_drupal_ids that
  // match those returned in $changed_records.
  $hold_queue_result = db_query("SELECT id, drupal_id, object_type, parent_type, parent_drupal_id
    FROM {salesforce_ngoc_hold_queue_items}
    WHERE (parent_drupal_id IN (:uids) AND parent_type = 'Contact') OR
          (parent_drupal_id IN (:dids) AND parent_type = 'Opportunity')",
    array(':uids' => $ngoc_sync_map_uids, ':dids' => $ngoc_sync_map_dids));
  return $hold_queue_result;
}


/**
 * Moves items from hold' to 'new' queue, if parents exist in sync_map table.
 *
 * @param array $changed_records
 *    An array of Objects representing changed salesforce_ngoc_sync_map records.
 *    Array(
 *      [0] => stdClass Object (
 *        [id] => 22
 *        [batch_upload_sfid] => a1MW0000000Tgc7MAC
 *        [sf_status] => Committed
 *        [drupal_id] => 1000500
 *        [uid] => 1000352
 *        [did] => 1000500
 *        [module] => salesforce_donation
 *        [delta] => donation
 *        [contact_sfid] => 003W000000UaNsRIAV
 *        [account_sfid] => 001W000000OaaleIAB
 *        [opportunity_sfid] => 006W0000006FbWKIA0
 *        [created] => 1480011866
 *        [lasted_polled] => 1480011934
 *        [polling_attempts] => 1
 *      )
 *    ).
 *
 * @todo Watchdog some info.
 */
function salesforce_ngoc_release_children_from_hold_queue(&$changed_records) {
  $child_hold_queue_items = salesforce_ngoc_get_child_hold_queue_items($changed_records);
  // @todo: Break this foreach out into its own function for code readability.
  $salesforce_ngoc_parent_objects = variable_get('salesforce_ngoc_parent_objects');
  $salesforce_queue_records_to_update = array();
  $hold_queue_records_to_delete = array();
  // Loop through the hold_queue records to find the changed_record that has the
  // corresponding parent did or uid.
  foreach ($child_hold_queue_items as $hold_queue_record) {
    $unsync_parent_found = FALSE;
    foreach ($changed_records as $changed_record) {
      if (($hold_queue_record->parent_type == 'Contact' && $hold_queue_record->parent_drupal_id == $changed_record->uid) ||
          ($hold_queue_record->parent_type == 'Opportunity' && $hold_queue_record->parent_drupal_id == $changed_record->did)) {
        // We've found the corresponding changed_record, now see if it has
        // the sfid for contact or opportunity or both depending on object type.
        foreach ($salesforce_ngoc_parent_objects[$hold_queue_record->object_type] as $parent_object_type) {
          if ($parent_object_type == 'Contact') {
            if (empty($changed_record->contact_sfid)) {
              // No contact_sfid found so $hold_queue_record has an
              // unsync'd parent.
              $unsyncd_parent_found = TRUE;
            }
          }
          elseif ($parent_object_type == 'Opportunity') {
            if (empty($changed_record->opportunity_sfid)) {
              // No opportunity_sfid found so $hold_queue_record has an
              // unsync'd parent.
              $unsyncd_parent_found = TRUE;
            }
          }
        }
      }
    }
    if ($unsyncd_parent_found == FALSE) {
      // Save data info for use in queries below.
      $salesforce_queue_records_to_update[$hold_queue_record->object_type][] = $hold_queue_record->drupal_id;
      $hold_queue_records_to_delete[] = $hold_queue_record->id;
    }
  }
  if (!empty($salesforce_queue_records_to_update)) {
    // Move queue items from 'hold' to 'new' queue.
    // The constructed query looks something like this:
    // UPDATE {salesforce_queue} SET queue='new' WHERE
    // (drupal_id IN (1, 2, 3) AND object_type = 'Social_Share__c') OR
    // (drupal_id IN (3, 4, 5) AND object_type = 'sb_Actions_Taken__c').
    $update_query = db_update('salesforce_queue')->fields(array('queue' => 'new'));
    foreach ($salesforce_queue_records_to_update as $type => $drupal_ids) {
      $db_and = db_and()->condition('drupal_id', $drupal_ids, 'IN')->condition('object_type', $type);
      $update_query->condition(db_or()->condition($db_and));
    }
    $update_query->execute();
  }
  if (!empty($hold_queue_records_to_delete)) {
    // Since this is no longer in the hold queue, remove records from
    // {salesforce_ngoc_hold_queue_items}.
    db_query("DELETE FROM {salesforce_ngoc_hold_queue_items} WHERE id IN (:id)", array(':id' => $hold_queue_records_to_delete));
  }
}

/**
 * Queries SF batch_upload objects for change in processing status.
 */
function salesforce_ngoc_poll_batch_upload() {
  $changed_records = array();
  // Get records from {salesforce_ngoc_sync_map}, oldest first.
  $ngoc_sync_map_records = db_query_range("SELECT * FROM {salesforce_ngoc_sync_map} WHERE sf_status != 'Committed' ORDER BY created DESC", 0, variable_get('salesforce_ngoc_poll_record_limit', SALESFORCE_NGOC_DEFAULT_POLL_RECORD_LIMIT))->fetchAllAssoc('batch_upload_sfid');
  $sf_ids = array_keys($ngoc_sync_map_records);
  // Get corresponding batch upload objects from SF.
  $batch_upload_sobjects = salesforce_ngoc_get_batch_upload_sobjects($sf_ids);
  // Compare values from SF to those in {salesforce_ngoc_sync_map}.
  if (count($batch_upload_sobjects)) {
    $changed_records = salesforce_ngoc_batch_upload_compare($ngoc_sync_map_records, $batch_upload_sobjects);
    if (count($changed_records)) {
      // Save {salesforce_ngoc_sync_map} records with updated values.
      salesforce_ngoc_sync_map_save_multiple($changed_records);
      // Update polling_attempts and last_polled values. This is bad form to
      // update entity values directly, but it also seems undesirable to load
      // 2,000 entities just to changes these values. This may cause issues with
      // caching, so attempting to flush manually afterwards.
      db_update('salesforce_ngoc_sync_map')
        ->fields(array('lasted_polled' => time()))
        ->expression('polling_attempts', 'polling_attempts + :one', array(':one' => 1))
        ->condition('batch_upload_sfid', $sf_ids, 'IN')
        ->execute();
      entity_get_controller('salesforce_ngoc_sync_map')->resetCache($sf_ids);
      // Allow this and other modules to a change in batch upload objects.
      // @see salesforce_ngoc_salesforce_ngoc_batch_status_change()
      foreach (module_implements('salesforce_ngoc_batch_status_change') as $module) {
        $hook = $module . '_salesforce_ngoc_batch_status_change';
        $hook($batch_upload_sobjects, $changed_records);
      }
    }
  }
  watchdog('salesforce_ngoc', 'Polling job complete. :ngoc_sync_map_records batch upload objects queried for changes. :changed_records changed records found.',
    array(
      ':ngoc_sync_map_records' => count($ngoc_sync_map_records),
      ':changed_records' => count($changed_records)
    ),
    WATCHDOG_NOTICE, l('Batch Upload Report', 'springboard/reports/integration-reports/batch-upload-item'));
}

/**
 * Compares array of salesforce_ngoc_sync_map records to their SObject Objects.
 *
 * @param array $ngoc_sync_map_records
 *   An assoc array of stdClass Objects (as returened by fetchAllAssoc())
 *   containing salesforce_ngoc_sync_map record values, keyed by sf id.
 * @param array $batch_upload_sobjects
 *   An indexed array of SObject Objects as returned from retrieve() method.
 *
 * @return array
 *   An array containing salesforce_ngoc_sync_map records that have changed,
 *   and their new values. Returns empty array if none found.
 */
function salesforce_ngoc_batch_upload_compare($ngoc_sync_map_records, $batch_upload_sobjects) {
  $changed_records = array();
  $compare_fields = array(
    'rC_Connect__Batch_Upload_Status__c' => 'sf_status',
    'rC_Connect__Batch_Upload_Contact_1_Matched__c' => 'contact_sfid',
    'rC_Connect__Batch_Upload_Account_Matched__c' => 'account_sfid',
    'rC_Connect__Batch_Upload_Giving_Matched__c' => 'opportunity_sfid',
    'SB_User_Id__c' => 'uid',
    'SB_Donation_Id__c' => 'did',
  );
  // For each batch_upload object retrieved form SF, see if the values are
  // different than the ones stored in {salesforce_ngoc_sync_map}
  foreach ($batch_upload_sobjects as $batch_upload_sobject) {
    $values_have_changed = FALSE;
    $ngoc_sync_map_record = $ngoc_sync_map_records[$batch_upload_sobject->Id];
    foreach ($compare_fields as $sf_field => $sb_field) {
      $new_ngoc_sync_map_record = $ngoc_sync_map_record;
      if ($batch_upload_sobject->fields->{$sf_field} !== $ngoc_sync_map_record->{$sb_field}) {
        $values_have_changed = TRUE;
        $new_ngoc_sync_map_record->{$sb_field} = $batch_upload_sobject->fields->{$sf_field};
      }
    }
    if ($values_have_changed) {
      $changed_records[] = $new_ngoc_sync_map_record;
    }
  }
  return $changed_records;
}

/**
 * Uses SF's retrieve method to get batch upload SF objects.
 *
 * @param array $sf_ids
 *   An array of salesforce ids to retrieve.
 *
 * @return array
 *   An assoc array containing SObject objects, keyed by the objects SF Id.
 *   Returns empty array if none are found.
 */
function salesforce_ngoc_get_batch_upload_sobjects($sf_ids) {
  $sobjects = array();
  $sf_fields = array(
    'Id',
    'rC_Connect__Batch_Upload_Giving_Matched__c',
    'rC_Connect__Batch_Upload_Account_Matched__c',
    'rC_Connect__Batch_Upload_Contact_1_Matched__c',
    'rC_Connect__Account_Status__c',
    'rC_Connect__Contact1_Status__c',
    'rC_Connect__Opportunity_Status__c',
    'rC_Connect__Payment_Method_Status__c',
    'rC_Connect__Batch_Upload_Status__c',
    'SB_User_Id__c',
    'SB_Donation_Id__c',
  );
  try {
    $sfapi = salesforce_get_api();
    $soap = new SalesforceSoapPartner($sfapi);
    $sobjects = $soap->retrieve(implode(',', $sf_fields), "rC_Connect__Batch_Upload__c", $sf_ids);
  }
  catch (Exception $e) {
    watchdog('salesforce_ngoc', $e->getMessage(), NULL, WATCHDOG_ERROR);
  }
  // Key the array by Salesforce Id to make them easier to access later.
  $keyed_array = array();
  foreach ($sobjects as $sobject) {
    if (isset($sobject->Id)) {
      $keyed_array[$sobject->Id] = $sobject;
    }
  }
  return $keyed_array;
}
