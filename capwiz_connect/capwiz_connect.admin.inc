<?php
/**
 * Admin functions for Capwiz Connect integration
 */

/**
 * Functions to copy from sf_webform:
 * render_salesforce_object_fields()
 * 
 * sf_webform_remove_mapping
 * 
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


/**
 * User field mapping for Capwiz Connect account
 * Based on sf_webform_mapping_form()
 */
function capwiz_connect_account_user_mapping($form_state, $account) {
	$base = drupal_get_path('module', 'capwiz_connect');
	drupal_add_js($base .'/capwiz_connect.js');

	$form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'mapping' => array(),
  );

	// only show object selection for webforms that haven't been mapped
	if (empty($node->sf_webform_object)) {
		$form['object'] = array(
			'#type' => 'select',
			'#options' => _get_salesforce_objects(),
			'#default_value' => $node->sf_webform_object,
			'#title' => t('Salesforce Object'),
			'#description' => t('Choose the Salesforce object you want to create when the webform is submitted.'),
			'#ahah' => array(
			  'path' => 'js/capwiz_connect/salesforce-object-fields',
	      'wrapper' => 'map',
	      'effect' => 'fade',
	    ),
		);
	}
	else {
		$form['object'] = array(
			'#prefix' => t("<div>This webform is currently mapped to the <strong>$node->sf_webform_object</strong> object.</div>"), 
			'#type' => 'hidden',
			'#default_value' => $node->sf_webform_object,
			'#suffix' => t('<div>Use the <strong>Remove Map</strong> button below if you wish to map to a different Salesforce object.</div>'),
		);
	}

	$form['map'] = array(
    '#prefix' => '<div id="map">',
    '#suffix' => '</div>',
    '#value' => '&nbsp;',
  );
	
	if ($form_state['values']['object'] || !empty($node->sf_webform_object)) {
		$object = !empty($node->sf_webform_object) ? $node->sf_webform_object : $form_state['values']['object'];
		
		// get the fields in the object
		$object_details = _get_object_details($object);
		$fields = $object_details['fields'];
		$record_types = $object_details['record_types'];

		// create a drop down list friendly version of the sf fields
		foreach($fields as $key => $value) {
			$options[$key] = $value['label'];
		}
		
		$disabled = !empty($node->sf_webform_object) ? false : true;
		
		$form['record_type'] = array(
			'#type' => 'select',
			'#title' => t('Record Type'),
			'#description' => t('The type of !name to create', array('!name' => $node->sf_webform_object)),
			'#options' => $record_types,
			'#default_value' => $node->sf_webform_recordtype,
		);

		foreach($node->webform['components'] as $cid => $component) {
			if($component['type'] != 'fieldset') {
				// component
				$form['map']['cid'][$cid] = array(
				'#type' => 'item',
				'#description' => t($component['name']." - (".$component['type'].")"),
				);
				// salesforce field
				$form['map']['salesforce'][$cid] = array(
					'#type' => 'select',
					'#options' => $options,
					'#default_value' => $node->sf_webform_map[$cid],
				);
				
				if ($node->sf_webform_map[$cid] && ($fields[$node->sf_webform_map[$cid]]['idLookup'] || $fields[$node->sf_webform_map[$cid]]['externalId'])) {
					if ($node->sf_dedupe_key == $node->sf_webform_map[$cid]) {
						$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/remove-dedupe-field/' . $node->sf_webform_map[$cid] .'">Remove dedupe field</a>';
					}
					else {
						$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/set-dedupe-field/' . $node->sf_webform_map[$cid] .'">Set as dedupe field</a>';
					}
				}
				else {
					$value = '';
				}
				$form['map']['dedupe'][$cid] = array(
					'#value' => $value,
				);
			}
		}

		$form['node_map']['nid']['label'] = array(
			'#type' => 'item',
			'#description' => 'Node Id',
		);
		
		$form['node_map']['nid']['value'] = array(
			'#type' => 'select',
			'#options' => $options,
			'#default_value' => $node->sf_webform_map['nid'],
		);
		
		$form['node_map']['node_title']['label'] = array(
			'#type' => 'item',
			'#description' => 'Node Title',
		);
		
		$form['node_map']['node_title']['value'] = array(
			'#type' => 'select',
			'#options' => $options,
			'#default_value' => $node->sf_webform_map['node_title'],
		);
		
		$form['submission_map']['sid']['label'] = array(
			'#type' => 'item',
			'#description' => 'Submission Id',
		);
	
		$form['submission_map']['sid']['value'] = array(
			'#type' => 'select',
			'#options' => $options,
			'#default_value' => $node->sf_webform_map['sid'],
		);
		if ($node->sf_webform_map['sid'] && ($fields[$node->sf_webform_map['sid']]['idLookup'] || $fields[$node->sf_webform_map['sid']]['externalId'])) {
		  if ($node->sf_dedupe_key == $node->sf_webform_map['sid']) {
				$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/remove-dedupe-field/' . $node->sf_webform_map['sid'] .'">Remove dedupe field</a>';
			}
			else {
				$value = '<a class="webform-dedupe" href="/js/' . $node->nid . '/set-dedupe-field/' . $node->sf_webform_map['sid'] .'">Set as dedupe field</a>';
			}  
		}
		else {
			$value = '';
		}
		$form['map']['dedupe']['sid'] = array(
			'#value' => $value,
		);
		
		$form['submission_map']['submission_date']['label'] = array(
			'#type' => 'item',
			'#description' => 'Submission Date',
		);
	
		$form['submission_map']['submission_date']['value'] = array(
			'#type' => 'select',
			'#options' => $options,
			'#default_value' => $node->sf_webform_map['submission_date'],
		);
		
		$form['remove'] = array(
			'#type' => 'submit',
			'#value' => t('Remove Map'),
			'#submit' => array('sf_webform_remove_mapping'),
		);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Submit',
		);
	}
	return $form;
}



