<?php

/**
 * @file
 * Manages webform to Salesforce mappings.
 */

/**
 */
function _fundraiser_sustainers_upgrade_salesforce_install() {

  $upgrade_mapping = entity_import('salesforce_mapping', fundraiser_sustainers_upgrade_sfobject_fieldmap());
  entity_save('salesforce_mapping', $upgrade_mapping);
  fundraiser_sustainers_upgrade_webform_fieldmap();

}

/**
 * Implements hook_uninstall().
 */
function fundraiser_sustainers_upgrade_salesforce_uninstall() {

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'salesforce_genmap_map');
  $query->propertyCondition('module', 'fundraiser_sustainers_upgrade');
  $query->propertyCondition('nid', -40); // This should use the constant, but it doesn't work.
  $result = $query->execute();
  if (!empty($result)) {
    $mids = array_keys($result['salesforce_genmap_map']);
    entity_delete('salesforce_genmap_map', $mids[0]);
  }

  $result = salesforce_mapping_load_multiple(array('drupal_bundle' => 'sustainers_upgrade_form'));
  $mapping = array_shift($result);
  entity_delete('salesforce_mapping', $mapping->salesforce_mapping_id);
}


function fundraiser_sustainers_upgrade_webform_fieldmap() {
  // Create default SF map insert into table.
  $map = array();
  $map = entity_create('salesforce_genmap_map', $map);
  $map->nid = -40;
  $map->map_handler = 'webform';
  $map->salesforce_object_type = 'Sustainers_Upgrade__c';
  $object_type = '';
  module_load_include('inc', 'salesforce_genmap', 'includes/salesforce_genmap.map');
  $map->salesforce_record_type = $object_type;
  $map->field_map = array(
    'webform_map' => array(
      'fsu_amount' => 'Upgrade_Amount__c',
      'fsu_did' => 'Donation_ID__c',
      'fsu_nid' => 'Node_ID__c',
      'fsu_uid' => 'User_ID__c',
      'fsu_fail_flag' => 'Failure_Flag__c',
      'fsu_timestamp' => 'Timestamp__c',
      'fsu_form_id' => 'Form_ID__c',
      'fsu_url' => 'Form_URL__c',
      'referrer' => 'Referrer__c',
      'initial_referrer' => 'Initial_Referrer__c',
      'ms' => 'Market_Source__c',
      'user_agent' => 'User_Agent__c',
      'utm_content' => 'UTM_content__c',
      'utm_medium' => 'UTM_medium__c',
      'utm_source' => 'UTM_source__c',
      'utm_term' => 'UTM_term__c',
    ),
  );
  $map->sync_options = array(
    'insert' => 'insert',
    'update' => 'update',
    'delete' => 'delete',
  );
  $map->status = 1;
  $map->module = 'fundraiser_sustainers_upgrade';
  salesforce_genmap_save_map($map, 'webform');
}

/**
 * Creates a default fieldmap for message action nodes.
 */
