<?php

/**
 * @file
 * Configures the mapping of Salesforce objects to webforms
 */

/**
 * Page callback to render the mapping page
 */
function salesforce_donation_map_page($node) {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'salesforce-donation-map-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('When users submit donations, map which values of the donation should be sent to your Salesforce organization.') . '</p>',
      '#prefix' => '<div class="salesforce-donation-map-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_donation_map_form' => drupal_get_form('salesforce_donation_map_form', $node),
  );
  return $build;
}

/**
 * Form to do the actual mapping configuration processes
 */
function salesforce_donation_map_form($form, &$form_state, $node) {
  // Fail if we can't connect to Salesforce
  $sfapi = salesforce_get_api();
  if (!$sfapi->isAuthorized()) {
    return array(
      'no_sfapi' => array(
        '#markup' => '<p>' . t('Cannot connect to Salesforce, preventing the ability to map Salesforce objects.  !ConfigureSalesforce',
                array('!ConfigureSalesforce' => l(t('Configure Salesforce'), 'admin/config/services/salesforce'))) . '</p>',
        '#prefix' => '<div class="salesforce-webform-map-error salesforce-webform-map-error-salesforce">',
        '#suffix' => '</div>',
      ),
    );
  }

  // Time to build up the form
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    '#prefix' => '<div id="salesforce-donation-map-ajax-sf-object">',
    '#sufix' => '</div>',
  );

  $form['salesforce_object_info'] = array(
    '#type' => 'fieldset',
    '#title' => 'Salesforce Object Mapping',
  );

  // The type of Salesforce Object
  $sf_objects = _sfw_salesforce_object_list($sfapi);
  $sf_object = _sfw_default_value($form_state, 'salesforce_object_type', 'salesforce_object_info');
  $form['salesforce_object_info']['salesforce_object_type'] = array(
    '#id' => 'salesforce-object-type',
    '#title' => t('Salesforce object'),
    '#type' => 'select',
    '#description' => t('Select a Salesforce object to map to donations.'),
    '#default_value' => $sf_object,
    '#options' => $sf_objects,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'salesforce_donation_map_form_ajax_sf_object',
      'wrapper' => 'salesforce-donation-map-ajax-sf-object',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  if (empty($sf_object)) {
    // Since the user has not selected a Salesforce object yet, we should not
    // display the remainder of the form.  Stop here.
    return $form;
  }

  // The type of record of the chosen Salesforce Object
  $sf_records = _sfw_salesforce_record_type_list($sfapi, $sf_object);
  if (count($sf_records) > 1) {
    $form['salesforce_object_info']['salesforce_record_type'] = array(
      '#id' => 'salesforce-record-type',
      '#title' => t('Object Record Type'),
      '#type' => 'select',
      '#description' => t('Select a record type.'),
      '#default_value' => _sfw_default_value($form_state, 'salesforce_record_type', 'salesforce_object_info'),
      '#options' => $sf_records,
      '#required' => TRUE,
    );
  }

  // Add the mapping functionality to the form
  $sf_fields = _sfw_salesforce_field_list($sfapi, $sf_object);
  $map_row_class_name = 'salesforce-donation-map-field';
  $form['field_map'] = array(
    '#type' => 'fieldset',
    '#title' => 'Field Mapping',
  );

  // Add the map from donation to Salesforce fields
  $form['field_map']['salesforce_donation_map'] = array(
    '#id' => 'salesforce-donation-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Donation Field'), t('Salesforce Field'), t('Dedupe Field')),
    '#row_storage' => array(),
    '#sticky' => FALSE,
    '#prefix' => '<div id="salesforce-donation-map-ajax-sf-dedupe">',
    '#suffix' => '</div>',
  );
  // Donation elements.
  $form['field_map']['salesforce_donation_map']['#row_storage']['did'] = array(
    'label' => array(
      '#id' => 'map-label-did',
      '#markup' => t('Donation ID'),
      '#prefix' => '<label class="' . $map_row_class_name . '">',
      '#suffix' => '</label>',
    ),
    'sf_field' => array(
      '#id' => 'salesforce-map-did',
      '#type' => 'select',
      '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_donation_map', 'did'),
      '#options' => $sf_fields,
    ),
    'dedupe' => array(
      '#id' => 'salesforce-dedupe-did',
      '#type' => 'hidden',
      '#value' => NULL,
      '#prefix' => '<div id="salesforce-ajax-dedupe-did">',
      '#suffix' => '</div>',
    ),
  );
  $form['field_map']['salesforce_donation_map']['#row_storage']['gateway'] = array(
    'label' => array(
      '#id' => 'map-label-gateway',
      '#markup' => t('Gateway ID'),
      '#prefix' => '<label class="' . $map_row_class_name . '">',
      '#suffix' => '</label>',
    ),
    'sf_field' => array(
      '#id' => 'salesforce-map-gateway',
      '#type' => 'select',
      '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_donation_map', 'gateway'),
      '#options' => $sf_fields,
    ),
    'dedupe' => array(
      '#id' => 'salesforce-dedupe-gateway',
      '#type' => 'hidden',
      '#value' => NULL,
      '#prefix' => '<div id="salesforce-ajax-dedupe-gateway">',
      '#suffix' => '</div>',
    ),
  );
  $form['field_map']['salesforce_donation_map']['#row_storage']['txn_id'] = array(
    'label' => array(
      '#id' => 'map-label-txn_id',
      '#markup' => t('Transaction ID'),
      '#prefix' => '<label class="' . $map_row_class_name . '">',
      '#suffix' => '</label>',
    ),
    'sf_field' => array(
      '#id' => 'salesforce-map-txn_id',
      '#type' => 'select',
      '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_donation_map', 'txn_id'),
      '#options' => $sf_fields,
    ),
    'dedupe' => array(
      '#id' => 'salesforce-dedupe-txn_id',
      '#type' => 'hidden',
      '#value' => NULL,
      '#prefix' => '<div id="salesforce-ajax-dedupe-txn_id">',
      '#suffix' => '</div>',
    ),
  );
  $form['field_map']['salesforce_donation_map']['#row_storage']['status'] = array(
    'label' => array(
      '#id' => 'map-label-status',
      '#markup' => t('Status'),
      '#prefix' => '<label class="' . $map_row_class_name . '">',
      '#suffix' => '</label>',
    ),
    'sf_field' => array(
      '#id' => 'salesforce-map-status',
      '#type' => 'select',
      '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_donation_map', 'status'),
      '#options' => $sf_fields,
    ),
    'dedupe' => array(
      '#id' => 'salesforce-dedupe-status',
      '#type' => 'hidden',
      '#value' => NULL,
      '#prefix' => '<div id="salesforce-ajax-dedupe-status">',
      '#suffix' => '</div>',
    ),
  );
  // Donation fields.
  $donation_fields = _fundraiser_get_field_fields(); // Grab the regular set of donation fields.
  foreach ($donation_fields as $form_key => $donation_field) {
    // Skip if this is a fieldset type.
    if ($donation_field['#type'] != 'fieldset') {
      $sf_field = _sfw_default_value($form_state, 'sf_field', 'salesforce_donation_map', $form_key);
      $sf_field_dedupe = _sfw_salesforce_field_can_dedupe($sfapi, $sf_object, $sf_field);
      $row = array(
        'label' => array(
          '#id' => 'donation-label-' . $form_key,
          '#markup' => $donation_field['#title'] . ' (' . $donation_field['#type'] . ')',
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-' . $form_key,
          '#type' => 'select',
          '#default_value' => $sf_field,
          '#options' => $sf_fields,
          '#ajax' => array(
            'callback' => 'salesforce_donation_map_form_ajax_sf_dedupe',
            'wrapper' => 'salesforce-ajax-dedupe-' . $form_key,
            'method' => 'replace',
            'effect' => 'fade',
            '#form_key' => $form_key,
          ),
        ),
        'dedupe' => array(
          '#id' => 'salesforce-dedupe-' . $form_key,
          '#type' => 'hidden',
          '#value' => NULL,
          '#prefix' => '<div id="salesforce-ajax-dedupe-' . $form_key . '">',
          '#suffix' => '</div>',
        ),
      );

      if (!empty($sf_field) && $sf_field_dedupe) {
        $dedupe_field = &$row['dedupe'];
        $dedupe_field['#type'] = 'radio';
        $dedupe_field['#return_value'] = $sf_field;
        $dedupe_field['#attributes'] = array('name' => 'salesforce_dedupe_field');
        $dedupe_field['#value'] = _sfw_default_value($form_state, 'dedupe', 'salesforce_donation_map', $form_key);
      }
      $form['field_map']['salesforce_donation_map']['#row_storage'][$form_key] = $row;
    }
  }
  // Add some help text
  $form['field_map']['salesforce_donation_map']['#row_storage'][0] = array(
    'empty' => array(
      '#markup' => '',
    ),
    'helptext' => array(
      '#id' => 'salesforce-donation-map-helptext-field-map',
      '#markup' => '<p>' . t('<strong>Dedupe Field:</strong> Minimizes duplicate
        records and allows updating existing records. Selecting a dedupe field
        chooses which field is the unique identifier for this form.  A dedupe
        option only appears if the selected field can be used as a unique
        identifier. Select \'None\' to not use this feature.') . '</p>',
      '#prefix' => '<div class="salesforce-donation-map-helptext">',
      '#suffix' => '</div>',
    ),
    'no_dedupe' => array(
      '#id' => 'salesforce-no-dedupe',
      '#title' => t('None'),
      '#type' => 'radio',
      '#return_value' => 'none',
      '#attributes' => array('name' => 'salesforce_dedupe_field'),
      '#default_value' => _sfw_default_value($form_state, 'no_dedupe', 'salesforce_donation_map', 0),
    )
  );
  _sfw_generate_form_table($form['field_map']['salesforce_donation_map']);

  // Add the map from node properties to Salesforce fields. For nid and title.
  $form['field_map']['salesforce_node_map'] = array(
    '#id' => 'salesforce-node-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Node Property'), t('Salesforce Field')),
    '#row_storage' => array(
      'nid' => array(
        'label' => array(
          '#id' => 'map-label-nid',
          '#markup' => t('Node ID'),
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-nid',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_node_map', 'nid'),
          '#options' => $sf_fields,
        ),
      ),
      'title' => array(
        'label' => array(
          '#id' => 'map-label-node-title',
          '#markup' => t('Node Title'),
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-node-title',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_node_map', 'title'),
          '#options' => $sf_fields,
        ),
      )
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_node_map']);

  // Add the map from user properties to Salesforce fields.
  $form['field_map']['salesforce_user_map'] = array(
    '#id' => 'salesforce-user-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('User Property'), t('Salesforce Field')),
    '#row_storage' => array(
      'uid' => array(
        'label' => array(
          '#id' => 'map-label-uid',
          '#markup' => t('User ID'),
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-uid',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_user_map', 'uid'),
          '#options' => $sf_fields,
        ),
      ),
      'name' => array(
        'label' => array(
          '#id' => 'map-label-user-name',
          '#markup' => t('User Name'),
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-user-name',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_user_map', 'name'),
          '#options' => $sf_fields,
        ),
      ),
      'email' => array(
        'label' => array(
          '#id' => 'map-label-user-email',
          '#markup' => t('User Email'),
          '#prefix' => '<label class="' . $map_row_class_name . '">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-user-email',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_user_map', 'email'),
          '#options' => $sf_fields,
        ),
      ),
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_user_map']);

  // Add the map from submission properties to Salesforce fields
  $form['field_map']['salesforce_submission_map'] = array(
    '#id' => 'salesforce-submission-map',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => array(t('Submission Property'), t('Salesforce Field')),
    '#row_storage' => array(
      'sid' => array(
        'label' => array(
          '#id' => 'map-label-submission-id',
          '#markup' => t('Submission ID'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-submission-id',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_submission_map', 'sid'),
          '#options' => $sf_fields,
        ),
      ),
      'date' => array(
        'label' => array(
          '#id' => 'map-label-submission-date',
          '#markup' => t('Submission Date'),
          '#prefix' => '<label class="'. $map_row_class_name .'">',
          '#suffix' => '</label>',
        ),
        'sf_field' => array(
          '#id' => 'salesforce-map-submission-date',
          '#type' => 'select',
          '#default_value' => _sfw_default_value($form_state, 'sf_field', 'salesforce_submission_map', 'date'),
          '#options' => $sf_fields,
        ),
      )
    ),
  );
  _sfw_generate_form_table($form['field_map']['salesforce_submission_map']);

  // Mapping configuration options
  $form['map_config'] = array(
    '#type' => 'fieldset',
    '#title' => 'Map Configuration',
  );

  // Options for Syncing with Salesforce
  $form['map_config']['sync_options'] = array(
    '#id' => 'salesforce-map-config-sync',
    '#type' => 'checkboxes',
    '#required' => TRUE,
    '#title' => t('Salesforce Sync Options'),
    '#description' => t('When should donations be synced with Salesforce?'),
    '#default_value' => _sfw_default_value($form_state, 'sync_options', 'map_config'),
    '#options' => array(
      'insert' => t('New donations'),
      'update' => t('Modified donations'),
      'delete' => t('Deleted donations'),
    ),
  );

  // Buttons buttons buttons
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  $map = salesforce_donation_load_map($node->nid);
  if (!empty($map)) {
    $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => 'Cancel',
    );
    $form['unmap'] = array(
      '#type' => 'submit',
      '#value' => 'Unmap',
    );
  }

  return $form;
}

