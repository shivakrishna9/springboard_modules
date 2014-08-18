<?php

/**
 * @file Salesforce integration code.
 */

/**
 * Add a social share event to the Salesforce queue.
 *
 * @param $share_id
 * Share id of the share event to add to the queue.
 *
 */
function sb_social_add_share_to_queue($share_id, $op = 'CREATE', $id = NULL) {
  module_load_include('module', 'salesforce_queue');
  $share = sb_social_share_event_load($share_id);
  $sobject = sb_social_create_share_event_object($share);

  $item = array(
    'drupal_id' => $share_id,
    'module' => 'sb_social',
    'delta' => 'share_event',
    'object_type' => 'Social_Share__c',
    'operation' => $op,
    'sobject' => $sobject,
  );
  if ($op == 'UPDATE' && !empty($id)) {
    $item['Id'] = $id;
  }
  $queue = salesforce_queue_load();
  $result = $queue->createItem($item);
}

/**
 * Generate a Salesforce object to sync a social share event
 *
 * @param $share
 * Share record
 *
 * @return stdClass
 * Returns SF object.
 */
function sb_social_create_share_event_object($share) {
  // Create a fields array.
  $name = '';
  $fields = array(
    'Contact_sharer__c' => !empty($share['uid']) ? sb_social_create_sf_user_token($share['uid']) : '',
    'Market_Source__c' => $share['market_source'],
    'Shared_Page_Drupal_ID__c' => $share['nid'],
    'Social_Network__c' => $share['service'],
    'Share_Date__c' => sb_social_format_sf_date($share['created']),
    'Shared_Page_Name__c' => $share['page_name'],
    'Shared_Page_Name_Internal__c' => $share['internal_name'],
    'Share_URL__c' => $share['url'],
    'Social_Share_ID__c' => $share['share_id'],
    'Social_Share_Type__c' => $share['type'],
    'Submission_ID__c' => $share['sid'],
  );

  // By default use the page name for the sobject name.
  $object_name = $share['page_name'];

  // conditionally add the opportunity id if this share is related to a donation.
  if (is_numeric($share['did']) && $share['did']) {
    $donation = fundraiser_donation_get_donation($share['did']);
    // Reset the sobject name; on donations we can include the first and last name.
    if (isset($donation->donation['first_name']) && isset($donation->donation['last_name'])) {
      $object_name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
    }

    $fields['Opportunity_ID__c'] = '[Opportunity:donation:' . $share['did'] . ']';
  }

  // Set the name value on the sobject.
  $fields['Name'] = 'Social Share - ' . substr($object_name, 0, 40) . ' (' . gmdate('Y-m-d H:i:s\Z', time()) . ')';

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
