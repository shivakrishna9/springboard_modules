<?php

/**
 * @file
 * Functions for rules integration.
 */

/**
 * Implementation of hook_rules_action_info()
 */
function salesforce_management_api_rules_action_info() {
  return array(
    'salesforce_management_api_delete_sync_data' => array(
      'label' => t('Wipe Salesforce data'),
      'module' => 'Salesforce',
      'arguments' => array(
        'type' => array(
          'type' => 'value',
          'label' => t('The type of Drupal object to clear'),
        ),
        'oid' => array(
          'type' => 'value',
          'label' => t('The id of the Drupal object to clear'),
        ),
      ),
    ),
  );
}

/**
 * Rules action to delete Salesforce map for an object.
 *
 * @param $type
 *   The Drupal object type.
 * @param $oid
 *   The id of the Drupal object.
 */
function salesforce_management_api_delete_sync_data($type, $oid) {
  db_query("DELETE FROM {salesforce_management_object_map} WHERE drupal_type = '%s' AND oid = '%s'", $type, $oid);
}