/**
 * AJAX callback to populate the form for the chosen Salesforce object
 */
function salesforce_donation_map_form_ajax_sf_object($form) {
  return $form;
}

/**
 * AJAX callback to set a field row's dedupe cell
 */
function salesforce_donation_map_form_ajax_sf_dedupe($form, &$form_state) {
  $form_key = $form_state['triggering_element']['#ajax']['#form_key'];
  return $form['field_map']['salesforce_donation_map'][$form_key]['dedupe'];
}

/**
 * Validate the mapping form
 */
function salesforce_donation_map_form_validate($form, &$form_state) {
  if (!isset($form_state['input']['field_map'])) {
    // There is nothing to check
    return;
  }

  // No Salesforce field can be used twice
  $error_text = t('Multiple donation fields cannot be mapped to the same Salesforce fields.');
  $sf_fields = array();
  foreach ($form_state['input']['field_map'] as $group => $drupal_fields) {
    foreach ($drupal_fields as $drupal_field => $columns) {
      $sf_field = isset($columns['sf_field']) ? $columns['sf_field'] : NULL;
      if (!empty($sf_field)) {
        $field_path = 'field_map][' . $group . '][' . $drupal_field . '][sf_field';
        $count = isset($sf_fields[$sf_field]) ? count($sf_fields[$sf_field]) : 0;
        if ($count == 1) {
          // Set error on current field
          form_set_error($field_path, $error_text);

          // Set error on prior field
          $prior_drupal_field = key($sf_fields[$sf_field]);
          $prior_field_path = 'field_map][' . $sf_fields[$sf_field][$prior_drupal_field] . '][' . $prior_drupal_field . '][sf_field';
          form_set_error($prior_field_path, $error_text);
        }
        elseif ($count > 1) {
          // Add another error for this field being used again
          form_set_error($field_path, $error_text);
        }
        $sf_fields[$sf_field][$drupal_field] = $group;
      }
    }
  }
}

