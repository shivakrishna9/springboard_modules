<?php

/**
 * @file
 * Configures the mapping of Salesforce objects to webforms
 */


/**
 * @defgroup salesforce_webform_map_form
 * @{
 */

/**
 * Form to do the actual mapping configuration processes
 *
 * @D6TOD7: Previously was sf_webform_map_form()
 */
function salesforce_webform_map_form() {

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
    //'#default_value' => isset($map) ? $map->salesforce_object_type : 0,
    '#options' => $sfobject_options,
    '#ajax' => array(
      'callback' => 'salesforce_map_form_callback',
      'wrapper' => 'salesforce_map_form'
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

/**
 * @} salesforce_webform_map_form
 */
