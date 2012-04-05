<?php

/**
 * @file
 * User profile code for fundraiser_webform.
 * THIS CODE DUPLICATES MUCH OF WEBFORM_USER.
 * Future work on this module should include a merger of these two pieces of code.
 */

/**
 * Profile stuff. This may need to go into another module.
 */
 
/**
 * DB function, creates a map of profile fields with their corrosponding values.
 */
//TODO figure out where this is called from.
function _fundraiser_webform_create_profile_values_map($nid, $sid) {
  $map = array();
  $results = db_query('SELECT f.map_id, d.data FROM {fundraiser_component_map} f ' .
    'INNER JOIN {webform_submitted_data} d on d.cid = f.cid ' .
    'WHERE f.nid = :nid and d.sid = :sid', array(':nid'=> $nid, ':sid' => $sid));
  foreach ($results as $row) {
    $map[$row->map_id] = $row->data;
  }
  return $map;
}

/**
 * DB function, version of _profile_get_fields that includes hidden profile fields by default.
 */
function _fundraiser_webform_profile_get_fields($category) {
  $query = db_select('profile_field');
  $query->condition('category', db_like($category), 'LIKE');
  return $query
    ->fields('profile_field')
    ->orderBy('category', 'ASC')
    ->orderBy('weight', 'ASC')
    ->execute();
}

/**
 * DB function, version of profile_save_profile that includes hidden profile fields
 */

/**
 * Helper function to fetch fids for the country and state profile fields.
 *
 * @return
 * Returns a keyed array of location fields or an empty array if there was an error.
 */
function _fundraiser_webform_get_profile_location_fields() {
  $results = db_query('SELECT fid, name FROM {profile_field} WHERE name IN (\'profile_country\', \'profile_state\')');
  foreach ($results as $profile_field) {
    if ($profile_field->name == 'profile_country') {
      $country_fid = $profile_field->fid;
    }
    if ($profile_field->name == 'profile_state') {
      $state_fid = $profile_field->fid;
    }
  }
  return array('country' => $country_fid, 'state' => $state_fid);
}

/**
 * Helper function to update user profile fields based on contry/state selected on the donation form.
 *
 *  @param $submission
 *  Webform submission object.
 *
 *  @param $update_user
 *  User object to be updated. If NULL, defaults to global $user
 *
 */
// TODO figure out where this is called from.
function _fundraiser_webform_update_profile_location($submission, $update_user = NULL) {
  // make sure we have a real user
  if (empty($update_user) || $update_user->uid == 0) {
    return;
  }
  $fids = _fundraiser_webform_get_profile_location_fields();
  // get the ids of the country and state webform components for node
  $country_cid = db_query('SELECT cid FROM {webform_component} '.
    'WHERE nid = :nid AND form_key = :form_key',
    array(':nid' => $submission->nid, ':form_key' => 'country'))->fetchField();
  $state_cid = db_query('SELECT cid FROM {webform_component} '.
    'WHERE nid = :nid AND form_key = :form_key',
    array(':nid' => $submission->nid, ':form_key' => 'state'))->fetchField();
  $country_code = $submission->data[$country_cid]['value'][0];
  $state_code = $submission->data[$state_cid]['value'][0];
  if ($fids['country']) {
    $country_name = module_invoke_all('fundraiser_get_country_iso', $country_code); //TODO this is no longer a hook, needsmore thought.
    $country_name = $country_name[0];
    $has_country = db_query('SELECT 1 FROM {profile_value} WHERE uid = :uid AND fid = :fid',
      array(':uid' => $update_user->uid, ':fid' => $fids['country']))->fetchField();
    if ($has_country) {
      db_query('UPDATE {profile_value} SET value = :value WHERE uid = :uid AND fid = :fid',
        array(':value' => $country_name, ':uid' => $update_user->uid, ':fid' => $fids['country']));
    }
    else {
      $values = array('fid' => $fids['country'], 'uid' => $update_user->uid, 'value' => $country_name);
      drupal_write_record('profile_value', $values);
    }
  }
  if ($fids['state']) {
    $state_name = module_invoke_all('fundraiser_get_zone_iso', $country_code, $state_code); //TODO this is no longer a hook, needsmore thought.
    $state_name = $state_name[0];
    $has_state = db_query('SELECT 1 FROM {profile_value} WHERE uid = :uid AND fid = :fid',
      array(':uid' => $update_user->uid, ':fid' => $fids['state']))->fetchField();
    if ($has_state) {
      db_query('UPDATE {profile_value} SET value = :value WHERE uid = :uid AND fid = :fid',
        array(':value' =>  $state_name, ':uid' => $update_user->uid, ':fid' => $fids['state']));
    }
    else {
      $values = array('fid' => $fids['state'], 'uid' => $update_user->uid, 'value' => $state_name);
      drupal_write_record('profile_value', $values);
    }
  }
}

