<?php
/**
 * Admin functions for Capwiz Connect integration
 */

/**
 * Functions to copy from sf_webform:
 * sf_webform_remove_mapping
 * 
 * @todo
 * - Add overwrite setting field to the mapping form
 * - Combine common elements of capwiz_connect_render_salesforce_object_fields() and theme_capwiz_connect_account_user_mapping(), and make general for use by action mapping
 * - Dump the values retrieved when getting a user, and make sure the core ones are included on the mapping form
 * - Dedupe field not saving
 */

/**
 * Amdin list of Capwiz Connect accounts
 */
function capwiz_connect_account_list() {
  $table_headers = array(
    'Name',
    'Username',
    'Operations',
  );

  $accounts = array();
  
  $result = db_query('SELECT * FROM {capwiz_connect_accounts} ORDER BY display_name');
  while ($row = db_fetch_object($result)) {
    $operations = array(
      l('overview', 'admin/settings/capwiz_connect/accounts/' . $row->id),
      l('edit', 'admin/settings/capwiz_connect/accounts/' . $row->id . '/edit'),
      l('user mapping', 'admin/settings/capwiz_connect/accounts/' . $row->id . '/user_mapping'),
      l('action mapping', 'admin/settings/capwiz_connect/accounts/' . $row->id . '/action_mapping'),
      l('delete', 'admin/settings/capwiz_connect/accounts/' . $row->id . '/delete'),
    );
    
    $accounts[] = array(
      $row->display_name,
      $row->user_name,
      implode(' | ', $operations),
    );
  }
  
  if (empty($accounts)) {
    $accounts[] = array(
      array(
        'data' => t('No Capwiz Connect accounts have been added.'),
        'colspan' => count($table_headers),
      ),
    );
  }
  
  return theme_table($table_headers, $accounts);
}


/**
 * Form to add or edit a Capwiz Connect account
 */
function capwiz_connect_account_addoredit($form_state, $account = NULL) {
  $form = array();
  $form['#redirect'] = 'admin/settings/capwiz_connect';
  if (is_object($account)) {
    $form['account_id'] = array(
      '#type' => 'value',
      '#value' => $account->id,
    );
    $account_details = (array)$account;
    
    // Go to the overview page after saving
    $form['#redirect'] = 'admin/settings/capwiz_connect/accounts/' . $account->id;
  }
  
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#description' => t('Name to show for this account. This is only used to differentiate accounts for admins.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['display_name'],
  );
  $form['user_name'] = array(
    '#type' => 'textfield',
    '#title' => t('User Name'),
    '#description' => t('API username for the Capwiz Connect API.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['user_name'],
  );
  $form['user_password'] = array(
    '#type' => 'textfield',
    '#title' => t('User Password'),
    '#description' => t('API password for the Capwiz Connect API.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['user_password'],
  );
  
  
  /**
   * How to store users. For now, the only option is to store users into the
   * capwiz_connect_users table. Later, we will add an option to create Drupal
   * users from Capwiz users.
   * For now, use the only option available.
   */
  $form['store_users_as'] = array(
    '#type' => 'value',
    '#value' => 'db'
  );
  
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );  
  
  return $form;
}


/**
 * Submit the add/edit form
 */
function capwiz_connect_account_addoredit_submit($form, &$form_state) {
  $values = $form_state['values'];
  $record = array(
    'display_name' => $values['display_name'],
    'user_name' => $values['user_name'],
    'user_password' => $values['user_password'],
    'id' => $values['account_id'],
  );
  
  // Determine if inserting or updating
  if ($values['account_id'] > 0) {
    $saved_record = drupal_write_record('capwiz_connect_accounts', $record, 'id');
    if ($saved_record) {
      drupal_set_message(t('Account updated'));
    }
  }
  else {
    $new_record = drupal_write_record('capwiz_connect_accounts', $record);
    if ($new_record) {
      drupal_set_message(t('New Account created'));
    }
  }
  
  return $save_ok;
}



/*********************************************************/
/**
 * User mapping form functions
 */
 