/** 
 * AHAH callback for mapping form
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
	
	foreach ($map['cid'] as $key=>$cid) {
		if (is_numeric($key)) {
			$row = array();
			$row[] = array('data' => drupal_render($map['cid'][$key]), 'width' => '300');
			$row[] = drupal_render($map['salesforce'][$key]);
			$row[] = drupal_render($map['dedupe'][$key]);
			$rows[] = $row;
		}
	}

	$header = array("Webform Component","Salesforce Field","Dedupe Field");

	$output .= theme('table', $header, $rows, array('id' => 'sf_webform_mapping_form'));
	
	foreach($form['node_map'] as $key=>$field) {
		if (substr($key, 0, 1) != '#') {
			$node_row = array();
			$node_row[] = array('data' => drupal_render($field['label']), 'width' => '300');
			$node_row[] = drupal_render($field['value']);
			$node_rows[] = $node_row;
		}
	}
	unset($form['node_map']);
	$output .= theme('table', array('Node Property', 'Salesforce Field'), $node_rows);
		
	foreach($form['submission_map'] as $key=>$field) {
		if (substr($key, 0, 1) != '#') {
			$row = array();
			$row[] = array('data' => drupal_render($field['label']), 'width' => '300');
			$row[] = drupal_render($field['value']);
			$submission_rows[] = $row;
		}
	}
	unset($form['submission_map']);
	$output .= theme('table', array('Submission Property', 'Salesforce Field'), $submission_rows);
	
	$output .= drupal_render($form['submit']);
  //$output = drupal_render($map);

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}


/**
 * Submit the user mapping form
 */
function capwiz_connect_account_user_mapping_submit($form, &$form_state) {
  if ($form_state['values']['map']['salesforce']) {

		$nid = $form['#node']->nid;
		$drupal_fieldmap_name = 'webform:' . $nid;
		$fieldmap_id = db_result(db_query("SELECT fieldmap FROM {salesforce_management_field_map} WHERE drupal = '%s'", $drupal_fieldmap_name));
		$record_type = $form_state['values']['record_type'];

		// re-add mappings
		foreach($form_state['values']['map']['salesforce'] as $key => $value) {
			if (!empty($value)) {
				$map[$key] = $value;
			}
		}
		
		// add node fields
		foreach($form_state['values']['node_map'] as $key => $value) {
			if (!empty($value['value'])) {
				$map[$key] = $value['value'];
			}
		}
		
		// add submission fields
		foreach($form_state['values']['submission_map'] as $key => $value) {
			if (!empty($value['value'])) {
				$map[$key] = $value['value'];
			}
		}
		
		// insert serialized map
		$name = $form['#node']->title . " (webform map)";
		
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