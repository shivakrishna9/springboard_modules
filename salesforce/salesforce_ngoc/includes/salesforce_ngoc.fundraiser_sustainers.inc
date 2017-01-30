<?php
/**
 * @file
 * Implementations of fundraiser_sustainer hooks and related helper functions.
 */

function salesforce_ngoc_fundraiser_sustainers_upgrade_success($master_did, $upgrade_form_values) {
  $donation = fundraiser_donation_get_donation($master_did);
  $components = _fundraiser_webform_submission_flatten($donation->nid, $donation->node->webform['components']);
  $amount = commerce_currency_amount_to_decimal($upgrade_form_values['fsu_amount'], $donation->currency);
  // Create a batch upload object and queue it up.
  $sobject = new stdClass();
  $sobject->type = 'rC_Connect__Batch_Upload__c';
  $sobject->fields = array(
    'rC_Connect__Batch_Upload_Giving_Matched__c' => sprintf('[Opportunity:donation:%d]', $master_did),
    'rC_Connect__Batch_Upload_Account_Matched__c' => sprintf('[Account:User:%d]', $donation->uid),
    'rC_Connect__Batch_Upload_Campaign_Matched__c' => $upgrade_form_values['cid'],
    'Upgrade_Amount__c' => $amount,
    'rC_Connect__Giving_Giving_Amount__c' => $amount,
    'Upgrade_Source__c' => $upgrade_form_values['cid'],
    'rC_Connect__Giving_Is_Sustainer__c' => TRUE,
    'rC_Connect__Giving_Close_Date__c' => date('Y-m-d'),
    'rC_Connect__Giving_Payment_Frequency__c' => variable_get('salesforce_ngoc_default_frequency', SALESFORCE_NGOC_DEFAULT_FREQUENCY),
    'rC_Connect__Giving_Giving_Type__c' => 'Pledge',
  );

  $item = array(
    'drupal_id' => $master_did,
    'module' => 'sustainer_upgrade',
    'delta' => 'ngoc',
    'object_type' => 'rC_Connect__Batch_Upload__c',
    'operation' => 'CREATE',
    'sobject' => $sobject,
  );

  $queue = salesforce_queue_load();
  $result = $queue->createItem($item);
  if ($result) {
    watchdog('salesforce_ngoc', 'rC_Connect__Batch_Upload__c queue item was created for a sustainer upgrade of series :series', array(':series' => $master_did));
  }
  else {
    watchdog('salesforce_ngoc', 'There was a problem creating a rC_Connect__Batch_Upload__c queue item for a sustainer upgrade of series :series', array(':series' => $master_did), WATCHDOG_ERROR);
  }
}
