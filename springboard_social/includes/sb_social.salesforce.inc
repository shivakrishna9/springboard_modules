<?php

/**
 * @file Salesforce integration code.
 */

function sb_social_add_share_to_queue($share_id) {
  module_load_include('module', 'salesforce_queue');
  $share = sb_social_share_event_load($share_id);
  $sobject = sb_social_create_share_event_object($share);

  $item = array(
      'drupal_id' => $share_id,
      'module' => 'sb_social',
      'delta' => 'share_event',
      'object_type' => 'Social_Share__c',
      'operation' => 'CREATE',
      'sobject' => $sobject,
  );

  $queue = salesforce_queue_load();
  $result = $queue->createItem($item);
}

function sb_social_create_share_event_object($share) {
    // Create a fields array.
  $fields = array(
    'Contact__c' => sb_social_create_sf_user_token($share['uid']),
    // 'Market_Source__c' => $share['market_source'],
    'Market_Source_Type__c' => '', // ???
    'Network__c' => $share['service'],
    'Share_Date__c' => sb_social_format_sf_date($share['created']),
    'shared_name__c' => '', // ???
    'Shared_URL__c' => $share['url'],
    'Submission_ID__c' => $share['sid'],
    'Total_Value_of_Social_Shares__c' => 0, // ???
  );

  // This is SOAP partner API format.
  $sobject = new stdClass();
  $sobject->type = 'Social_Share__c'; // sf object type

  $sobject->fields = $fields;

  return $sobject;
}

/**
 * Generate a Salesforce token from user id.
 *
 * @param type $uid
 */
function sb_social_create_sf_user_token($uid) {
  return sprintf('[Contact:user:%d]', $uid);
}

/**
 * Convert unix timestamp to date format accepted by Salesforce.
 * @param type $time
 */
function sb_social_format_sf_date($time) {
  return date('Y-m-d H:i:s', $time);
}