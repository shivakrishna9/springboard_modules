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
  // salesforce_ngoc_release_children_from_hold_queue($batch_upload_sobjects, $changed_records);
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
 * [salesforce_ngoc_release_children_from_hold_queue description]
 * @param  [type] &$batch_upload_sobjects [description]
 * @param  [type] &$changed_records       [description]
 * @return [type]                         [description]
 */
function salesforce_ngoc_release_children_from_hold_queue(&$batch_upload_sobjects, &$changed_records) {

}

/**
 * Queries SF batch_upload objects for change in processing status.
 */
function salesforce_ngoc_poll_batch_upload() {
  // Get records from {salesforce_ngoc_sync_map}, oldest first.
  $ngoc_sync_map_records = db_query_range("SELECT * FROM {salesforce_ngoc_sync_map} WHERE sf_status != 'Committed' ORDER BY created DESC", 0, variable_get('salesforce_ngoc_poll_record_limit', SALESFORCE_NGOC_DEFAULT_POLL_RECORD_LIMIT))->fetchAllAssoc('batch_upload_sfid');
  $sf_ids = array_keys($ngoc_sync_map_records);
  // Get corresponding batch upload objects from SF.
  $batch_upload_sobjects = salesforce_ngoc_get_batch_upload_sobjects($sf_ids);
  // Compare values from SF to those in {salesforce_ngoc_sync_map}.
  if (count($batch_upload_sobjects)) {
    $changed_records = salesforce_ngoc_batch_upload_compare($ngoc_sync_map_records, $batch_upload_sobjects);
  }
  // Save {salesforce_ngoc_sync_map} records with updated values.
  salesforce_ngoc_sync_map_save_multiple($changed_records);
  // Update polling_attempts and last_polled values. This is bad form to update
  // entity values directly, but it also seems undesirable to load 2,000
  // entities just to changes these values. This may cause issues with caching,
  // so attempting to flush manually afterwards.
  db_update('salesforce_ngoc_sync_map')
    ->fields(array('lasted_polled' => time()))
    ->expression('polling_attempts', 'polling_attempts + :one', array(':one' => 1))
    ->condition('batch_upload_sfid', $sf_ids, 'IN')
    ->execute();
  entity_get_controller('salesforce_ngoc_sync_map')->resetCache($sf_ids);

  // Allow this and other modules to react.
  // @see salesforce_ngoc_salesforce_ngoc_batch_status_change()
  foreach (module_implements('salesforce_ngoc_batch_status_change') as $module) {
    $hook = $module . '_salesforce_ngoc_batch_status_change';
    $hook($batch_upload_sobjects, $changed_records);
  }
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
 *   and their new values.
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
    $keyed_array[$sobject->Id] = $sobject;
  }
  return $keyed_array;
}