function fundraiser_sustainers_upgrade_sfobject_fieldmap() {
  module_load_include('inc', 'salesforce_genmap', 'includes/salesforce_genmap.map');
  $sfapi = salesforce_get_api();
  $object_type = '';
  if ($sfapi->isAuthorized()) {
    $sf_records = _sfw_salesforce_record_type_list($sfapi, 'sustainers_upgrade__c');
    $sf_records = array_flip($sf_records);
    if (array_key_exists('Sustainers Upgrade', $sf_records)) {
      $object_type = $sf_records['Sustainers Upgrade'];
    }
  }

  return '
  {
    "type" : "salesforce_mapping",
    "name" : "sustainers_upgrade",
    "label" : "Sustainers Upgrade",
    "sync_triggers" : "3",
    "salesforce_object_type" : "Sustainers_Upgrade__c",
    "salesforce_record_type" : "default",
    "drupal_entity_type" : "node",
    "drupal_bundle" : "sustainers_upgrade_form",
    "field_mappings" : [
      {
        "drupal_field" : { "fieldmap_type" : "property", "fieldmap_value" : "nid" },
        "salesforce_field" : {
          "autoNumber" : false,
          "byteLength" : 0,
          "calculated" : false,
          "calculatedFormula" : null,
          "cascadeDelete" : false,
          "caseSensitive" : false,
          "controllerName" : null,
          "createable" : true,
          "custom" : true,
          "defaultValue" : null,
          "defaultValueFormula" : null,
          "defaultedOnCreate" : false,
          "dependentPicklist" : false,
          "deprecatedAndHidden" : false,
          "digits" : 0,
          "displayLocationInDecimal" : false,
          "externalId" : false,
          "filterable" : true,
          "groupable" : false,
          "htmlFormatted" : false,
          "idLookup" : false,
          "inlineHelpText" : null,
          "label" : "Form ID",
          "length" : 0,
          "name" : "Form_ID__c",
          "nameField" : false,
          "namePointing" : false,
          "nillable" : true,
          "permissionable" : true,
          "picklistValues" : [],
          "precision" : 18,
          "referenceTo" : [],
          "relationshipName" : null,
          "relationshipOrder" : null,
          "restrictedDelete" : false,
          "restrictedPicklist" : false,
          "scale" : 0,
          "soapType" : "xsd:double",
          "sortable" : true,
          "type" : "double",
          "unique" : false,
          "updateable" : true,
          "writeRequiresMasterRead" : false
        },
        "key" : false,
        "direction" : "drupal_sf",
        "drupal_sf" : "always",
        "sf_drupal" : "always"
      },
      {
        "drupal_field" : { "fieldmap_type" : "property", "fieldmap_value" : "nid" },
        "salesforce_field" : {
          "autoNumber" : false,
          "byteLength" : 0,
          "calculated" : false,
          "calculatedFormula" : null,
          "cascadeDelete" : false,
          "caseSensitive" : false,
          "controllerName" : null,
          "createable" : true,
          "custom" : true,
          "defaultValue" : null,
          "defaultValueFormula" : null,
          "defaultedOnCreate" : false,
          "dependentPicklist" : false,
          "deprecatedAndHidden" : false,
          "digits" : 0,
          "displayLocationInDecimal" : false,
          "externalId" : false,
          "filterable" : true,
          "groupable" : false,
          "htmlFormatted" : false,
          "idLookup" : false,
          "inlineHelpText" : null,
          "label" : "Form ID",
          "length" : 0,
          "name" : "Form_ID__c",
          "nameField" : false,
          "namePointing" : false,
          "nillable" : true,
          "permissionable" : true,
          "picklistValues" : [],
          "precision" : 18,
          "referenceTo" : [],
          "relationshipName" : null,
          "relationshipOrder" : null,
          "restrictedDelete" : false,
          "restrictedPicklist" : false,
          "scale" : 0,
          "soapType" : "xsd:double",
          "sortable" : true,
          "type" : "double",
          "unique" : false,
          "updateable" : true,
          "writeRequiresMasterRead" : false
        },
        "key" : false,
        "direction" : "sf_drupal",
        "drupal_sf" : "always",
        "sf_drupal" : "always"
      },
      {
        "drupal_field" : { "fieldmap_type" : "property", "fieldmap_value" : "title" },
        "salesforce_field" : {
          "autoNumber" : false,
          "byteLength" : 240,
          "calculated" : false,
          "calculatedFormula" : null,
          "cascadeDelete" : false,
          "caseSensitive" : false,
          "controllerName" : null,
          "createable" : true,
          "custom" : false,
          "defaultValue" : null,
          "defaultValueFormula" : null,
          "defaultedOnCreate" : true,
          "dependentPicklist" : false,
          "deprecatedAndHidden" : false,
          "digits" : 0,
          "displayLocationInDecimal" : false,
          "externalId" : false,
          "filterable" : true,
          "groupable" : true,
          "htmlFormatted" : false,
          "idLookup" : true,
          "inlineHelpText" : null,
          "label" : "Sustainers Upgrade Name",
          "length" : 80,
          "name" : "Name",
          "nameField" : true,
          "namePointing" : false,
          "nillable" : true,
          "permissionable" : false,
          "picklistValues" : [],
          "precision" : 0,
          "referenceTo" : [],
          "relationshipName" : null,
          "relationshipOrder" : null,
          "restrictedDelete" : false,
          "restrictedPicklist" : false,
          "scale" : 0,
          "soapType" : "xsd:string",
          "sortable" : true,
          "type" : "string",
          "unique" : false,
          "updateable" : true,
          "writeRequiresMasterRead" : false
        },
        "key" : false,
        "direction" : "drupal_sf",
        "drupal_sf" : "always",
        "sf_drupal" : "always"
      },
      {
        "drupal_field" : { "fieldmap_type" : "property", "fieldmap_value" : "title" },
        "salesforce_field" : {
          "autoNumber" : false,
          "byteLength" : 240,
          "calculated" : false,
          "calculatedFormula" : null,
          "cascadeDelete" : false,
          "caseSensitive" : false,
          "controllerName" : null,
          "createable" : true,
          "custom" : false,
          "defaultValue" : null,
          "defaultValueFormula" : null,
          "defaultedOnCreate" : true,
          "dependentPicklist" : false,
          "deprecatedAndHidden" : false,
          "digits" : 0,
          "displayLocationInDecimal" : false,
          "externalId" : false,
          "filterable" : true,
          "groupable" : true,
          "htmlFormatted" : false,
          "idLookup" : true,
          "inlineHelpText" : null,
          "label" : "Sustainers Upgrade Name",
          "length" : 80,
          "name" : "Name",
          "nameField" : true,
          "namePointing" : false,
          "nillable" : true,
          "permissionable" : false,
          "picklistValues" : [],
          "precision" : 0,
          "referenceTo" : [],
          "relationshipName" : null,
          "relationshipOrder" : null,
          "restrictedDelete" : false,
          "restrictedPicklist" : false,
          "scale" : 0,
          "soapType" : "xsd:string",
          "sortable" : true,
          "type" : "string",
          "unique" : false,
          "updateable" : true,
          "writeRequiresMasterRead" : false
        },
        "key" : false,
        "direction" : "sf_drupal",
        "drupal_sf" : "always",
        "sf_drupal" : "always"
      }
    ],
    "push_async" : "0",
    "push_batch" : "0",
    "created" : "1455720126",
    "updated" : "1455734208",
    "weight" : "0",
    "locked" : "0",
    "rdf_mapping" : []
  }';
}