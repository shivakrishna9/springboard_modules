<?php

/**
 * @file
 * Functions for rules integration.
 */

/**
 * Implementation of hook_rules_action_info().
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
 * Implementation of hook_rules_event_info().
 */
function salesforce_management_api_rules_event_info() {
  return array(
    'salesforce_management_api_soapfault' => array(
      'label' => t("An object can't be synced due to a SOAP fault"),
      'module' => 'Salesforce',
      'arguments' => array(
        'drupal_type' => array(
          'type' => 'string',
          'label' => t("The Drupal type of the object that can't be synced"),
        ),
        'sf_type' => array(
          'type' => 'string',
          'label' => t("The Salesforce type of the object that can't be synced"),
        ),
        'sfid' => array(
          'type' => 'string',
          'label' => t("The Salesforce Id of the object that can't be exported"),
        ),
        'message' => array(
          'type' => 'string',
          'label' => t("The SOAP fault error message"),
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
