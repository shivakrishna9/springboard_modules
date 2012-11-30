<?php
/**
 * @file
 * Saleforce webform mapping form and functions.
 */

function salesforce_webform_mapping_form() {

  // Get list of available SObjects for mapping.
  $sfobjects = salesforce_get_sobjects();

  $sfobject_options = array('' => t('-- Select --'));
  foreach ($sfobjects as $object) {
    $sfobject_options[$object['name']] = $object['label'];
  }

  $form['salesforce_object_type'] = array(
    '#title' => t('Salesforce object'),
    '#type' => 'select',
    '#description' => t('Select a Salesforce object to map.'),
    //'#default_value' => isset($mapping) ? $mapping->salesforce_object_type : 0,
    '#options' => $sfobject_options,
    '#ajax' => array(
      'callback' => 'salesforce_mapping_form_callback',
      'wrapper' => 'salesforce_mapping_form'
    ),
    '#required' => TRUE
  );

  $form['salesforce_record_type'] = array(
    '#title' => t('Salesforce record type'),
    '#type' => 'select',
    '#description' => t('Select a record type. Leave blank for default.'),
  );

  $record_types = salesforce_get_recordtypes('Opportunity');
  krumo($record_types);

  return $form;
}