<?php

/**
 * @file
 * Configures the mapping of Salesforce objects to webforms
 */

/**
 * Page callback to render the mapping page
 *
 * @D6TOD7: This step was skipped previously
 */
function salesforce_webform_map_page($node) {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#type' => 'markup',
      '#markup' => t('When users submit this webform, map which values of the webform\'s componants should be sent to your Salesforce organization.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    ),
    'salesforce_webform_map_form' => drupal_get_form('salesforce_webform_map_form', $node),
  );
  return $build;
}


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


/**
 * @defgroup salesforce_webform_map_crud Made easier with Entity API
 * @{
 *
 * @D6TOD7: This was done manually each time it was needed
 */

/**
 * Creates and saves a Salesforce+Webform map
 *
 * @param $map
 *   Can be an array of values or a salesforce_webform_map entity object
 *
 * @return
 *   The new version of the object (such as if you had passed in an array) and
 *   FALSE if what you passed through was incompatible with the save process.
 */
function salesforce_webform_save_map($map) {
  if (is_array($map)) {
    // Creates an entity object, but does not add it to the database
    $map = entity_create('salesforce_webform_map', $map);
  }
  if ($map->entityType() == 'salesforce_webform_map') {
    // Adds the data to the database
    return entity_save('salesforce_webform_map', $map);
  }
  return FALSE;
}

/**
 * Retreives a Salesforce+Webform map
 *
 * @param $id
 *   The unique id of the map, in our case it's salesforce_webform_map_id
 *
 * @return
 *   The entity object you requested
 */
function salesforce_webform_load_map($id) {
  $entities = entity_load('salesforce_webform_map', array($id));
  return $entities[$id];
}

/**
 * Deletes a Salesforce+Webform map
 *
 * @param $id
 *   The unique id of the map, in our case it's salesforce_webform_map_id
 *
 * @return
 *   FALSE if the delete could be performed
 */
function salesforce_webform_delete_map($id) {
  return entity_delete('salesforce_webform_map', $id);
}

/**
 * @} salesforce_webform_map_crud
 */
