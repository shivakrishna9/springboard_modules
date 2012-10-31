<?php
/**
 * @file
 * Contains the admin page callbacks for the Salesforce module, including forms
 *   for general settings and fieldmap administration.
 */

// The settings form at admin/settings/salesforce.
function salesforce_management_api_settings_form() {
  $form = array();

  // Use the username field to collapse the API settings fieldset.
  $username = variable_get('salesforce_management_api_username', '');

  $form['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Salesforce API settings'),
    '#description' => t('Use your Salesforce.com login information for these username and password fields.'),
    '#collapsible' => !empty($username),
    '#collapsed' => !empty($username),
    '#weight' => -10,
  );
  $form['api']['salesforce_management_api_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Should be in the form of an e-mail address.'),
    '#default_value' => variable_get('salesforce_management_api_username', ''),
    '#required' => TRUE,
  );
  $form['api']['salesforce_management_api_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => variable_get('salesforce_management_api_password', ''),
  );
  $form['api']['salesforce_management_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Security token'),
    '#description' => t('Set your security token by logging into Salesforce and navigating to Setup > My Personal Information > Reset My Security Token.'),
    '#default_value' => variable_get('salesforce_management_api_token', ''),
  );

  $form['log'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -9,
  );
  $form['log']['salesforce_management_api_activity_log'] = array(
    '#type' => 'radios',
    '#title' => t('Activity log level'),
    '#options' => array(
      SALESFORCE_LOG_NONE => t('Do not log any Salesforce activities.'),
      SALESFORCE_LOG_SOME => t('Log important Salesforce activities.'),
      SALESFORCE_LOG_ALL => t('Log all Salesforce activitiies.'),
    ),
    '#default_value' => variable_get('salesforce_management_api_activitiy_log', SALESFORCE_LOG_SOME),
  );
  $form['log']['salesforce_management_api_error_log'] = array(
    '#type' => 'radios',
    '#title' => t('Error log level'),
    '#options' => array(
      SALESFORCE_LOG_NONE => t('Do not log any Salesforce errors.'),
      SALESFORCE_LOG_SOME => t('Log important Salesforce errors.'),
      SALESFORCE_LOG_ALL => t('Log all Salesforce errors.'),
    ),
    '#default_value' => variable_get('salesforce_management_api_error_log', SALESFORCE_LOG_ALL),
  );

  // Validate handler makes sure that the salesforce_management_api_password doesn't get set to null on accident
  $form['#submit'][] = 'salesforce_management_api_settings_form_submit';
  return system_settings_form($form);
}

// Settings form submit handler so that password doesn't get deleted.
function salesforce_management_api_settings_form_submit($form, &$form_state) {
  // If the user hit "Save Configuration" and the required field
  // salesforce_management_api_password is blank, try to get it from variables
  if ($form_state['values']['submit'] == $form_state['values']['op'] and empty($form_state['values']['salesforce_management_api_password'])) {
    $pass = variable_get('salesforce_management_api_password', FALSE);
    if (isset($pass)) {
      $form_state['values']['salesforce_management_api_password'] = $pass;
    }
  }
}

// Displays an admin table for fieldmaps.
function salesforce_management_api_fieldmap_admin() {
  // Define the header for the admin table.
  $header = array(t('Index'), t('Name'), t('Drupal object'), t('Salesforce object'), array('data' => t('Operations'), 'colspan' => 4));
  $rows = array();

  // Loop through all the indexed field maps.
  $result = db_query("SELECT fieldmap, name, drupal, salesforce FROM {salesforce_management_field_map}");
  while ($map = db_fetch_array($result)) {
    // Add the row to the table with the basic operations.
    $rows[] = array(
      $map['fieldmap'],
			$map['name'],
      salesforce_management_api_fieldmap_object_label('drupal', $map['drupal']),
      salesforce_management_api_fieldmap_object_label('salesforce', $map['salesforce']),
      l(t('edit'), SALESFORCE_PATH_FIELDMAPS .'/'. $map['fieldmap'] .'/edit'),
      l(t('delete'), SALESFORCE_PATH_FIELDMAPS .'/'. $map['fieldmap'] .'/delete'),
	  	l(t('dedupe'), SALESFORCE_PATH_FIELDMAPS .'/'. $map['fieldmap'] .'/dedupe'),
	  	l(t('import'), SALESFORCE_PATH_FIELDMAPS .'/'. $map['fieldmap'] .'/import'),
    );
  }

  // Add a message if no objects have been mapped.
  if (count($rows) == 0) {
    $rows[] = array(
      array('data' => t('You have not yet defined any fieldmaps.'), 'colspan' => 8),
    );
  }

  return theme('table', $header, $rows);
}

