<?php

/**
 * @file Salesforce integration code.
 */

/**
 * Implements hook_salesforce_mapping_map_fields_alter().
 *
 * Replace Drupal objects either with IDs or with tokens
 * for associated objects in exported
 * social share items, depending on what should be mapped to the SF field.
 */
function sb_social_salesforce_mapping_map_fields_alter(&$fields, $context) {
  // Handle mappings of sb_social_share entities.
  if ($context['entity_type'] == 'sb_social_share') {
    $wrapper = $context['entity_wrapper'];
    foreach ($context['map']->field_mappings as $field_mapping) {
      $drupal_name = $field_mapping['drupal_field']['fieldmap_value'];
      $sf_name = $field_mapping['salesforce_field']['name'];
      switch ($drupal_name) {
        case 'user':
          // Give Contact SFID replacement token for a reference field,
          // or Drupal account UID otherwise.
          $account = $wrapper->user->value();
          if ($account && $account->uid && 'reference' == $field_mapping['salesforce_field']['type']) {
            $fields[$sf_name] = sprintf('[Contact:user:%d]', $account->uid);
          }
          else {
            $fields[$sf_name] = $account ? $account->uid : NULL;
          }
          break;

        case 'donation':
          // Give Opportunity SFID replacement token for a reference field,
          // or Fundraiser donation ID otherwise.
          $donation = $wrapper->donation->value();
          if ($donation && 'reference' == $field_mapping['salesforce_field']['type']) {
            $fields[$sf_name] = sprintf('[Opportunity:donation:%d]', $donation->did);
          }
          else {
            $fields[$sf_name] = $donation ? $donation->did : NULL;
          }
          break;

        case 'submission':
          // Give Action__c SFID replacement token for a reference field,
          // IF the Webform of the submission has a map to Action__c,
          // or give the Webform submission ID otherwise.
          $submission = $wrapper->submission->value();
          if ('reference' == $field_mapping['salesforce_field']['type']) {
            if ($submission && _sb_social_webform_maps_to_action($submission->nid)) {
              $fields[$sf_name] = sprintf('[sb_Actions_Taken__c:%d:%d]', $submission->nid, $submission->sid);
            }
            else {
              // If we can't give a token, give nothing.
              // A number won't sync to a reference field.
              $fields[$sf_name] = NULL;
            }
          }
          else {
            $fields[$sf_name] = $submission ? $submission->sid : NULL;
          }
          break;

        case 'share_date':
          // Format timestamp for Salesforce API.
          $timestamp = $wrapper->share_date->value();
          $fields[$sf_name] = $timestamp ? sb_social_format_sf_date($timestamp) : NULL;
          break;
      }
    }
  }
  // Handle mappings of users.
  if ($context['entity_type'] == 'user') {
    foreach ($context['map']->field_mappings as $field_mapping) {
      $drupal_name = $field_mapping['drupal_field']['fieldmap_value'];
      $sf_name = $field_mapping['salesforce_field']['name'];
      switch ($drupal_name) {
        case 'sbp_social_originating_share':
          // Replace share_id with Social_Share__c SFID token
          // if SF field is a reference field.
          if ('reference' == $field_mapping['salesforce_field']['type']) {
            if ($fields[$sf_name] && $share = sb_social_share_event_load($fields[$sf_name])) {
              $fields[$sf_name] = sprintf('[Social_Share__c:sb_social_share:%d]', $share['share_id']);
            }
            else {
              // Drop this field if there isn't a valid share ID.
              unset($fields[$sf_name]);
            }
          }
          break;
      }
    }
  }
}

/**
 * Implements hook_salesforce_genmap_map_fields_alter().
 *
 * If a donation or other Webform submission was created by a user who came to
 * the site  from a share URL (e.g., social_referer_transaction market
 * source field was set) we want to try to link the new
 * donation/submission to the "referer transaction" -- that is, the
 * social share that produced this new user action.
 *
 * The"social_referer_transaction" value will be populated already by
 * market_source,  giving the numeric share_id. To sync the relationship
 * to Salesforce, we replace the  share_id value with a token so the sync
 * engine can look up the Social_Share__c object's  SFID.
 */
