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
      '#id' => 'salesforce-webform-map-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>'. t('When users submit this webform, map which values of the webform\'s components should be sent to your Salesforce organization.') .'</p>',
      '#prefix' => '<div class="salesforce-webform-map-helptext">',
      '#suffix' => '</div>',
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
        '#markup' => '<p>'. t('You must first add components to your webform before proceeding with mapping to Salesforce objects.') .'</p>',
        '#prefix' => '<div class="salesforce-webform-map-error salesforce-webform-map-error-components">',
        '#suffix' => '</div>',
      ),
    );
  }

  // Fail if we can't connect to Salesforce
  $sfapi = salesforce_get_api();
  if (!$sfapi->isAuthorized()) {
    return array(
      'no_sfapi' => array(
        '#markup' => '<p>'. t('Cannot connect to Salesforce, preventing the ability to map Salesforce objects.  !ConfigureSalesforce',
                array('!ConfigureSalesforce' => l(t('Configure Salesforce'), 'admin/config/services/salesforce'))) .'</p>',
        '#prefix' => '<div class="salesforce-webform-map-error salesforce-webform-map-error-salesforce">',
        '#suffix' => '</div>',
      ),
    );
  }

  // Time to build up the form
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    '#prefix' => '<div id="salesforce-webform-map-ajax-sf-object">',
    '#sufix' => '</div>',
  );

  $form['salesforce_object_info'] = array(
    '#type' => 'fieldset',
    '#title' => 'Salesforce Object Mapping',
  );

  // The type of Salesforce Object
  $sf_objects = _sfw_salesforce_object_list($sfapi);
  $sf_object = _sfw_default_value('salesforce_object_type', $form_state);
  $form['salesforce_object_info']['salesforce_object_type'] = array(
    '#id' => 'salesforce-object-type',
    '#title' => t('Salesforce object'),
    '#type' => 'select',
    '#description' => t('Select a Salesforce object to map to this webform.'),
    '#default_value' => $sf_object,
    '#options' => $sf_objects,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'salesforce_webform_map_form_ajax_sf_object',
      'wrapper' => 'salesforce-webform-map-ajax-sf-object',
      'method' => 'replace',
      'effect' => 'fade',
      // @TODO: Warn user that changing this value will erase their map work
    ),
    // @TODO: Disable field once mapping has been saved
  );

  if (empty($sf_object)) {
    // Since the user has not selected a Salesforce object yet, we should not
    // display the remainder of the form.  Stop here.
    return $form;
  }

  // @TODO: Add unmap button if mapping has been saved

  // The type of record of the chosen Salesforce Object
  $sf_records = _sfw_salesforce_record_type_list($sfapi, $sf_object);
  if (count($sf_records) > 1) {
    $form['salesforce_object_info']['salesforce_record_type'] = array(
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
    '#row_storage' => array(),
    '#sticky' => FALSE,
    '#prefix' => '<div id="salesforce-webform-map-ajax-sf-dedupe">',
    '#suffix' => '</div>',
  );
  $wf_components = $node->webform['components'];
  foreach ($wf_components as $cid => $component) {
    $sf_field = _sfw_default_value('cid_'. $cid, $form_state);
    $sf_field_dedupe = _sfw_salesforce_field_can_dedupe($sfapi, $sf_object, $sf_field);
    $row = array(
      'webform_component' => array(
        '#id' => 'map-label-webform-cid-'. $cid,
        '#markup' => $component['name'] .' ('. $component['type'] .')',
        '#prefix' => '<label class="'. $map_row_class_name .'">',
        '#suffix' => '</label>',
      ),
      'cid_'. $cid => array(
        '#id' => 'salesforce-field-map-webform-cid-'. $cid,
        '#type' => 'select',
        '#default_value' => $sf_field,
        '#options' => $sf_fields,
        '#ajax' => array(
          'callback' => 'salesforce_webform_map_form_ajax_sf_dedupe',
          'wrapper' => 'salesforce-ajax-dedupe-'. $cid,
          'method' => 'replace',
          'effect' => 'fade',

          // These are referred to in the AJAX callback
          '#cid' => $cid,
          '#dedupe_field' => 'salesforce_dedupe_'. $cid,
        ),
      ),
      'salesforce_dedupe_'. $cid => array(
        '#id' => 'salesforce-field-dedupe-cid-'. $cid,
        '#type' => 'hidden',
        '#value' => NULL,
        '#prefix' => '<div id="salesforce-ajax-dedupe-'. $cid .'">',
        '#suffix' => '</div>',
      ),
    );

    if (!empty($sf_field) && $sf_field_dedupe) {
      $dedupe_field = &$row['salesforce_dedupe_'. $cid];
      $dedupe_field['#type'] = 'radio';
      $dedupe_field['#return_value'] = $sf_field;
      $dedupe_field['#attributes'] = array('name' => 'salesforce_dedupe');
      $dedupe_field['#value'] = _sfw_default_value('salesforce_dedupe_'. $cid, $form_state);
    }

    $form['field_map']['salesforce_webform_map']['#row_storage'][$cid] = $row;
  }
  // Add some help text
  $form['field_map']['salesforce_webform_map']['#row_storage'][] = array(
    'empty' => array(
      '#markup' => '',
    ),
    'helptext' => array(
      '#id' => 'salesforce-webform-map-helptext-field-map',
      '#markup' => '<p>'. t('<strong>Dedupe Field:</strong> Minimizes duplicate
        records and allows updating existing records. Selecting a dedupe field
        chooses which field is the unique identifier for this form.  A dedupe
        option only appears if the selected field can be used as a unique
        identifier. Select \'None\' to not use this feature.') .'</p>',
      '#prefix' => '<div class="salesforce-webform-map-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_dedupe_none' => array(
      '#id' => 'salesforce-field-no-dedupe',
      '#title' => t('None'),
      '#type' => 'radio',
      '#return_value' => 'salesforce_dedupe_none',
      '#attributes' => array('name' => 'salesforce_dedupe'),
      '#value' => _sfw_default_value('salesforce_dedupe_none', $form_state),
    )
  );
  _sfw_generate_form_table($form['field_map']['salesforce_webform_map']);

  // Add the map from node properties to Salesforce fields
  $form['field_map']['salesforce_node_map'] = array(
    '#id' => 'salesforce-node-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Node Property'), t('Salesforce Field')),
    '#row_storage' => array(
      0 => array(
        'node_id_label' => array(
          '#id' => 'map-label-nid',
          '#markup' => t('Node ID'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'node_id' => array(
          '#id' => 'salesforce-field-map-nid',
          '#type' => 'select',
          '#default_value' => _sfw_default_value('node_id', $form_state),
          '#options' => $sf_fields,
        ),
      ),
      1 => array(
        'node_title_label' => array(
          '#id' => 'map-label-node-title',
          '#markup' => t('Node Title'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'node_title' => array(
          '#id' => 'salesforce-field-map-node-title',
          '#type' => 'select',
          '#default_value' => _sfw_default_value('node_title', $form_state),
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
    '#row_storage' => array(
      0 => array(
        'submission_id_label' => array(
          '#id' => 'map-label-submission-id',
          '#markup' => t('Submission ID'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'submission_id' => array(
          '#id' => 'salesforce-field-map-submission-id',
          '#type' => 'select',
          '#default_value' => _sfw_default_value('submission_id', $form_state),
          '#options' => $sf_fields,
        ),
      ),
      1 => array(
        'submission_date_label' => array(
          '#id' => 'map-label-submission-date',
          '#markup' => t('Submission Date'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'submission_date' => array(
          '#id' => 'salesforce-field-map-submission-date',
          '#type' => 'select',
          '#default_value' => _sfw_default_value('submission_date', $form_state),
          '#options' => $sf_fields,
        ),
      )
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_submission_map']);


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

/**
 * AJAX callback to populate the form for the chosen Salesforce object
 */
function salesforce_webform_map_form_ajax_sf_object($form) {
  return $form;
}

/**
 * AJAX callback to set a field row's dedupe cell
 */
function salesforce_webform_map_form_ajax_sf_dedupe($form, &$form_state) {
  $cid = $form_state['triggering_element']['#ajax']['#cid'];
  $field = $form_state['triggering_element']['#ajax']['#dedupe_field'];
  return $form['field_map']['salesforce_webform_map'][$cid][$field];
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
  $node = $form_state['build_info']['args'][0];
  $salesforce_object_type = $form_state['input']['salesforce_object_info']['salesforce_object_type'];
  $salesforce_record_type = '';
  if (isset($form_state['input']['salesforce_object_info']['salesforce_record_type'])) {
    $salesforce_record_type = $form_state['input']['salesforce_object_info']['salesforce_record_type'];
  }
  $dedupe_field = $form_state['input']['salesforce_dedupe'];

  $field_map = array();
  foreach ($form_state['input']['field_map'] as $group => $rows) {
    foreach ($rows as $row) {
      $sf_field = reset($row);
      $component = key($row);
      $field_map[$component] = $sf_field;
    }
  }

  if (isset($node->salesforce_webform_map)) {
    $map = $node->salesforce_webform_map;
    $map->salesforce_object_type = $salesforce_object_type;
    $map->salesforce_record_type = $salesforce_record_type;
    $map->dedupe_field = $dedupe_field;
    $map->field_map = $field_map;
    $map->updated = time();
  }
  else {
    $map = array(
      'nid' => $node->nid,
      'salesforce_object_type' => $salesforce_object_type,
      'salesforce_record_type' => $salesforce_record_type,
      'salesforce_dedupe_field' => $dedupe_field,
      'field_map' => $field_map,
      'created' => time(),
      'updated' => time(),
    );
  }
  $result = salesforce_webform_save_map($map);
  if ($result === FALSE) {
    drupal_set_message(t('There was an error saving the map.'), 'error');
  }
  else {
    drupal_set_message(t('The map was saved successfully.'));
  }
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
  // If there isn't any user contributed data yet, check against the node
  // @TODO: Requires adding map entity on node load
  if (empty($form_state['input'])) {
    // Check against the node at $form_state['build_info']['args'][0]
    switch ($field) {
      case 'salesforce_dedupe_none':
        // if not configured in node
          return TRUE;
        // else return node's value
    }
  }
  else {
    // Check against the user contributed data
    switch ($field) {
      case 'salesforce_object_type':
        return $form_state['input']['salesforce_object_info']['salesforce_object_type'];

      case 'salesforce_record_type':
        // If there is not a set record type yet, use the default record type for the object
        if (empty($form_state['input']['salesforce_object_info']['salesforce_record_type'])) {
          $sf_object = _sfw_default_value('salesforce_object_type', $form_state);
          return _sfw_salesforce_default_record_type($sf_object);
        }

        // Otherwise use the user's configured record type
        return $form_state['input']['salesforce_object_info']['salesforce_record_type'];

      case 'salesforce_dedupe_none':
        if (
             !isset($form_state['input']['salesforce_dedupe'])
             || empty($form_state['input']['salesforce_dedupe'])
             || $form_state['input']['salesforce_dedupe'] == 'salesforce_dedupe_none'
           ) {
          return TRUE;
        }
        return FALSE;

      case 'node_id':
        $i = 0;
      case 'node_title':
        if (isset($form_state['input']['field_map'])) {
          $i = isset($i) ? $i : 1;
          return $form_state['input']['field_map']['salesforce_node_map'][$i][$field];
        }
        return NULL;

      case 'submission_id':
        $i = 0;
      case 'submission_title':
        if (isset($form_state['input']['field_map'])) {
          $i = isset($i) ? $i : 1;
          return $form_state['input']['field_map']['salesforce_submission_map'][$i][$field];
        }
        return NULL;


      default:
        // Get the CID
        $field_parts = explode('_', $field);
        $cid = array_pop($field_parts);

        if (strpos($field, 'dedupe') === FALSE && isset($form_state['input']['field_map'])) {
          // These guys are all salesforce_webform_map fields
          return $form_state['input']['field_map']['salesforce_webform_map'][$cid][$field];
        }
        else if (isset($form_state['input']['salesforce_dedupe']) && $form_state['input']['salesforce_dedupe'] != 'salesforce_dedupe_none') {
          // And these guys are all dedupe fields which actually need to check
          // against the salesforce_dedupe value

          // Get the mapped field for this componant
          $sf_field = _sfw_default_value('cid_'. $cid, $form_state);

          // See if this is the currently configured dedupe field
          if ($form_state['input']['salesforce_dedupe'] == $sf_field) {
            return TRUE;
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
  foreach ($form['#row_storage'] as $row_num => $row) {
    // In order for the fields to render, they need to be in the root
    $form[$row_num] = $row;
    foreach ($form[$row_num] as $column => $cell) {
      // The magic is right here.  You need to pass the field data by reference
      // to the table rendering in order for the fields to function properly
      $form['#rows'][$row_num][$column]['data'] = &$form[$row_num][$column];
    }
  }
  unset($form['#row_storage']);
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
  $field_list = array(
    NULL => '-',
  );
  foreach ($object_info['fields'] as $field) {
    if ($field['updateable']) { // Filter out fields that can't be altered
      $field_list[$field['name']] = $field['label'];
    }
  }
  return $field_list;
}

/**
 * Find out the idLookup value for a given Salesforce field
 *
 * @param Salesforce $sfapi
 *   A Salesforce API object
 * @param String $sf_object
 *   The type of Salesforce object that has the field
 * @param String $sf_field
 *   The field you're looking for information about
 *
 * @return Bool
 *   The value of idLookup as provided by Salesforce
 */
function _sfw_salesforce_field_can_dedupe(Salesforce $sfapi, $sf_object, $sf_field) {
  if (empty($sf_object) || empty($sf_field)) {
    return FALSE;
  }
  $object_info = $sfapi->objectDescribe($sf_object);
  // Find the field
  foreach ($object_info['fields'] as $field) {
    if ($field['name'] == $sf_field) {
      return $field['idLookup'];
    }
  }
}

/**
 * Find out if the field's value must be unique in the Salesforce database
 *
 * @param Salesforce $sfapi
 *   A Salesforce API object
 * @param String $sf_object
 *   The type of Salesforce object that has the field
 * @param String $sf_field
 *   The field you're looking for information about
 *
 * @return Bool
 *   The value of as provided by Salesforce
 */
function _sfw_salesforce_field_is_unique(Salesforce $sfapi, $sf_object, $sf_field) {
  if (empty($sf_object) || empty($sf_field)) {
    return FALSE;
  }
  $object_info = $sfapi->objectDescribe($sf_object);
  // Find the field
  foreach ($object_info['fields'] as $field) {
    if ($field['name'] == $sf_field) {
      return $field['unique'];
    }
  }
}

/**
 * @} form_helper
 */
