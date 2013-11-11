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
  $fields = array(
    'Contact_sharer__c' => sb_social_create_sf_user_token($share['uid']),
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

  // conditionally add the opportunity id if this share is related to a donation.
  if (is_numeric($share['did']) && $share['did']) {
    // this will usually return 0 but covers the highly unlikely event where the opportunity
    // has synced before the user shares.
    if ($opportunity_id = _sb_social_get_opportunity_id_from_submission_id($share['sid'])) {
      $fields['Opportunity_ID__c'] = $opportunity_id;
    }
  }

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

function sb_social_salesforce_requeue_related_shares($item, $result) {
  $related_shares = sb_social_get_shares_related_to_donation($item->drupal_id);

  foreach ($related_shares as $share) {
    dsm($share, 'share requeuing');
    // if the share has a sfid requeue, otherwise delete dupes in queue and recreate
    if (!empty($share['sf_share_id'])) {
      sb_social_add_share_to_queue($share['share_id'], 'UPDATE', $id = $share['sf_share_id']);
    }
    else {
      // item hasn't synced yet, nuke it and rebuild.
      db_query('DELETE FROM {salesforce_queue WHERE drupal_id = :share_id AND module = :module AND delta = :delta}', array(':share_id' => $share['share_id'], ':module' => 'sb_social', ':delta' => 'share_event'));
      sb_social_add_share_to_queue($share['share_id']);
    }
  }
}