function sb_social_salesforce_genmap_map_fields_alter(&$fields, $context) {
  // Is this a webform-based mapping (donation, petition, etc.)?
  if (isset($context['map']->field_map['salesforce_webform_map']) || isset($context['map']->field_map['webform_map'])) {
    $webform_map = (isset($context['map']->field_map['webform_map']) ? $context['map']->field_map['webform_map'] : array())
      + (isset($context['map']->field_map['salesforce_webform_map']) ? $context['map']->field_map['salesforce_webform_map'] : array());
    foreach ($webform_map as $drupal_name => $sf_name) {
      switch ($drupal_name) {
        case 'social_referer_transaction':
          // Give Social_Share__c SFID replacement token if there's a share_id,
          // null otherwise.
          if (isset($fields[$sf_name]) && $share = sb_social_share_event_load($fields[$sf_name])) {
            if ($context['object']->nid == $share['nid'] && $context['object']->sid == $share['sid']) {
              // Extreme edge case: if the refering share is somehow
              // itself a share of this submission,
              // then remove the field and log an error.
              unset($fields[$sf_name]);
              watchdog(
                'sb_social',
                'Impossible self-reference! Submission ID %sid is both the referrer and the referral of share ID %share_id.',
                array('%sid' => $share['sid'], '%share_id' => $share['share_id']),
                WATCHDOG_ERROR
              );
            }
            else {
              $fields[$sf_name] = sprintf('[Social_Share__c:sb_social_share:%d]', $share['share_id']);
            }
          }
          break;
      }
    }
  }
}

/**
 * Implements hook_salesforce_queue_create_item_alter().
 */
function sb_social_salesforce_queue_create_item_alter(&$item) {
  // If Originating Social Share is mapped but the value is 0, remove the field
  // from the queue item. We can't sync a social share value for an anon user.
  if (isset($item['sobject']->fields['Originating_Social_Share__c']) && $item['sobject']->fields['Originating_Social_Share__c'] == 0) {
    unset($item['sobject']->fields['Originating_Social_Share__c']);
  }
}

/**
 * Helper: lookup and return the social share Salesforce mapping, with caching.
 */
function _sb_social_salesforce_get_share_mapping() {
  static $mappings;
  if (!isset($mappings)) {
    $mappings = salesforce_mapping_load_multiple(
      array('drupal_bundle' => 'sb_social_share', 'salesforce_object_type' => 'Social_Share__c')
    );
  }
  return reset($mappings);
}


/**
 * Generate a Salesforce object to sync a social share event.
 *
 * @param array $share
 *   Share record.
 *
 * @return object
 *   Returns SF object.
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
    'Client_ID__c' => empty($share['client_id']) ? '' : $share['client_id'],
    // @TODO: What if the SF instance isn't configured with this field?
  );

  // Conditionally add the opportunity id if this share is related to a donation.
  if (is_numeric($share['did']) && $share['did']) {
    $donation = fundraiser_donation_get_donation($share['did']);
    if (isset($donation->donation['first_name']) && isset($donation->donation['last_name'])) {
      $name = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
    }
    $fields['Opportunity_ID__c'] = '[Opportunity:donation:' . $share['did'] . ']';
  }
  // Set the Name field for the share.
  // On donations we can include the first and last name.
  $fields['Name'] = 'Social Share - ' . $name . ' (' . date('Y-m-d H:i:se', time()) . ')';
  // This is SOAP partner API format.
  $sobject = new stdClass();
  $sobject->type = 'Social_Share__c';
  $sobject->fields = $fields;

  return $sobject;
}

/**
 * Generate a Salesforce token from user id.
 *
 * @param int $uid
 *   The user id.
 *
 * @return string
 *   The user id token.
 */
function sb_social_create_sf_user_token($uid) {
  return sprintf('[Contact:user:%d]', $uid);
}

/**
 * Convert unix timestamp to date format accepted by Salesforce.
 *
 * @param string $time
 *
 * @return string
 *   Date.
 */
function sb_social_format_sf_date($time) {
  return date('Y-m-d H:i:s', $time);
}

/**
 * Utility function.
 *
 * Query genmap whether a Webform has a map to
 * the sb_Actions_Taken__c object.
 */
function _sb_social_webform_maps_to_action($nid) {
  static $result = array();
  // This is a moot point if genmap isn't enabled to create Webform mappings.
  if (!module_exists('salesforce_genmap')) {
    return FALSE;
  }
  // For performance, query the genmap mappings table directly, then cache.
  if (!isset($results[$nid])) {
    // We are specifically interested in whether this webform maps to
    // sb_Actions_Taken__c, irrespective of
    // what module is the map's handler.
    $results[$nid] = (bool) db_query(
      "SELECT mid FROM {salesforce_genmap_map} WHERE salesforce_object_type='sb_Actions_Taken__c' AND nid=:nid;",
      array(':nid' => $nid)
    )->fetchField();
  }
  return $results[$nid];
}