// Displays the form to add a fieldmap.
function salesforce_management_api_fieldmap_add_form(&$form_state) {
  $form = array();
	$form['#cache'] = TRUE;
	
  // Build an options array out of the Drupal objects.
  $options = array();

  foreach (salesforce_management_api_fieldmap_objects_load('drupal') as $key => $value) {
    $options[$key] = $value['label'];
  }

	array_unshift($options, '-- Select --');

	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name'),
		'#description' => t('Enter a recognizable name for the fieldmap.'),
		'#required' => TRUE,
	);

  $form['drupal_object'] = array(
    '#type' => 'select',
    '#title' => t('Drupal object'),
    '#options' => count($options) > 0 ? $options : array(t('None available')),
    '#disabled' => count($options) == 0,
    '#required' => TRUE,
		'#ahah' => array(
		  'path' => 'js/select-drupal-object',
      'wrapper' => 'shizzle',
      'effect' => 'fade',
			'method' => 'replace',
    ),
  );

	// Container for just the poll choices.
  $form['role_wrapper'] = array(
    '#prefix' => '<div id="shizzle">',
    '#suffix' => '</div>',
  );

	if ($form_state['values']['drupal_object'] == 'user') {
		$drupal_roles = user_roles(TRUE);
		$form['role_wrapper']['user_role'] = array(
			'#type' => 'select',
			'#title' => 'User role',
			'#options' => $drupal_roles,
			'#description' => t('This fieldmap will be assigned to users in the selected role.'),
		);
	}
	else {
		$form['role_wrapper']['user_role'] = array(
			'#type' => 'hidden',
		);
	}

  // Build an options array out of the Salesforce objects.
  $options = array();

  foreach (salesforce_management_api_fieldmap_objects_load('salesforce') as $key => $value) {
    $options[$key] = $value['label'];
  }

  $form['salesforce_management_object'] = array(
    '#type' => 'select',
    '#title' => t('Salesforce object'),
    '#options' => count($options) > 0 ? $options : array(t('None available')),
    '#disabled' => count($options) == 0,
    '#required' => TRUE,
		'#ahah' => array(
		  'path' => 'js/select-salesforce-object',
      'wrapper' => 'recordtype',
      'effect' => 'fade',
			'method' => 'replace',
    ),
  );

	
	$form['salesforce_recordtype_wrapper'] = array(
		'#prefix' => '<div id="recordtype">',
		'#suffix' => '</div>',
	);
	
	if (!empty($form_state['values']['salesforce_management_object'])) {
		$recordtypes = salesforce_management_api_get_object_recordtypes($form_state['values']['salesforce_management_object']);
	}
	else {
		$recordtypes = salesforce_management_api_get_object_recordtypes(array_shift($options));
	}
	
	$form['salesforce_recordtype_wrapper']['salesforce_recordtype'] = array(
		'#type' => 'select',
		'#title' => 'Salesforce record type',
		'#options' => count($recordtypes) > 1 ? $recordtypes : array(t('None available')),
		'#description' => t('Objects using this fieldmap will be assigned to the selected record type.'),
	);

  $form['automatic'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically Populate Salesforce?'),
    '#return_value' => 1,
    '#default_value' => 0,
    '#description' => t('Automatically create and link new salesforce objects when Drupal objects are created?'),
  );

  $form['submit'] = array(
		'#submit' => array('salesforce_management_fieldmap_submit'),
    '#type' => 'submit',
    '#value' => t('Map object fields'),
    '#suffix' => l(t('Cancel'), SALESFORCE_PATH_FIELDMAPS),
  );

  return $form;
}

