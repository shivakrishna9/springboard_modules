<?php
/**
 * Admin functions for Capwiz Connect integration
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
      l('test', 'admin/settings/capwiz_connect/accounts/' . $row->id . '/test'),
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
 */
function capwiz_connect_account_user_mapping($form_state, $account) {
  // Load the fieldmap from the database.
   $map = salesforce_management_api_fieldmap_load($fieldmap);
   // Return to the admin page if the fieldmap did not exist.
   if (empty($map)) {
     drupal_set_message(t('That fieldmap does not exist.'), 'error');
   }

   // Include the CSS file for the form.
   drupal_add_css(drupal_get_path('module', 'capwiz_connect') .'/capwiz_connect.admin.css');

   $form = array();

   $form['salesforce_recordtype_wrapper'] = array(
     '#prefix' => '<div id="recordtype">',
     '#suffix' => '</div>',
   );

   $recordtypes = array(); //salesforce_management_api_get_object_recordtypes($map['salesforce']);

   $form['salesforce_recordtype_wrapper']['salesforce_recordtype'] = array(
     '#type' => 'select',
     '#title' => 'Salesforce record type',
     '#options' => count($recordtypes) > 1 ? $recordtypes : array(t('None available')),
     '#description' => t('Objects using this fieldmap will be assigned to the selected record type.'),
     '#default_value' => $map['recordtype_id'],
   );

   // Add the index to the form array.
   $form['fieldmap_index'] = array(
     '#type' => 'value',
     '#value' => $fieldmap,
   );

   // Add a description of the source fieldmap to the form array.
   $form['fieldmap_desc'] = array(
     '#value' => '<p>Fieldmap description</p>',
   );

   $source = array(
     'label' => t('Capwiz label'),
     'fields' => array(
       'uno' => array('label' => 'Capwiz field uno'),
       'dos' => array('label' => 'Capwiz field dos'),
      )
    );
   $target = array(
      'label' => t('Capwiz target'),
      'fields' => array(
        'one' => array('label' => 'Capwiz field 1'),
        'two' => array('label' => 'Capwiz field 2'),
       )
     );


  foreach($target['fields'] as $key => $value){
 	  $fieldmapsort[$key] = array('label'=>$value['label'], 'type'=>$value['type']);
  }
  asort($fieldmapsort);

  // Add the data to the form for the required fields table.
  $form['fields'] = array(
    '#theme' => 'salesforce_management_api_fieldmap_edit_form_table',
    'rows' => array(),
  );

  $form['fields']['header'] = array(
    array('#value' => t('Target: @label (@type)', array('@label' => $target['label'], '@type' => $map['action'] == 'import' ? t('Drupal') : t('Salesforce')))),
    array('#value' => t('Source: @label (@type)', array('@label' => $source['label'], '@type' => $map['action'] == 'import' ? t('Salesforce') : t('Drupal')))),
    array('#value' => t('Business rules Drupal to SF')),
    array('#value' => t('Business rules SF to Drupal')),
  );

  // Loop through each of the target fields.
  $rows = array('required' => array(), 'optional' => array());
  foreach ($fieldmapsort as $key => $value) {
    // Determine to which table this field should belong.
    if ($value['type'] == SALESFORCE_FIELD_REQUIRED) {
      $type = 'required';
      $required = ' <span class="form-required" title="'. t('This field is required.') .'">*</span>';
    }
    else {
      $type = 'optional';
      $required = '';
    }

    // Create a row for this field.
    $row = array(
      'target' => array('#value' => $value['label'] . $required),
    );

    // Add the select list for the associated target field.
    $row['source'][$key] = array(
      '#type' => 'select',
      '#title' => $value['label'],
      '#options' => salesforce_management_api_fieldmap_field_options($source),
      '#default_value' => $map['fields'][$key],
      '#required' => $type == 'required',
    );
    $row['business'][$key.'_r'] = array(
      '#type' => 'select', 
      '#title' => t('Default options'),
      '#default_value' => $map['business'][$key],
      '#options' => array(
        'null' => t(''), 
        'blank' => t('Overwrite when blank'), 
        'append' => t('Append'),
        'always' => t('Always overwrite'),
        'never' => t('Never overwrite'),
      ),
    );
   	$row['businessSD'][$key.'_l'] = array(
      '#type' => 'select', 
      '#title' => t('Default options'),
   	  '#default_value' => $map['businessSD'][$key],
   	  '#options' => array(
   		'null' => t(''), 
   		'blank' => t('Overwrite when blank'), 
   		'append' => t('Append'),
   		'always' => t('Always overwrite'),
   		'never' => t('Never overwrite'),
     ),
   	);

     // Add the row to the correct rows array.
     $rows[$type][] = $row;
   }

  // Combine the rows arrays into one with required fields displayed first.
  $form['fields']['rows'] = array_merge($rows['required'], $rows['optional']);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
    '#suffix' => l(t('Cancel'), 'admin/settings/capwiz_connect/accounts/' . $account->id),
  );

  return $form;
}

function capwiz_connect_account_user_mapping_submit($form, &$form_state) {
  // Load the fieldmap from the database.
  $map = salesforce_management_api_fieldmap_load($form_state['values']['fieldmap_index']);
  
  // Update name, record type, etc.
  $map['name'] = $form_state['values']['name'];
  $map['recordtype_id'] = $form_state['values']['salesforce_recordtype'];
  // Reset the fields array on the fieldmap.
  $map['fields'] = array();

  // Get the object definition for the target object.
  if ($map['action'] == 'import') {
    $object = salesforce_management_api_fieldmap_objects_load('drupal', $map['drupal']);
  }
  else {
    $object = salesforce_management_api_fieldmap_objects_load('salesforce', $map['salesforce']);
  }

  // Loop through all the fields on the object.
  foreach (array_keys($object['fields']) as $field) {
    // If a field has been mapped to this field on the form...
    if (!empty($form_state['values'][$field])) {
      // Add the association to the fieldmap's fields array.
			$map['fields'][$field] = $form_state['values'][$field];
	 		$map['business'][$field] = $form_state['values'][$field.'_r'];
	 		$map['businessSD'][$field] = $form_state['values'][$field.'_l'];
			$map['validation'][$field] = array(
				'field_type' => $object['fields'][$field]['field_type'],
				'nillable' => $object['fields'][$field]['nillable'],
				'length' => $object['fields'][$field]['length'],
			);
	 	}
  }

  // Save the updated fieldmap.
  salesforce_management_api_fieldmap_save($map);

  // Display a message and return to the admin page.
  drupal_set_message(t('The changes have been saved.'));

  $form_state['redirect'] = SALESFORCE_PATH_FIELDMAPS;
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
  db_query('DELETE FROM {capwiz_connect_accounts} WHERE id = %d', $account_id);
  drupal_set_message(t('The account has been deleted.'));
  $form_state['redirect'] = 'admin/settings/capwiz_connect/accounts';
}


function capwiz_connect_account_overview($account) {
  return $account->display_name;
}


/**
 * Test Connection tab on account
 */
function capwiz_connect_account_test_connection($account) {
  $connection = capwiz_connect($account->id);
  $connection->authenticate();
  return $connection->getLastError();
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