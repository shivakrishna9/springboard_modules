<?php
/**
 * @file
 * Implementations of fundraiser_sustainer hooks and related helper functions.
 */

function salesforce_ngoc_fundraiser_sustainers_upgrade_success($master_did, $amount) {
  dpm('master_did');
  dpm($master_did);
  dpm('amount');
  dpm($amount);
  watchdog('ngoc', 'upgrade success was triggered');

  $donation = fundraiser_donation_get_donation($master_did);
  dpm('donation');
  dpm($donation);

  // Create a batch upload object and queue it up.
  // $sobject = new stdClass();
  // $sobject->type = 'rC_Connect__Batch_Upload__c';
  // $sobject->fields = array(
  //   'rC_Connect__Batch_Upload_Giving_Matched__c' => ,
  //   'Upgrade_Amount__c' => ,
  //   'Payment_Token__c' => ,
  // );

  // $item = array(
  //   'drupal_id' => ,
  //   'module' => 'salesforce_quick_donate',
  //   'delta' => 'commerce_cardonfile',
  //   'object_type' => 'rC_Connect__Batch_Upload__c',
  //   'operation' => 'CREATE',
  //   'sobject' => $sobject,
  // );

  // $queue = salesforce_queue_load();
  // $result = $queue->createItem($item);
}
