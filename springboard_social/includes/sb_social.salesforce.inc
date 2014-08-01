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
 * Replace Drupal objects either with IDs or with tokens for associated objects in exported 
 * social share items, depending on what should be mapped to the SF field.
 */
function sb_social_salesforce_mapping_map_fields_alter(&$fields, $context) {
  if ($context['entity_type'] == 'sb_social_share') {
    $wrapper = $context['entity_wrapper'];
    foreach ($context['map']->field_mappings as $field_mapping) {
      $drupal_name = $field_mapping['drupal_field']['fieldmap_value'];
      $sf_name = $field_mapping['salesforce_field']['name'];
      switch ($drupal_name) {
        case 'user':
          // Give Contact SFID replacement token for a reference field, or Drupal account UID otherwise.
          $account = $wrapper->user->value();
          if ($account->uid && 'reference' == $field_mapping['salesforce_field']['type']) {
            $fields[$sf_name] = sprintf('[Contact:user:%d]', $account->uid);
          } else {
            $fields[$sf_name] = $account->uid;
          }
          break;
        case 'donation':
          // Give Opportunity SFID replacement token for a reference field, or Fundraiser donation ID otherwise.
          $donation = $wrapper->donation->value();
          if ($donation && 'reference' == $field_mapping['salesforce_field']['type']) {
            $fields[$sf_name] = sprintf('[Opportunity:donation:%d]', $donation->did);
          } else {
            $fields[$sf_name] = $donation ? $donation->did : NULL;
          }
          break;
        case 'submission':
          // Give Action__c SFID replacement token for a reference field, IF the Webform of the submission
          // has a map to Action__c, or give the Webform submission ID otherwise.
          $submission = $wrapper->submission->value();
          if ('reference' == $field_mapping['salesforce_field']['type']) {
            if ($submission && _sb_social_webform_maps_to_action($submission->nid)) {
              $fields[$sf_name] = sprintf('[Actions__c:%d:%d]', $submission->nid, $submission->sid);
            } else {
              // If we can't give a token, give nothing. A number won't sync to a reference field.
              $fields[$sf_name] = NULL;
            }
          } else {
            $fields[$sf_name] = $submission ? $submission->sid : NULL;
          }
          break;
      }
    }
  }
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

/**
 * Utility function: Query genmap whether a Webform has a map to the Actions__c object.
 */
function _sb_social_webform_maps_to_action($nid) {
  static $result = array();
  // This is a moot point if genmap isn't enabled to create Webform mappings.
  if (!module_exists('salesforce_genmap')) {
    return FALSE;
  }
  // For performance, query the genmap mappings table directly, then cache.
  if (!isset($results[$nid])) {
    // We are specifically interested in whether this webform maps to Actions__c, irrespective of
    // what module is the map's handler.
    $results[$nid] = (bool) db_query(
      "SELECT mid FROM {salesforce_genmap_map} WHERE salesforce_object_type='Actions__c' AND nid=:nid;",
      array(':nid' => $nid)
    )->fetchField();
  }
  return $results[$nid];
}