// Submit handler for a new fieldmap
function salesforce_management_fieldmap_submit($form, &$form_state) {
  // Create the new fieldmap.
	$name = $form_state['values']['name'];
	$rid = $form_state['values']['user_role'];
	$recordtype_id = $form_state['values']['salesforce_recordtype'];

  $index = salesforce_management_api_fieldmap_create($form_state['values']['drupal_object'], $form_state['values']['salesforce_management_object'], $form_state['values']['automatic'], $name, $rid, $recordtype_id);

  // Redirect to its edit form.
  $form_state['redirect'] = SALESFORCE_PATH_FIELDMAPS .'/'. $index .'/edit';

}

/**
 * Creates a new fieldmap in the database and returns its index.
 *
 * @param $drupal
 *   The name of a Drupal object.
 * @param $salesforce
 *   The name of a Salesforce object.
 * @return
 *   The numeric index of the new fieldmap.
 */
function salesforce_management_api_fieldmap_create($drupal, $salesforce, $automatic, $name, $rid = null, $recordtype_id = null) {
  // Create the fieldmap array.
  $map = array(
    'drupal' => $drupal,
    'salesforce' => $salesforce,
    'automatic' => $automatic,
    'fields' => array(),
		'name' => $name,
		'rid' => $rid,
		'recordtype_id' => $recordtype_id,
  );

  // Save the new fieldmap.
  salesforce_management_api_fieldmap_save($map);

  return $map['fieldmap'];
}

// Displays the confirm form for deleting a fieldmap.
function salesforce_management_api_fieldmap_delete_form(&$form_state, $fieldmap) {
  // Load the fieldmap from the database.
  $map = salesforce_management_api_fieldmap_load($fieldmap);

  // Return to the admin page if the fieldmap did not exist.
  if (empty($map)) {
    drupal_set_message(t('That fieldmap does not exist.'), 'error');
    drupal_goto(SALESFORCE_PATH_FIELDMAPS);
  }

  $form = array();

  // Add the fieldmap to the form array.
  $form['fieldmap_index'] = array(
    '#type' => 'value',
    '#value' => $fieldmap,
  );

  // Build the description text for this fieldmap.
  if ($map['action'] == 'import') {
    $desc = t('Fieldmap @index maps Salesforce %salesforce objects to Drupal %drupal objects for import.', array('@index' => $map['fieldmap'], '%drupal' => $map['drupal'], '%salesforce' => $map['salesforce']));
  }
  else {
    $desc = t('Fieldmap @index maps Drupal %drupal objects to Salesforce %salesforce objects for export.', array('@index' => $map['fieldmap'], '%drupal' => $map['drupal'], '%salesforce' => $map['salesforce']));
  }

  return confirm_form($form, t('Are you sure you want to delete this fieldmap?'), SALESFORCE_PATH_FIELDMAPS, $desc, t('Delete'));
}

function salesforce_management_api_fieldmap_delete_form_submit($form, &$form_state) {
  // Delete the specified fieldmap.
  salesforce_management_api_fieldmap_delete($form_state['values']['fieldmap_index']);

  // Display a message and return to the admin screen.
  drupal_set_message(t('The fieldmap has been deleted.'));

  $form_state['redirect'] = SALESFORCE_PATH_FIELDMAPS;
}

