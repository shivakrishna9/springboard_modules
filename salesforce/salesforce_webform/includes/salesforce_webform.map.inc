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
      '#id' => 'salesforce-webform-map-helptext',
      '#type' => 'markup',
      '#markup' => t('When users submit this webform, map which values of the webform\'s components should be sent to your Salesforce organization.'),
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
  // Fail if there are no components attached to the webform
  if (empty($node->webform['components'])) {
    return array(
      'no_components' => array(
        '#markup' => t('You must first add components to your webform before proceeding with mapping to Salesforce objects.'),
        '#prefix' => '<div class="salesforce-webform-map-error salesforce-webform-map-error-components"><p>',
        '#suffix' => '</p></div>',
      ),
    );
  }

  // Fail if we can't connect to Salesforce
  $sfapi = salesforce_get_api();
  if (!$sfapi->isAuthorized()) {
    return array(
      'no_sfapi' => array(
        '#markup' => t('Cannot connect to Salesforce, preventing the ability to map Salesforce objects.  !ConfigureSalesforce',
                array('!ConfigureSalesforce' => l(t('Configure Salesforce'), 'admin/config/services/salesforce'))),
        '#prefix' => '<div class="salesforce-webform-map-error salesforce-webform-map-error-salesforce"><p>',
        '#suffix' => '</p></div>',
      ),
    );
  }

  // Time to build up the form
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
  );

  $form['salesforce_object_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Salesforce Object Mapping',
  );

  // The type of Salesforce Object
  $sf_objects = _sfw_salesforce_object_list($sfapi);
  $sf_object = _sfw_default_value('salesforce_object_type', $form_state);
  $form['salesforce_object_fieldset']['salesforce_object_type'] = array(
    '#id' => 'salesforce-object-type',
    '#title' => t('Salesforce object'),
    '#type' => 'select',
    '#description' => t('Select a Salesforce object to map to this webform.'),
    '#default_value' => $sf_object,
    '#options' => $sf_objects,
    '#required' => TRUE,
    // @TODO: Disable field once mapping has been saved
  );

  // @TODO: Add unmap button if mapping has been saved

  // The type of record of the chosen Salesforce Object
  $sf_records = _sfw_salesforce_record_type_list($sfapi, $sf_object);
  if (!empty($sf_records)) {
    $form['salesforce_object_fieldset']['salesforce_record_type'] = array(
      '#id' => 'salesforce-record-type',
      '#title' => t('Object Record Type'),
      '#type' => 'select',
      '#description' => t('Select a record type.'),
      '#default_value' => _sfw_default_value('salesforce_record_type', $form_state),
      '#options' => $sf_records,
      '#required' => TRUE,
    );
  }

  // Add the mapping functionality to the form
  $sf_fields = _sfw_salesforce_field_list($sfapi, $sf_object);
  $map_row_class_name = 'salesforce-webform-map-field';
  $form['field_map'] = array(
    '#type' => 'fieldset',
    '#title' => 'Field Mapping',
  );

  // Add the map from webform components to Salesforce fields
  $form['field_map']['salesforce_webform_map'] = array(
    '#id' => 'salesforce-webform-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Webform Componant'), t('Salesforce Field'), t('Dedupe Field')),
    '#rows' => array(),
  );
  $wf_components = $node->webform['components'];
  array_unshift($sf_fields, '-');
  foreach ($wf_components as $cid => $component) {
    $form['field_map']['salesforce_webform_map']['#rows'][] = array(
      'webform_component' => array(
        '#id' => 'map-label-webform-cid-'. $cid,
        '#markup' => $component['name'] .' ('. $component['type'] .')',
        '#prefix' => '<label class="'. $map_row_class_name .'">',
        '#suffix' => '</label>',
      ),
      'salesforce_webform_map_cid'. $cid => array(
        '#id' => 'salesforce-field-map-webform-cid-'. $cid,
        '#type' => 'select',
        '#options' => $sf_fields,
      ),
       'dedupe_field' => array(
        // @TODO
        '#markup' => '',
      ),
    );
  }
  _sfw_generate_form_table($form['field_map']['salesforce_webform_map']);

  // Add the map from node properties to Salesforce fields
  $form['field_map']['salesforce_node_map'] = array(
    '#id' => 'salesforce-node-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Node Property'), t('Salesforce Field')),
    '#rows' => array(
      array(
        'node_id' => array(
          '#id' => 'map-label-nid',
          '#markup' => t('Node ID'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'salesforce_node_map_id' => array(
          '#id' => 'salesforce-field-map-nid',
          '#type' => 'select',
          '#options' => $sf_fields,
        ),
      ),
      array(
        'node_title' => array(
          '#id' => 'map-label-node-title',
          '#markup' => t('Node Title'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'salesforce_node_map_title' => array(
          '#id' => 'salesforce-field-map-node-title',
          '#type' => 'select',
          '#options' => $sf_fields,
        ),
      )
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_node_map']);

  // Add the map from submission properties to Salesforce fields
  $form['field_map']['salesforce_submission_map'] = array(
    '#id' => 'salesforce-submission-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Submission Property'), t('Salesforce Field')),
    '#rows' => array(
      array(
        'submission_id' => array(
          '#id' => 'map-label-submission-id',
          '#markup' => t('Submission ID'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'salesforce_submission_map_id' => array(
          '#id' => 'salesforce-field-map-submission-id',
          '#type' => 'select',
          '#options' => $sf_fields,
        ),
      ),
      array(
        'submission_date' => array(
          '#id' => 'map-label-submission-date',
          '#markup' => t('Submission Date'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'salesforce_submission_map_date' => array(
          '#id' => 'salesforce-field-map-submission-date',
          '#type' => 'select',
          '#options' => $sf_fields,
        ),
      )
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_submission_map']);


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

/**
 * Validate the mapping form
 *
 * @D6TOD7: Did not exist in prior version
 */
function salesforce_webform_map_form_validate($form, &$form_state) {
  // @TODO
  //   - Using the same Salesforce field twice
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


/**
 * @defgroup form_helper Misc tools to help streamline form building
 * @{
 */

/**
 * Helper function for the tedious case of finding the default value of a field
 *
 * @param String $field
 *   The name of the field to find the default value of
 * @param Array $form_state
 *   The current $form_state as given to you by the system
 *
 * @return
 *   The value of the field you requested
 */
function _sfw_default_value($field, $form_state) {
  if (empty($form_state['input'])) {
    // Check against the node at $form_state['build_info']['args'][0]
    switch ($field) {
      // @TODO: Requires adding map entity on node load
    }
  }
  else {
    switch ($field) {
      case 'salesforce_object_type':
        return $form_state['input']['salesforce_object_fieldset']['salesforce_object_type'];
      case 'salesforce_record_type':
        if (empty($form_state['input']['salesforce_object_fieldset']['salesforce_record_type'])) {
          $sf_object = _sfw_default_value('salesforce_object_type', $form_state);
          return _sfw_salesforce_default_record_type($sf_object);
        }
        return $form_state['input']['salesforce_object_fieldset']['salesforce_record_type'];
      default:
        // Find the prefix of the field name
        $field_name_parts = explode('_', $field);
        array_pop($field_name_parts);
        $field_prefix = implode('_', $field_name_parts);

        // The field could be anywhere in the list, so we have to search for it
        foreach ($form_state['input']['field_map'][$field_prefix] as $field_name => $field_value) {
          if ($field_name == $field) {
            return $field_value;
          }
        }
    }
  }
  return NULL;
}

/**
 * Helper function for the annoying case of setting a table in a form
 */
function _sfw_generate_form_table(&$form) {
  $rows = $form['#rows'];
  $form['#rows'] = array();
  foreach ($rows as $row) {
    $form[] = $row;
    $columns = array();
    foreach ($row as $column) {
      $columns[] = array('data' => $column);
    }
    $form['#rows'][] = $columns;
  }
}

/**
 * Gather a simple list of Salesforce objects
 *
 * @param Salesforce $sfapi
 *   A Salesforce API object
 *
 * @return Array
 *   Array of Salesforce objects keyed by the object's name and the label as the
 *   value
 */
function _sfw_salesforce_object_list(Salesforce $sfapi) {
  // Salesforce module v4 filters down to updatable and triggerable so it is
  // easier to visually consume the list.
  $objects_info = $sfapi->objects(array('updateable' => TRUE, 'triggerable' => TRUE));

  $objects_list = array();
  $dupes = array();
  foreach ($objects_info as $object) {
    // Some objects have the same label, which just makes things confusing
    // I could make only subsequent duplicates unique, but I figure that users
    // may wonder what the first instance is referencing
    if (in_array($object['label'], $objects_list)) {
      $dupes[] = $object['label'];
    }

    $objects_list[$object['name']] = $object['label'];
  }

  // Now make duplicate labels be unique
  if (!empty($dupes)) {
    foreach ($objects_list as $object_name => $object_label) {
      if (in_array($object_label, $dupes)) {
        $objects_list[$object_name] = $object_label .' ('. $object_name .')';
      }
    }
  }

  return $objects_list;
}

/**
 * Gather a simple list of record types for a given Salesforce object
 *
 * @param Salesforce $sfapi
 *   A Salesforce API object
 * @param String $sf_object
 *   The type of Salesforce object you are querying
 *
 * @return Array
 *   Array of record types of the given Salesforce object
 */
function _sfw_salesforce_record_type_list(Salesforce $sfapi, $sf_object) {
  $object_info = $sfapi->objectDescribe($sf_object);
  $record_type_list = array();
  foreach ($object_info['recordTypeInfos'] as $record_type) {
    if ($record_type['name'] != 'Master' && $record_type['available'] == TRUE) {
      $record_type_list[$record_type['recordTypeId']] = $record_type['name'];
    }
  }
  return $record_type_list;
}

/**
 * Find the default record type for the given Salesforce object
 *
 * @param String $sf_object
 *   The type of Salesforce object you are querying
 *
 * @return String
 *   The default record type of the given Salesforce object, or NULL if there is
 *   not a default type
 */
function _sfw_salesforce_default_record_type($sf_object) {
  $sfapi = salesforce_get_api();
  $object_info = $sfapi->objectDescribe($sf_object);
  foreach ($object_info['recordTypeInfos'] as $record_type) {
    if ($record_type['defaultRecordTypeMapping'] == TRUE) {
      return $record_type['name'];
    }
  }
  return NULL;
}

/**
 * Gather a simple list of fields for the given Salesforce object
 *
 * @param Salesforce $sfapi
 *   A Salesforce API object
 * @param String $sf_object
 *   The type of Salesforce object you are querying
 *
 * @return Array
 *   Array of fields on the given Salesforce object
 */
function _sfw_salesforce_field_list(Salesforce $sfapi, $sf_object) {
  $object_info = $sfapi->objectDescribe($sf_object);
  $field_list = array();
  foreach ($object_info['fields'] as $field) {
    if ($field['updateable']) { // Filter out fields that can't be altered
      $field_list[$field['name']] = $field['label'];
    }
  }
  return $field_list;
}

/**
 * @} form_helper
 */