/**
 * User field mapping for Capwiz Connect account
 * Based on sf_webform_mapping_form()
 */
function capwiz_connect_account_user_mapping($form_state, $account) {
	$base = drupal_get_path('module', 'capwiz_connect');
	drupal_add_js($base .'/capwiz_connect.js');

	$form = array(
    '#tree' => TRUE,
    '#account' => $account,
    'mapping' => array(),
  );

	// only show object selection for webforms that haven't been mapped
	if (empty($account->mapping_user)) {
		$form['object'] = array(
			'#type' => 'select',
			'#options' => _get_salesforce_objects(),
			'#default_value' => $account->mapping_user['sf_object'],
			'#title' => t('Salesforce Object'),
			'#description' => t('Choose the Salesforce object you want to create for each Capwiz user.'),
			'#ahah' => array(
			  'path' => 'js/capwiz_connect/salesforce-object-fields',
	      'wrapper' => 'map',
	      'effect' => 'fade',
	    ),
		);
	}
	else {
		$form['object'] = array(
			'#prefix' => t("<div>Users from this Capwiz account are currently mapped to the <strong>%sf_object</strong> object.</div>", array('%sf_object' => $account->mapping_user['sf_object'])),
			'#type' => 'hidden',
			'#default_value' => $account->mapping_user['sf_object'],
			'#suffix' => t('<div>Use the <strong>Remove Map</strong> button below if you wish to map to a different Salesforce object.</div>'),
		);
	}

	$form['map'] = array(
    '#prefix' => '<div id="map">',
    '#suffix' => '</div>',
    '#value' => '&nbsp;',
  );
	
	// Only build the field mapping form if a Salesforce object type has been
	// selected to map to
	if ($form_state['values']['object'] || !empty($account->mapping_user['sf_object'])) {
		$object = !empty($account->mapping_user['sf_object']) ? $account->mapping_user['sf_object'] : $form_state['values']['object'];
		
		$connection = capwiz_connect($account->id);
		$user_fields = $connection->getCustomUserFields();
    
		// Get the fields available on this type of Salesforce object
		$object_details = _capwiz_connect_get_sf_object_details($object);
		$fields = $object_details['fields'];
		$record_types = $object_details['record_types'];

		// Create a drop down list friendly version of the Salesforce fields
		foreach($fields as $key => $value) {
			$options[$key] = $value['label'];
		}
		
		$disabled = !empty($account->mapping_user['sf_object']) ? false : true;
		
		$form['record_type'] = array(
			'#type' => 'select',
			'#title' => t('Record Type'),
			'#description' => t('The type of !name to create', array('!name' => $account->mapping_user['sf_object'])),
			'#options' => $record_types,
			'#default_value' => $account->mapping_user['recordtype_id'],
		);

		foreach($user_fields as $field) {
		  $field_key = $field->name;
		  $map_for_field = $account->mapping_user['map'][$field_key];
		  
			// Capwiz field
			$form['map']['capwiz_field'][$field_key] = array(
  			'#type' => 'item',
  			'#description' => t($field->label . " - (" . $field->formInputType . ")"),
			);
			// Salesforce field
			$form['map']['salesforce'][$field_key] = array(
				'#type' => 'select',
				'#options' => $options,
				'#default_value' => $account->mapping_user['map'][$field_key],
			);
			
			if ($map_for_field && ($fields[$map_for_field]['idLookup'] || $fields[$map_for_field]['externalId'])) {
				if ($account->mapping_user['dedupe_key'] == $map_for_field) {
					$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/remove-dedupe-field/' . $map_for_field .'">Remove dedupe field</a>';
				}
				else {
					$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/set-dedupe-field/' . $map_for_field .'">Set as dedupe field</a>';
				}
			}
			else {
				$value = '';
			}
			$form['map']['dedupe'][$field_key] = array(
				'#value' => $value,
			);
		}

		$form['remove'] = array(
			'#type' => 'submit',
			'#value' => t('Remove Map'),
			'#submit' => array('capwiz_connect_remove_user_mapping'),
		);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Submit',
		);
	}
	return $form;
}

