<?php

/**
 * @file Salesforce integration code.
 */

/**
 * Implements hook_salesforce_queue_create_item_alter().
 * 
 * If the donation was created by a user who came to the site from a share
 * URL (social_referrer_transaction market source field set) we want to
 * try to add the sfid of the original share event to the opportunity as it's
 * going into the queue.
 * 
 * If the original share event hasn't been synced yet (no sfid) we go ahead and
 * sync the opportunity anyway, when the share event syncs we'll requeue any
 * applicable donations.
 */
function sb_social_salesforce_queue_create_item_alter(&$item) {
  if ($item['object_type'] == 'Opportunity') {
    if (!empty($item['sobject']->fields['Social_Referrer_Transaction__c'])) {
      $sfid = _sb_social_get_share_salesforce_id_from_map($item['sobject']->fields['Social_Referrer_Transaction__c']);
      // The original share event has synced at some point,
      // requeue with the sfid for the share event.
      if ($sfid) {
        $item['sobject']->fields['Social_Share__c'] = $sfid;
      }
    }
  }
}

/**
 * Implements hook_salesforce_mapping_map_fields_alter().
 * 
 * Replace Drupal object IDs with tokens for associated Contact and Opportunity objects
 * in exported social share items.
 */
function sb_social_salesforce_mapping_map_fields_alter(&$fields, $context) {
  if ($context['entity_type'] == 'sb_social_share') {
    foreach ($context['map']->field_mappings as $field_mapping) {
      $drupal_name = $field_mapping['drupal_field']['fieldmap_value'];
      $sf_name = $field_mapping['salesforce_field']['name'];
      switch ($drupal_name) {
        case 'uid':
          // Replace non-zero uid with Contact token.
          if (!empty($fields[$sf_name]) && 'reference' == $field_mapping['salesforce_field']['type']) {
            $fields[$sf_name] = sprintf('[Contact:user:%d]', (int)$fields[$sf_name]);
          }
          break;
        case 'did':
          if ('reference' == $field_mapping['salesforce_field']['type']) {
            // If zero then set to null.
            if ($fields[$sf_name] == 0) {
              $fields[$sf_name] = NULL;
            }
            // Otherwise, replace did with Opportunity token.
            else {
              $fields[$sf_name] = sprintf('[Opportunity:donation:%d]', (int)$fields[$sf_name]);
            }
          }
          break;
      }
    }
  }
}

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
    $donation = fundraiser_donation_get_donation($share['did']);
    if (isset($donation->donation['first_name']) && isset($donation->donation['last_name'])) {
      $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
    }
    $fields['Opportunity_ID__c'] = '[Opportunity:donation:' . $share['did'] . ']';
  }
  // set the Name field for the share. On donations we can include the first and last name.
  $fields['Name'] = 'Social Share - ' . $name . ' (' . date('Y-m-d H:i:se', time()) . ')';
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