/**
 * Creates an array to map webform component fields to user profile fields.
 */
function _fundraiser_webform_get_user_map($nid) {
  $map = array();
  $results = db_query('SELECT w.form_key, f.cid, f.map_id, w.type '.
    'FROM {webform_component} w '.
    'INNER JOIN {fundraiser_component_map} f ON f.cid = w.cid AND f.nid = w.nid '.
    'WHERE f.nid = :nid '.
    'ORDER BY f.cid',
    array(':nid' => $nid)
  );
  foreach ($results as $result) {
    $row = (array) $result;
    $map[$row['form_key']] = $row['map_id'];
  }
  return $map;
}

// TODO figure out where this is called from
function _fundraiser_webform_profile_save_profile(&$edit, &$user, $category) {
  $results = _fundraiser_webform_profile_get_fields($category);
  foreach ($results as $field) {
    if (_profile_field_serialize($field->type)) { // From profile.module.
      $edit[$field->name] = serialize($edit[$field->name]);
    }
    db_delete('profile_value')->condition('fid', $field->fid)->condition('uid', $user->uid)->execute();
    $profile_values = array('fid' => $field->fid, 'uid' => $user->uid, 'value' => $edit[$field->name]);
    drupal_write_record('profile_value', $profile_values);
    // Mark field as handled (prevents saving to user->data).
    $edit[$field->name] = NULL;
  }
}

/**
 * Automatically maps predefined webform components to user profile fields. Called by fundraiser_insert().
 */
function _fundraiser_webform_create_user_map($nid, $components) {
  foreach ($components as $cid => $component) {
    if (array_key_exists('maps_to', $component)) {
      $component_map = array('nid' => $nid, 'cid' => $cid, 'map_id' => $component['maps_to']);
      drupal_write_record('fundraiser_component_map', $component_map);
    }
  }
  drupal_set_message(t('User profile fields have been mapped. Use the User Map tab to edit the map.'));
}

/**
 * User mapping form.
 */
//TODO does this move to webform? Or it's own profile?
function fundraiser_webform_mapping_form($form, &$form_state, $node) {
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
    '#value' => 'This form allows you to map the submitted form values to Drupal user and profile fields. When the ' .
      'donation form is submitted, the user\'s profile will be updated with the values from the form.',
    '#weight' => 0,
  );
  // Gather user profile options. TODO this becomes hookable eventually? -SeH
  $options = array('' => '--Select One--');
  if (module_exists('profile')) {
    $cat_array = profile_user_categories();
    foreach ($cat_array as $cat) {
      $result = _profile_get_fields($cat['name']);
      foreach( $result as $this_row) {
        $row = (array) $this_row;
        $options['Drupal Profile Fields'][$row['name']] = $row['title'];
      }
    }
  }
  // Add final field, user email address.
  $options['User Fields'] = array(
    'mail' => t("Email Address"),
  );
  // For each component create possible profile fields.
  foreach ($node->webform['components'] as $cid => $component) {
    if ($component['type'] != 'fieldset') {
      $form['mapping']['type'][$cid] = array(
        '#label' => t($component['name'] . " - (" . $component['type'] . ")"),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => isset($node->map[$cid]) ? $node->map[$cid] : '',
      );
    }
  }
  if (empty($form['mapping']['type'])) {
    $form['mapping'] = array(
      '#markup' => t('You must add some fields to the webform before you can map them.'),
    );
  }
  else {
    $form['mapping']['type']['#theme'] = 'fundraiser_mapping_options'; //TODO rename the theme function here.
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  }
  return $form;
}

/**
 * User mapping form submit.
 */
//TODO branch these out into their own CRUD style functions.
function fundraiser_webform_mapping_form_submit($form, &$form_state) {
  db_delete('fundraiser_component_map')->condition('nid', $form['#node']->nid)->execute();
  foreach ($form_state['values']['mapping']['type'] as $key => $value) {
    if (!empty($value)) {
      $component_map = array('nid' => $form['#node']->nid, 'cid' => $key, 'map_id' => $value);
      drupal_write_record('fundraiser_component_map', $component_map);
    }
  }
  drupal_set_message(t('Profile mapping has been saved'));
}
