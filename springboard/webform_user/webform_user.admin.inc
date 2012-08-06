<?php

/**
 * @file
 * Administration page callbacks for the webform_user module.
 */


/**
 * Settings form for webform_user
 */
function webform_user_settings_form() {
  // get a list of all webform enabled node types
  $webform_enabled_types = variable_get('webform_node_types', array('webform'));

  // make a friendly list of options
  $options = array();
  foreach ($webform_enabled_types as $type) {
    $options[$type] = $node_types[$type]->name;
  }

  $form['webform_user_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Webform user enabled content types'),
    '#options' => $options,
    '#default_value' => variable_get('webform_user_node_types', array('webform')),
  );

  // add our own submit handler
  // TODO: remove submit handler.
  $form['#submit'][] = 'webform_user_settings_form_submit';

  return system_settings_form($form);
}

// TODO: remove this submit handler, unnecessary if we're only listing webform-enabled content types on the admin form.
/**
 * Submit handler for webform_user settings form.
 */
function webform_user_settings_form_submit($form, &$form_state) {
  // we need to add the selected node types to the webform_node_types_primary variable so that a webform record is added by default
  $webform_node_types_primary = variable_get('webform_node_types_primary', array('webform'));
  $webform_user_node_types = $form_state['values']['webform_user_node_types'];
  
  foreach ($webform_user_node_types as $type => $enabled) {
    if ($enabled && !in_array($type, $webform_node_types_primary)) {
      $webform_node_types_primary[] = $type;
    }
  }
  
  // re-save the variable
  variable_set('webform_node_types_primary', $webform_node_types_primary);
}

/**
 * User mapping form.
 *
 * Webform User can map webform components to user profile fields. Values added to the user profile for these fields
 * will be used to populate default values for webform components they are mapped to.
 *
 *  Example: we have a user profile field "First Name". We map this to a webform component "First Name" with a form key of first_name.
 *  Note that the name and form key of the component are irrelevant, all that matters is a relationship has been established between
 *  this profile field and this webform component.
 *
 *  If an anonymous user submits a webform a new user account is created using the email address provided. Whatever value the user entered
 *  in the "First Name" webform component will be used to set the value of the "First Name" profile field as well.
 *
 *  For existing users, mapped profile fields are used to provide default values for webform components. For example, given the mapping
 *  described above, if an existing user views a webform, the "First Name" component field will contain a default value that matches
 *  the value in the "First Name" field in that user's profile.
 */
function webform_user_mapping_form($form, $form_state, $node) {

  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'mapping' => array(),
  );
  
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  
  $form['info'] = array(
    '#value' => 'This form allows you to map the submitted form values to Drupal user and profile fields. When the webform form is submitted, the user\'s profile will be updated with the values from the form.',
    '#weight' => 0,
  );

  $header = array(
    t('Webform Component'),
    t('Profile Field'),
  );
  
  // The NULL option and any core account fields have to be handled separately now that profile fields
  // are implemented as entity fields.
  $options = array(
    '' => '--Select One--',
    'mail' => t('E-mail address'),
  );
  
  $profile_fields = _webform_user_get_profile_fields();
  foreach($profile_fields as $field) {
    $options[$field['name']] = $field['title'];
  }
  
  $form['mapping'] = array(
    '#prefix' => '<div id="fields">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => array(),
  );
  

 
  foreach ($node->webform['components']  as $cid => $component) {
    if (isset($node->map[$cid])) {
      $default = $node->map[$cid];
    }
    else {
      $default = 0;
    }
    if ($component['type'] !== 'fieldset') {
      $select = array(
        '#id' => 'component-' . $cid,
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $default,
      );
      
      $form['mapping'][$cid] = array(
        'component_cid' => &$select,
      );
      
      $form['mapping']['#rows'][] = array(
        array('data' => $component['name']),
        array('data' => &$select),
      );
      unset($select);
    }
    
  }
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Submit handler for user profile mapping form.
 */
function webform_user_mapping_form_submit($form, &$form_state) {
 
  $query = db_delete('webform_user_component_map');
  $query->condition('nid', $form['#node']->nid);
  $query->execute();

  foreach ($form_state['values']['mapping'] as $key => $values) {
    if (!empty($values['component_cid'])) {
      
      $query = db_insert('webform_user_component_map');
      $query->fields(array(
        'nid' => $form['#node']->nid,
        'cid' => $key,
        'map_id' => $values['component_cid'],
      ));
      $query->execute();
    }
  }
  
   drupal_set_message(t('Profile mapping has been saved'));
}

