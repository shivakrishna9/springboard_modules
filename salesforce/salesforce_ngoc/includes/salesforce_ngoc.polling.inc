<?php
/**
 * @file
 * Functions for polling SF for changes to batch_upload processing status.
 */

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
 * Uses SF's retrieve method to get get batch upload SF objects.
 *
 * @param array $sf_ids
 *   An array of salesforce ids to retrieve.
 *
 * @return array
 *   An array containing SObject objects
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
  return $sobjects;
}
