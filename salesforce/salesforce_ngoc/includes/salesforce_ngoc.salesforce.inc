<?php
/**
 * @file
 * Implementations of salesforce-related hooks and related helper functions.
 */

/**
 * Implements hook_salesforce_genmap_destination_queue_alter().
 */
function salesforce_ngoc_salesforce_genmap_destination_queue_alter(&$queue_name, $item, $drupal_object) {
  if ($parents = salesforce_ngoc_has_unsyncd_parents($item['object_type'], $drupal_object)) {
    $queue_name = 'hold';
    salesforce_ngoc_write_hold_queue_items($item, $parents);
  }
}

/**
 * Implements hook_salesforce_genmap_map_fields_alter().
 */
function salesforce_ngoc_salesforce_genmap_map_fields_alter(&$fields, $context) {
  // The batch upload object needs some system flags set correct in order
  // to do it's thing. Let's do that here instead of implementing in the
  // mapping interface because these flags have no donation counterpart.
  if ($context['module'] == 'salesforce_donation' && $context['map']->salesforce_object_type == 'rC_Connect__Batch_Upload__c') {
    $fields['rC_Connect__Giving_Paid__c'] = TRUE;
    $fields['rC_Connect__Address_Type__c'] = variable_get('salesforce_ngoc_default_address_type', SALESFORCE_NGOC_DEFAULT_ADDRESS_TYPE);
    $fields['rC_Connect__Giving_Payment_Frequency__c'] = variable_get('salesforce_ngoc_default_frequency', SALESFORCE_NGOC_DEFAULT_FREQUENCY);
    // Prevents NGOC from trying to process the payment.
    $fields['rC_Connect__Disable_Merchant_Messages__c'] = TRUE;
    // Depends on one-time or recurring gift.
    $fields['rC_Connect__Giving_Giving_Frequency__c'] = 'One Payment';
    $fields['rC_Connect__Giving_Record_Type__c'] = variable_get('salesforce_ngoc_default_record_type', SALESFORCE_NGOC_DEFAULT_RECORD_TYPE);
    // Per tom: "for online donations the Address: Preferred Billing and
    // Address: Preferred Shipping should be set to TRUE.  (For advocacy
    // actions these should be set to FALSE.)" which appears to be the default
    // so not explicitly setting FALSE for those.
    $fields['rC_Connect__Address_Preferred_Shipping__c'] = TRUE;

    // Set fields that change depending on payment method type.
    switch ($fields['rC_Connect__Payment_Method_Payment_Type__c']) {
      case 'credit':
        $fields['rC_Connect__Payment_Method_Payment_Type__c'] = 'Charge Card';
        break;

      case 'bank account':
        $fields['rC_Connect__Payment_Method_Payment_Type__c'] = 'EFT';
        break;

      case 'paypal':
        $fields['rC_Connect__Payment_Method_Payment_Type__c'] = 'Paypal';
        break;
    }

  }
}

/**
 * Implements hook_salesforce_mapping_confirm_entity_for_queue().
 */
function salesforce_ngoc_salesforce_mapping_confirm_entity_for_queue($entity, $entity_type, $op){
  // Suppress creation of user salesforce sync item if the user was created by
  // submitting a donation form because the user data is included in the
  // batch_upload salesforce sync item.
  if ($entity_type == 'user') {
    $wrapper = entity_metadata_wrapper('user', $entity);
    $created_via = $wrapper->sbp_created_via->value();
    if (!empty($created_via) && fundraiser_is_donation_type($created_via)) {
      return FALSE;
    }
  }
}

/**
 * Implements hook_salesforce_mapping_destination_queue_alter().
 */
function salesforce_ngoc_salesforce_mapping_destination_queue_alter(&$queue_name, $item, $entity) {
  if ($parents = salesforce_ngoc_has_unsyncd_parents($item['object_type'], $entity)) {
    $queue_name = 'hold';
    salesforce_ngoc_write_hold_queue_items($item, $parents);
  }
}

/**
 * Implements hook_salesforce_sync_save_map_alter().
 */
function salesforce_ngoc_salesforce_sync_save_map_alter($record) {
  if ($record->object_type == 'rC_Connect__Batch_Upload__c') {
    if ($record->is_new == TRUE) {
      // Save record in salesforce_ngoc_sync_map.
      $ngoc_record = array(
        'batch_upload_sfid' => $record->sfid,
        'sf_status' => 'New',
        'drupal_id' => $record->drupal_id,
        'module' => $record->module,
        'delta' => $record->delta,
        'created' => REQUEST_TIME,
      );
      salesforce_ngoc_sync_map_save($ngoc_record);
      // Prevents batch_upload record from being saved to salesforce_sync_map.
      $record->prevent_save = TRUE;
    }
  }
}