/**
 * Submit callback for the mapping form
 */
function salesforce_donation_map_form_submit($form, &$form_state) {
  $node = $form_state['build_info']['args'][0];

  // Delete the map
  if ($form_state['clicked_button']['#value'] == 'Unmap') {
    $result = salesforce_donation_delete_map($node->nid);
    if ($result === FALSE) {
      drupal_set_message(t('The Salesforce to donation map could not be deleted.'), 'error');
    }
    else {
      drupal_set_message(t('The Salesforce to donation map has been removed.'));
    }
    return;
  }

  // Cancel and do nothing
  elseif ($form_state['clicked_button']['#value'] == 'Cancel') {
    drupal_set_message(t('The Salesforce donation map has been reset to the original values.'));
    return;
  }

  // Save the contributed values
  elseif ($form_state['clicked_button']['#value'] == 'Save') {
    $map = _sfw_convert_input($form_state['input']);
    $map['nid'] = $node->nid;
    $result = salesforce_webform_save_map($map);
    if ($result === FALSE) {
      drupal_set_message(t('There was an error saving the map.'), 'error');
    }
    else {
      drupal_set_message(t('The map was saved successfully.'));
    }
    return;
  }

  // No action was specified
  drupal_set_message(t('Form was submitted unexpectedly. It has been reset to the original values.'), 'error');
}

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
function _sfw_default_value($form_state, $field, $group = NULL, $drupal_field = NULL) {
  $node = $form_state['build_info']['args'][0];
  $node_map = salesforce_donation_load_map($node->nid);
  if (empty($form_state['input']) && empty($node_map)) {
    return '';
  }

  // Refer against inputted values if the form has been submitted, else the node
  if (empty($form_state['input']['salesforce_object_info']) && !empty($node_map)) {
    $map = (array)$node_map;
  }
  else {
    $map = _sfw_convert_input($form_state['input']);
  }

  // Salesforce object, record and dedupe field value
  if (empty($group) || $group == 'salesforce_object_info' || $group == 'map_config') {
    if ($field == 'sync_options' && empty($map['sync_options'])) {
      // Make all the default value
      return array(
        'insert',
        'update',
        'delete',
      );
    }
    elseif (isset($map[$field])) {
      return $map[$field];
    }
    elseif ($field == 'salesforce_record_type') {
      // If there is not a set record type yet, use the default record type for the object
      if (empty($map[$group]['salesforce_record_type'])) {
        $sf_object = _sfw_default_value($form_state, 'salesforce_object_type', $group);
        return _sfw_salesforce_default_record_type($sf_object);
      }
    }
  }

  // Handling for the case of no dedupe field value
  elseif ($field == 'no_dedupe') {
    $salesforce_dedupe_field = _sfw_default_value($form_state, 'salesforce_dedupe_field');
    if (empty($salesforce_dedupe_field) || $salesforce_dedupe_field == 'none') {
      return TRUE;
    }
    return FALSE;
  }

  // Dedupe field values
  // These are found by seeing which salesforce field is mapped to the related
  // dedupe field and then checking if salesforce_dedupe_field has that field
  // selected
  elseif ($field == 'dedupe') {
    $salesforce_dedupe_field = _sfw_default_value($form_state, 'salesforce_dedupe_field');
    if (!empty($salesforce_dedupe_field) && $salesforce_dedupe_field != 'none') {
      $sf_field = _sfw_default_value($form_state, 'sf_field', $group, $drupal_field);
      if ($salesforce_dedupe_field == $sf_field) {
        return TRUE;
      }
    }
    return FALSE;
  }

  // The rest are all the salesforce mapping fields
  else {
    if (isset($map['field_map'][$group][$drupal_field])) {
      return $map['field_map'][$group][$drupal_field];
    }
  }
  return '';
}

