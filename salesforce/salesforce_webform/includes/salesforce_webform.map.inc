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
function salesforce_webform_map_form($form, &$form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'map' => array(),
  );

  // If webform is not mapped to salesforce object

    // Give option to map to a salesforce object
    // and lots more

  // else

    // Give option to unmap
    // All kinds of other options
    // Deduping, and removing deduping

  // end

  return $form;
}

/**
 * Validate the mapping form
 *
 * @D6TOD7: Did not exist in prior version
 */
function salesforce_webform_map_form_validate($form, &$form_state) {
  // @TODO: What would I be validating...?
}

/**
 * Submit callback for the mapping form
 *
 * @D6TOD7: Previously was sf_webform_map_form_submit()
 */
function salesforce_webform_map_form_submit($form, &$form_state) {
  // Store all the data.  Save using the CRUD API below.
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
 * @param $nid
 *   The nid of the webform of whose map we want to load
 *
 * @return
 *   The entity object you requested
 */
function salesforce_webform_load_map($nid) {
  $entities = entity_load('salesforce_webform_map', array($nid));
  return $entities[$nid];
}

/**
 * Deletes a Salesforce+Webform map
 *
 * @param $nid
 *   The nid of the webform of whose map we're deleting
 *
 * @return
 *   FALSE if the delete could be performed
 */
function salesforce_webform_delete_map($nid) {
  return entity_delete('salesforce_webform_map', $nid);
}

/**
 * @} salesforce_webform_map_crud
 */