// Displays the edit form for adding field associations to a fieldmap.
function salesforce_management_api_fieldmap_edit_form(&$form_state, $fieldmap) {
  // Load the fieldmap from the database.
  $map = salesforce_management_api_fieldmap_load($fieldmap);
  // Return to the admin page if the fieldmap did not exist.
  if (empty($map)) {
    drupal_set_message(t('That fieldmap does not exist.'), 'error');
    drupal_goto(SALESFORCE_PATH_FIELDMAPS);
  }

  // Include the CSS file for the form.
  drupal_add_css(drupal_get_path('module', 'salesforce_management_api') .'/salesforce_management_api.admin.css');

  $form = array();

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Enter a recognizable name for the fieldmap.'),
    '#default_value' => $map['name'],
    '#required' => TRUE,
  );

  $form['salesforce_recordtype_wrapper'] = array(
    '#prefix' => '<div id="recordtype">',
    '#suffix' => '</div>',
  );

  $recordtypes = salesforce_management_api_get_object_recordtypes($map['salesforce']);

  $form['salesforce_recordtype_wrapper']['salesforce_recordtype'] = array(
    '#type' => 'select',
    '#title' => 'Salesforce record type',
    '#options' => count($recordtypes) > 1 ? $recordtypes : array(t('None available')),
    '#description' => t('Objects using this fieldmap will be assigned to the selected record type.'),
    '#default_value' => $map['recordtype_id'],
  );

  $form['automatic'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically Populate Salesforce?'),
    '#return_value' => 1,
    '#default_value' => $map['automatic'],
    '#description' => t('Automatically create and link new salesforce objects when Drupal objects are created?'),
  );
  
  // Add the index to the form array.
  $form['fieldmap_index'] = array(
    '#type' => 'value',
    '#value' => $fieldmap,
  );

  // Add a description of the source fieldmap to the form array.
  $form['fieldmap_desc'] = array(
    '#value' => '<p>'. salesforce_management_api_fieldmap_description($map) .'</p>',
  );

  $source = salesforce_management_api_fieldmap_objects_load('drupal', $map['drupal']);
  $target = salesforce_management_api_fieldmap_objects_load('salesforce', $map['salesforce']);
  

 foreach($target['fields'] as $key => $value){
	$fieldmapsort[$key] = array('label'=>$value['label'], 'type'=>$value['type']);
	
  }
 asort($fieldmapsort);
  
  // Fail with an error message if either the source or target object
  // definitions were not found.
  if (empty($source) || empty($target)) {
    drupal_set_message(t('This fieldmap cannot be edited, because an object definition could not be found.'), 'error');
    drupal_goto(SALESFORCE_PATH_FIELDMAPS);
  }

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
    // Skip fields that are automatically assigned upon create for now.
    /*if ($value['type'] == SALESFORCE_FIELD_SOURCE_ONLY || $value['label'] == 'Owner ID') {
      continue;
    }*/
    
    /*echo "<pre>";
    print_r($value);
    echo "</pre>";*/

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
    '#suffix' => l(t('Cancel'), SALESFORCE_PATH_FIELDMAPS),
  );

  return $form;
}

function salesforce_management_api_fieldmap_edit_form_submit($form, &$form_state) {
  // Load the fieldmap from the database.
  $map = salesforce_management_api_fieldmap_load($form_state['values']['fieldmap_index']);
  
  // Update name, record type, etc.
  $map['name'] = $form_state['values']['name'];
  $map['recordtype_id'] = $form_state['values']['salesforce_recordtype'];
  $map['automatic'] = $form_state['values']['automatic'];
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

function theme_salesforce_management_api_map_user_role_form($form) {
	$header = array('Drupal Role', 'Fieldmap');
	foreach ($form['fieldmap']['rid'] as $key=>$rid) {
		if (is_numeric($key)) {
			$row = array();
			$row[] = drupal_render($form['fieldmap']['rid'][$key]);
			$row[] = drupal_render($form['fieldmap']['map'][$key]);
			$rows[] = $row;
		}
	}

	$output .= theme('table', $header, $rows, array('id' => 'salesforce_management_api_map_user_role_form'));
	$output .= drupal_render($form);
	return $output;
}

// Themes the field associations on a fieldmap edit form into a table.
function theme_salesforce_management_api_fieldmap_edit_form_table($form) {
  // Build the header array.
  $header = array();

  foreach (element_children($form['header']) as $element) {
    $header[] = drupal_render($form['header'][$element]);
  }
  

  // Build the rows array.
  $rows = array();

  foreach (element_children($form['rows']) as $element) {
    $rows[] = array(
      drupal_render($form['rows'][$element]['target']),
      array(
        'data' => drupal_render($form['rows'][$element]['source']), 
        'class' => 'source-cell'
      ),
      array(
        'data' => drupal_render($form['rows'][$element]['business']), 
        'class' => 'source-cell'
      ),
	  array(
        'data' => drupal_render($form['rows'][$element]['businessSD']), 
        'class' => 'source-cell'
      ),
    );
    
 }
	
  // Add a message if no rows were found.
  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('There are no fields of this type to set.'), 'colspan' => 4),
    );
  }

  // Build the attributes array.
  $attributes = array();
  

  // Build the caption.
  $caption = NULL;

  if (isset($form['caption'] )) {
    $caption = drupal_render($form['caption']);
  }

  return theme('table', $header, $rows, $attributes, $caption);
}