/**
 * Helper function that will convert input from a form submission into a savable format
 */
function _sfw_convert_input($input) {
  $map = array();

  $map['salesforce_dedupe_field'] = NULL;
  if (isset($input['salesforce_dedupe_field']) && $input['salesforce_dedupe_field'] != 'none') {
    $map['salesforce_dedupe_field'] = $input['salesforce_dedupe_field'];
  }

  if (isset($input['salesforce_object_info'])) {
    $map['salesforce_object_type'] = '';
    if (isset($input['salesforce_object_info']['salesforce_object_type'])) {
      $map['salesforce_object_type'] = $input['salesforce_object_info']['salesforce_object_type'];
    }
    $map['salesforce_record_type'] = '';
    if (isset($input['salesforce_object_info']['salesforce_record_type'])) {
      $map['salesforce_record_type'] = $input['salesforce_object_info']['salesforce_record_type'];
    }
  }

  $map['field_map'] = array();
  if (isset($input['field_map'])) {
    foreach ($input['field_map'] as $group => $drupal_fields) {
      foreach ($drupal_fields as $drupal_field => $columns) {
        if (isset($columns['sf_field']) && !empty($columns['sf_field'])) {
          $map['field_map'][$group][$drupal_field] = $columns['sf_field'];
        }
      }
    }
  }

  $map['sync_options'] = isset($input['map_config']['sync_options']) ? $input['map_config']['sync_options'] : array();

  return $map;
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
        $objects_list[$object_name] = $object_label . ' (' . $object_name . ')';
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
