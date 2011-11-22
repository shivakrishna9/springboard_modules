<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the webform_user module.
 */
 
/**
 * Clones a user webform
 */
function webform_user_clone($node) {
  global $user;
  if (isset($node->nid)) {
    $nid = $node->nid;
    $original_node = drupal_clone($node);

    $node->nid = NULL;
    $node->vid = NULL;
    $node->name = $user->name;
    $node->uid = $user->uid;
    $node->created = NULL;
    $node->menu = NULL;
    $node->book['mlid'] = NULL;
    $node->path = NULL;
    $node->files = array();
    $node->title = t('Clone of !title', array('!title' => $node->title));
    $node->is_being_cloned = true;

    if (variable_get('clone_reset_'. $node->type, FALSE)) {
      $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
      // Fill in the default values.
      foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
        $node->$key = in_array($key, $node_options);
      }
    }
    
    node_save($node);

		// clone the fieldmap & webform_user settings
		$drupal = 'webform:' . $nid;
		$new_drupal = 'webform:' . $node->nid;
		
		$fieldmap = db_fetch_object(db_query("SELECT * FROM {salesforce_management_field_map} WHERE drupal = '%s'", $drupal));
		$webform_user = db_fetch_object(db_query("SELECT * FROM {webform_user} WHERE nid = %d", $nid));
		
		db_query("INSERT INTO {webform_user_component_map} (nid, cid, map_id) SELECT %d, cid, map_id FROM {webform_user_component_map} WHERE nid = %d", $node->nid, $nid);
		
		// make sure cloned node has a fieldmap
		if ($fieldmap) {
		  db_query("INSERT INTO {salesforce_management_field_map} (name, drupal, salesforce, fields, recordtype_id, dedupe_key) 
			 VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", $node->title . ' (webform map)', $new_drupal, $fieldmap->salesforce, $fieldmap->fields, $fieldmap->recordtype_id, $fieldmap->dedupe_key);
		}
		
		db_query("UPDATE {webform_user} SET foreign_key_field = '%s' WHERE nid = %d", $webform_user->foreign_key_field, $node->nid);
	
		drupal_set_message("User webform [$original_node->title] has been cloned successfully.");
		drupal_goto('node/'. $node->nid .'/edit');
  }
}

/**
 * Settings form for webform_user
 */
function webform_user_settings_form() {	
	// get a list of all webform enabled node types
	$webform_enabled_types = variable_get('webform_node_types', array('webform'));
	$node_types = node_get_types();

  // make a friendly list of options
	$options = array();
	foreach($webform_enabled_types as $type) {
    $options[$type] = $node_types[$type]->name;
	}

	$form['webform_user_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Webform user enabled content types'),
    '#options' => $options,
    '#default_value' => variable_get('webform_user_node_types', array('webform')),
	);
	
	// add our own submit handler
	$form['#submit'][] = 'webform_user_settings_form_submit';
	
	return system_settings_form($form);
}

/**
 * Submit handler for webform_user settings form.
 */
function webform_user_settings_form_submit($form, &$form_state) {
  // we need to add the selected node types to the webform_node_types_primary variable so that a webform record is added by default
  $webform_node_types_primary = variable_get('webform_node_types_primary', array('webform'));
  $webform_user_node_types = $form_state['values']['webform_user_node_types'];
  
  foreach($webform_user_node_types as $type => $enabled) {
    if ($enabled && !in_array($type, $webform_node_types_primary)) {
      $webform_node_types_primary[] = $type;
    }
  }
  
  // re-save the variable
  variable_set('webform_node_types_primary', $webform_node_types_primary);
}

/**
 * Form builder. Select the salesforce object relationship field.
 */ 
function webform_user_field_selection_form(&$form_state = NULL, $node) {
	// only show the related field drop down if the webform has been related to a salesforce object
	if (!empty($node->sf_webform_object) && $node->is_webform_user) {
		$form['webform_user_related_field'] = array(
			'#prefix' => t("<div>This webform is currently mapped to the <strong>$node->sf_webform_object</strong> object. Please select the field in this object that references the <strong>$node->related_object</strong> object.</div>"), 
			'#type' => 'select',
			'#options' => _get_object_relation_fields($node->sf_webform_object),
			'#title' => t('Lookup field'),
			'#required' => TRUE,
			'#default_value' => $node->foreign_key_field,
			'#description' => t("The field in the $node->sf_webform_object object that references the $node->related_object."),
		);
		
		$form['nid'] = array(
			'#type' => 'hidden',
			'#value' => $node->nid,
		);
		
		$form['vid'] = array(
			'#type' => 'hidden',
			'#value' => $node->vid,
		);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Save',
		);
	}
	else {
		$form['message'] = array(
			'#value' => 'The webform must first be mapped to a Salesforce object before the relationship field can be selected. Click the Salesforce mapping tab to create the map.',
		);
	}
	return $form;
}

/**
 * Form submission handler.
 */
function webform_user_field_selection_form_submit($form, &$form_state) {
	$foreign_key_field = $form_state['values']['webform_user_related_field'];
	$nid = $form_state['values']['nid'];
	
	db_query("UPDATE {webform_user} SET foreign_key_field = '%s' WHERE nid = %d", $foreign_key_field, $nid);
	drupal_set_message(t('Relationship field successfully set to !field', array('!field' => $foreign_key_field)));
}