/**
 * Submit handler to delete the salesforce map from a webform
 */
function capwiz_connect_remove_user_mapping($form, &$form_state) {
	$drupal = 'capwiz_connect:' . $form['#account']->id . ':user';	
	db_query("DELETE FROM {salesforce_management_field_map} WHERE drupal = '%s'", $drupal);	
	drupal_set_message(t('Salesforce mapping has been removed'));
}


/**
 * Function to return an array of object fields
 * Copy of _get_object_details() from sf_webform module
 */
function _capwiz_connect_get_sf_object_details($object_name) {
	$sf = salesforce_management_api_connect();
	if ($sf) {
		$record_types = array();
		
		$object = $sf->client->describeSObject($object_name);
		// get object fields
		foreach($object->fields as $field) {
			$object_fields[$field->name] = array(
				'label' => $field->label,
				'idLookup' => $field->idLookup,
				'externalId' => $field->externalId,
			);
		}

		// get object record types
		foreach($object->recordTypeInfos as $record_type) {
			if ($record_type->available && $record_type->name != 'Master') {
				$record_types[$record_type->recordTypeId] = $record_type->name;
			}
		}
		
		$object_fields[] = '--Select One--';
		$record_types[] = '--Select One--';
		
		asort($object_fields);
		asort($record_types);
		
		return array(
			'fields' => $object_fields,
			'record_types' => $record_types,
		);
	}	
	return array();
}


/** 
 * AHAH callback for mapping form
 * Copy of render_salesforce_object_fields() from sf_webform module
 */
function capwiz_connect_render_salesforce_object_fields() {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

	// HACK: Select values changing never get recognized
	unset($form['object']['#value']);

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

	$output = drupal_render($form['record_type']);

  $map = $form['map'];
	
	// Add all the mappable fields as elements in an array to be passed into the
	// theme function
	$rows = array();
	$capwiz_fields = element_children($map['capwiz_field']);
	foreach ($capwiz_fields as $field_key) {
		$row = array();
		$row[] = array('data' => drupal_render($map['capwiz_field'][$field_key]), 'width' => '300');
		$row[] = drupal_render($map['salesforce'][$field_key]);
		$row[] = drupal_render($map['dedupe'][$field_key]);
		$rows[] = $row;
	}

	$header = array("Capwiz User Field","Salesforce Field","Dedupe Field");
	$output .= theme('table', $header, $rows, array('id' => 'capwiz_connect_account_user_mapping'));
	$output .= drupal_render($form['submit']);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}


/**
 * Submit the user mapping form
 */
function capwiz_connect_account_user_mapping_submit($form, &$form_state) {
  if ($form_state['values']['map']['salesforce']) {

		$drupal_fieldmap_name = 'capwiz_connect:' . $form['#account']->id . ':user';
		$fieldmap_id = db_result(db_query("SELECT fieldmap FROM {salesforce_management_field_map} WHERE drupal = '%s'", $drupal_fieldmap_name));
		$record_type = $form_state['values']['record_type'];

		// re-add mappings
		foreach($form_state['values']['map']['salesforce'] as $key => $value) {
			if (!empty($value)) {
				$map[$key] = $value;
			}
		}
		
		// insert serialized map
		$name = $form['#account']->display_name . " (User field mapping)";
		
		if (empty($fieldmap_id)) {
			db_query("INSERT INTO {salesforce_management_field_map} (name, drupal, salesforce, recordtype_id, fields) VALUES ('%s', '%s', '%s', '%s', '%s')", $name, $drupal_fieldmap_name, $form_state['values']['object'], $record_type, serialize($map));
		}
		else {
			db_query("UPDATE {salesforce_management_field_map} SET name = '%s', drupal = '%s', salesforce = '%s', recordtype_id = '%s', fields = '%s' WHERE fieldmap  = %d", $name, $drupal_fieldmap_name, $form_state['values']['object'], $record_type, serialize($map), $fieldmap_id);
		}
			
		//db_query($sql, $name, $form['#node']->type . ':' . $form['#node']->nid, $form_state['values']['object'], serialize($map));
		drupal_set_message(t('Salesforce mapping has been saved. Please select a dedupe field if you want to prevent duplicate objects from being created in Salesforce.'));
	}
}