/**
 * Determine if item has "parent" records that have not been processed in SF.
 *
 * Salesforce queue items of records that reference "parent" objects which are
 * created via a batch_upload object (Contact and Opportunity) must go in
 * the hold queue until such time that the batch_upload object has been
 * processed and the Contact and/or Opportunity objects have been created. This
 * allows the child objects to reference the parent objecs using the parent's
 * sfid.
 *
 * We determine that those parents have been processed by looking for them in
 * the salesforce_sync_map table.
 *
 * Objects that have parent Contact and Opportunity objects are:
 * - sb_Actions_Taken__c
 * - Social_Share__c
 * - Donation_Upsell__c
 * - P2P_Personal_Campaign__c
 * - Sustainers_Upgrade__c
 *
 * @param string $object_type
 *   The type of salesforce object (eg 'Social_Share__c').
 * @param object $drupal_object
 *   An object or drupal entity containing data for finding parent records.
 *
 * @return array
 */
function salesforce_ngoc_has_unsyncd_parents($object_type, $drupal_object){
  $unsyncd_parents_found = array();
  $salesforce_ngoc_parent_objects = variable_get('salesforce_ngoc_parent_objects');
  // If this is an object type that has parents that need to exist first.
  if (isset($salesforce_ngoc_parent_objects[$object_type])) {
    foreach ($salesforce_ngoc_parent_objects[$object_type] as $parent_object_type => $delta) {
      // Find the drupal_id of the parent object. It can be a uid or a did.
      $parent_drupal_id = 0;
      if ($parent_object_type == 'Contact') {
        if (!empty($drupal_object->uid)) {
          $parent_drupal_id = $drupal_object->uid;
        }
      }
      elseif ($parent_object_type == 'Opportunity') {
        if (!empty($drupal_object->uid)) {
          $parent_drupal_id = $drupal_object->did;
        }
      }
      // Look for the parent object in salesforce_sync_map.
      if (!empty($parent_drupal_id) && !empty($delta) && !empty($parent_object_type)) {
        $result = db_query('SELECT sfid FROM {salesforce_sync_map} WHERE
          object_type = :parent_object_type AND
          delta = :delta AND
          drupal_id = :drupal_id',
          array(
            ':parent_object_type' => $parent_object_type,
            ':delta' => $delta,
            ':drupal_id' => $parent_drupal_id,
          )
        );
        // If ZERO rows were returned, then the parent object must not have
        // been created by the batch_upload process yet.
        if ($result->rowCount() == 0) {
          $unsyncd_parents_found[] = array($parent_object_type => $parent_drupal_id);
        }
      }
      else {
        // $parent_drupal_id is not found. Making a BIG assumption here: We
        // assume that this is missing because the webform submission was just
        // saved, and webform_user_webform_submit() will come by and update it
        // in a moment to change the webform_sumission record uid to that of the
        // newly created user by calling webform_submission_update(). That
        // update causes salesforce_genmap_webform_submission_update() to fire
        // and this record to re-queue. However,
        // once salesforce_genmap_send_object_to_queue() instantiates $queue,
        // the queue name cannot be changed. We have to get it right the first
        // time.
        // This would be unnecessary if webform_user_webform_submit() didn't
        // create and then update webform_submission records in the same
        // request.
        $unsyncd_parents_found[] = array($parent_object_type => $parent_drupal_id);
      }
      unset($result);
    }
  }
  return $unsyncd_parents_found;
}

/**
 * Records which hold queue items have parent items pending processing in SF.
 *
 * @param array $item
 *   A salesforce_mapping or salesforce_genmap queue item.
 * @param array $parent_records
 */
function salesforce_ngoc_write_hold_queue_items($item, $parent_records) {
  foreach ($parent_records as $parent_record) {
    reset($parent_record);
    $key = key($parent_record);
    $record = array(
      'drupal_id' => $item['drupal_id'],
      'object_type' => $item['object_type'],
      'parent_type' => $key,
      'parent_drupal_id' => $parent_record[$key],
    );
    drupal_write_record('salesforce_ngoc_hold_queue_items', $record);
  }
}