//Ability to set dedupe key for a mapping of users to a fieldmap.
function salesforce_management_api_fieldmap_dedupe_form(&$form_state, $fieldmap) {
   // Load the fieldmap from the database.
  $map = salesforce_management_api_fieldmap_load($fieldmap);
  $object = salesforce_management_api_fieldmap_objects_load('salesforce', $map['salesforce']);

  // Return to the admin page if the fieldmap did not exist.
  if (empty($map)) {
    drupal_set_message(t('That fieldmap does not exist.'), 'error');
    drupal_goto(SALESFORCE_PATH_FIELDMAPS);
  }
  $form = array();
  // Add the index to the form array.
  $form['fieldmap_index'] = array(
    '#type' => 'value',
    '#value' => $fieldmap,
  );
  // Loop through each of the target fields.
  $field = array();
  foreach ($object['fields'] as $key => $value) {
        /*echo  "<pre>";
        print_r($value);
        echo "</pre>";*/
        
	$field[$key] = $value['label'];
  }
  asort($field);
  $form['fieldmap_options'] = array(
	  '#type' => 'select', 
	  '#title' => t('Dedupe key for mapping'), 
	  '#default_value' =>  isset($map['dedupe_key']) ? $map['dedupe_key'] : '',
	  '#options' => $field,
	  '#description' => t('Fieldmap maps Salesforce objects to Drupal objects.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
    '#suffix' => l(t('Cancel'), SALESFORCE_PATH_FIELDMAPS),
  );
  return $form;
}
function salesforce_management_api_fieldmap_dedupe_form_submit($form, &$form_state) {
  // Update the fieldmap into the database.
  $fieldmap = $form_state['values']['fieldmap_index'];
  $dedupe_key = $form_state['values']['fieldmap_options'];
  db_query("UPDATE {salesforce_management_field_map} SET dedupe_key = '%s' WHERE fieldmap = %d", $dedupe_key, $fieldmap);
}


/**
 * Show form for admin to upload a new WSDL file
 */
function salesforce_management_api_update_wsdl_form() {
  $form = array();
  
  // Need to set the form type so file upload will work
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['salesforce_management_api_wsdl_file_path'] = array(
    '#type' => 'textfield',
    '#title' => t('File System Path'),
    '#description' => t('The location on the server where your WSDL file will be saved, defaults to the Drupal files directory.'),
    '#default_value' => variable_get('salesforce_management_api_wsdl_file_path', file_directory_path()),
  );
  $form['salesforce_management_api_wsdl_file'] = array(
    '#type' => 'file',
    '#title' => t('WSDL File'),
    '#description' => t('Upload the new WSDL definition file. The name of the file is irrelevent, but it must end with an XML or WSDL extension.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload WSDL'),
  );
  
  return $form;
}


/**
 * Validate the WSDL file upload - check upload error code and file extension
 */
function salesforce_management_api_update_wsdl_form_validate($form, &$form_state) {
  $source = 'salesforce_management_api_wsdl_file';

  if (isset($_FILES['files']) && $_FILES['files']['name'][$source] && is_uploaded_file($_FILES['files']['tmp_name'][$source])) {
    // Check for file upload errors and return FALSE if a
    // lower level system error occurred.
    switch ($_FILES['files']['error'][$source]) {
      // @see http://php.net/manual/en/features.file-upload.errors.php
      case UPLOAD_ERR_OK:
        break;

      case UPLOAD_ERR_INI_SIZE:
      case UPLOAD_ERR_FORM_SIZE:
        drupal_set_message(t('The file %file could not be saved, because it exceeds %maxsize, the maximum allowed size for uploads.', array('%file' => $source, '%maxsize' => format_size(file_upload_max_size()))), 'error');
        return 0;

      case UPLOAD_ERR_PARTIAL:
      case UPLOAD_ERR_NO_FILE:
        drupal_set_message(t('The file %file could not be saved, because the upload did not complete.', array('%file' => $source)), 'error');
        return 0;

        // Unknown error
      default:
        drupal_set_message(t('The file %file could not be saved. An unknown error has occurred.', array('%file' => $source)), 'error');
        return 0;
    }
    
    // Check the file extension
    $pathParts = pathinfo($_FILES['files']['name'][$source]);
    if(!in_array(strtolower($pathParts['extension']), array('xml', 'wsdl'))) {
      form_set_error($source, t('File upload error: invalid file extension. Please upload a file with an XML or WSDL  extension.'));
      return 0;
    }
  }
}

/**
 * Process form when an admin uploads a WSDL file.
 * This function SHOULD use file_save_upload(), but unfortunately, that function calls 
 * file_create_path() which calls file_check_location() - which fails on any destination 
 * dir that isn't in the default file upload dir. And since the enterprise.wsdl.xml file 
 * needs to go into a sub-dir in the module directory, that won't do.
 */
function salesforce_management_api_update_wsdl_form_submit($form, &$form_state) {

  variable_set('salesforce_management_api_wsdl_file_path', $form_state['values']['salesforce_management_api_wsdl_file_path']);

  // Find the dir the file should go into
  $dir = variable_get('salesforce_management_api_wsdl_file_path', file_directory_path());
  // Upload file path
  $file = $dir . '/enterprise.wsdl.xml';

  // Array key for the file element
  $source = 'salesforce_management_api_wsdl_file';
  
  // Make sure the directory is writeable
  if( file_check_directory( $dir ) ) {  
    // Try to move the uploaded file into the right place
    if (!move_uploaded_file($_FILES['files']['tmp_name'][$source], $file)) {
         form_set_error($source, t('File upload error. Could not move uploaded file.'));
         watchdog('file', 'Upload error. Could not move uploaded file to destination %destination.', array('%destination' => $dir));
         return 0;
    }
    else {
      drupal_set_message('The WSDL file has been successfully uploaded');
      variable_set('salesforce_management_api_wsdl_updated', time());
      // Clear the cache
      drupal_flush_all_caches();
      
      // Clear WSDL files from tmp directory
      $tmpDir = file_directory_temp();
      // Make sure the tmp dir exists and isn't the root, just to be safe
      if(is_dir($tmpDir) && $tmpDir != '/') {
        $cmd = 'rm -f ' . $tmpDir . '/*.wsdl.*';
        $execOutput = exec($cmd);
      }
      
      drupal_set_message('Drupal cache emptied and WSDL files removed from temp directory.');
    }
  } 
  else {
    drupal_set_message( t('WARNING: server directory is not writeable. Please contact a site admin to correct this.'), 'error' );
    watchdog('salesforce_management_api', 'WSDL file upload failed because directory is not writeable: %directory', array('%directory'=>$dir), WATCHDOG_ERROR);
    return 0;
  }
}


/******************************************************************************
 * AHAH FUNCTIONS
 ******************************************************************************/
function salesforce_management_api_drupal_object_select() {
	$form_state = array('storage' => NULL, 'submitted' => TRUE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
	
	$role_field = $form['role_wrapper'];
  unset($role_field['#prefix'], $role_field['#suffix']); // Prevent duplicate wrappers.
	$output = drupal_render($role_field);
	
  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();
}

function salesforce_management_api_salesforce_object_select() {
	$form_state = array('storage' => NULL, 'submitted' => TRUE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
	
	$recordtype_field = $form['salesforce_recordtype_wrapper'];
  unset($recordtype_field['#prefix'], $recordtype_field['#suffix']); // Prevent duplicate wrappers.
	$output = drupal_render($recordtype_field);
	
  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output));
  exit();	
}