/**
 * Theme the mapping form
 * Based on theme_sf_webform_mapping_form()
 * @todo update this
 */
function theme_capwiz_connect_account_user_mapping($form) {	
	if ($form['map']['capwiz_field']) {
		
		$output = drupal_render($form['object']);
		$output .= drupal_render($form['record_type']);
		$map = $form['map'];

    $capwiz_fields = element_children($map['capwiz_field']);
    foreach ($capwiz_fields as $field_key) {
    	$row = array();
    	$row[] = array('data' => drupal_render($map['capwiz_field'][$field_key]), 'width' => '300');
    	$row[] = drupal_render($map['salesforce'][$field_key]);
    	$row[] = drupal_render($map['dedupe'][$field_key]);
    	$rows[] = $row;
    }
		
  	$header = array("Capwiz User Field","Salesforce Field","Dedupe Field");
  	$output .= theme('table', $header, $rows, array('id' => 'capwiz_connect_account_user_mapping'));

		// Remove map so it's not rendered twice
		unset($form['map']);
		
		$output .= drupal_render($form['submit']);
		$output .= drupal_render($form['remove']);
		$output .= drupal_render($form);
		return $output;
	}
}







/**
 * Action field mapping for Capwiz Connect account
 */
function capwiz_connect_account_action_mapping($form_state, $account) {
  $form = array();
  
  $form['action-mapping-form'] = array(
    '#value' => 'action mapping form',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  
  
  return $form;
}



/**
 * Confirm account delete
 */
function capwiz_connect_account_delete($form_state, $account) {
  if ($account->id > 0) {
    $form = array(
      '#account_id' => $account->id,
    );
    
    $cancel_path = 'admin/settings/capwiz_connect/accounts/' . $account->id;
    
    return confirm_form(
      $form,
      t('Are you sure you want to delete the account %display_name?', array('%display_name' => $account->display_name)),
      $cancel_path,
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel'),
      'capwiz_connect_account_delete');
  }
  else {
    drupal_set_message(t('That account does not exist.'), 'error');
    drupal_goto($cancel_path);
  }
}


/**
 * After confirming an account delete, actually delete it
 */
function capwiz_connect_account_delete_submit($form, &$form_state) {
  $account_id = $form['#account_id'];
  
  // Delete the account record
  db_query('DELETE FROM {capwiz_connect_accounts} WHERE id = %d', $account_id);
  
  // Delete any field mappings
  $drupal = 'capwiz_connect:' . $account_id . ':%';
  db_query("DELETE FROM {salesforce_management_field_map} where drupal LIKE '%s'", $drupal);
  
  drupal_set_message(t('The account has been deleted.'));
  $form_state['redirect'] = 'admin/settings/capwiz_connect/accounts';
}


function capwiz_connect_account_overview($account) {
  $connection = capwiz_connect($account->id);
  $connection->authenticate();
  if ($connection->isConnected()) {
    return '<div class="connected">Credentials OK</div>';
  }
  else {
    $error = $connection->getLastError();
    drupal_set_message($error, 'error');
    return '<div class="connection_error">Error connectnig to Capwiz.</div>';
  }
}




/**
 * General settings for Capwiz Connect API usage
 */
function capwiz_connect_settings($form_state) {
  $form = array();
  $form['capwiz_connect_standalone_cron_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable standalone cron.'),
    '#description' => t('If this option is enabled all Capwiz Connect related cron tasks will be removed from the standard cron run. These tasks will need to be cronned separately via !link', array('!link' => l('capwiz_connect_cron', 'capwiz_connect_cron'))),
    '#default_value' => variable_get('capwiz_connect_standalone_cron_enabled', 0),
  );
  $form = system_settings_form($form);

  // Need to clear the menu cache after saving so that the independent cron
  // callback will be available (or not, depending on selection)
  $form['#submit'][] = 'menu_rebuild';

  return $form;
}