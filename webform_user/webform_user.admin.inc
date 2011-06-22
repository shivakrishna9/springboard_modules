<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the webform_user module.
 */

/** 
 * Generates a list of webforms that have been converted to webform_user forms
 */
function webform_user_list() {
	$pager_num = 0;
	$result = pager_query(db_rewrite_sql("SELECT n.nid, n.title, n.created, n.changed, u.related_object, u.authenticate_submitters FROM {node} n INNER JOIN {webform_user} u
		ON u.nid = n.nid ORDER BY n.title"), 20, $pager_num);
		
	$header = array('Title', 'Related To', 'Authenticate Submitters', 'Last Modified', '', '', '');
	$rows = array();
	
	while ($data = db_fetch_object($result)) {
		$edit_link = l('edit', 'node/' . $data->nid . '/edit');
		$view_link = l('view', drupal_get_path_alias('node/' . $data->nid));
		$clone_link = l('clone', 'admin/content/clone-webform-user/'. $data->nid);
		$rows[] = array($data->title, $data->related_object, $data->authenticate_submitter ? 'Yes' : 'No', date('m/d/Y g:i:s a', $data->changed), $edit_link, $view_link, $clone_link);
	}
	
	$output = theme('table', $header, $rows);
	$output .= theme('pager', NULL, 20, $pager_num);
	return $output;
}

/**
 * Clones a user webform
 */
function webform_user_clone($nid) {
	if (is_numeric($nid)) {
    global $user;

    $node = node_load($nid);
    if (isset($node->nid)) {
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
			
			db_query("INSERT INTO {salesforce_management_field_map} (name, drupal, salesforce, fields, recordtype_id, dedupe_key) 
				VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", $node->title . ' (webform map)', $new_drupal, $fieldmap->salesforce, $fieldmap->fields, $fieldmap->recordtype_id, $fieldmap->dedupe_key);
			
			db_query("UPDATE {webform_user} SET foreign_key_field = '%s' WHERE nid = %d", $webform_user->foreign_key_field, $node->nid);
		
			drupal_set_message("User webform [$original_node->title] has been cloned successfully.");
			drupal_goto('node/'. $node->nid .'/edit');
    }
  }
}

/**
 * Settings form for webform_user
 */
function webform_user_settings_form() {	
	
	$form['webform_user_menu_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Menu title for webform user'),
		'#description' => t('The name of the menu item for the list of webform user forms under content management'),
		'#default_value' => variable_get('webform_user_menu_name', 'User webforms'),
	);
	
	return system_settings_form($form);
	
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