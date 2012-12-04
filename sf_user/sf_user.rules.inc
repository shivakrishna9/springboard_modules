<?php

/**
 * @file
 * Functions for rules integration.
 */

/**
 * Implementation of hook_rules_action_info().
 */
function sf_user_rules_action_info() {
  return array(
    'sf_user_update_profile_field_action' => array(
      'label' => t('Update a user profile field'),
      'module' => 'Salesforce user',
      'arguments' => array(
        'oid' => array(
          'type' => 'value',
          'label' => t('The id of the user to update')
        ),
      ),
      'eval input' => array('field', 'value'),
    ),
  );
}

/**
 * Settings form for choosing which profile field to update.
 */
function sf_user_update_profile_field_action_form($settings, &$form) {
  $settings += array('field' => '', 'value' => '');

  // get a list of all configured profile fields
  $profile_fields = _sf_user_profile_get_fields();
  // sort the fields
  ksort($profile_fields);

  // create an array to hold the select options
  $options = array();
  foreach ($profile_fields as $name => $field) {
    $options[$field['fid']] = $field['title'];
  }

  $form['settings']['field'] = array(
    '#type' => 'select',
    '#title' => t('Profile field'),
    '#options' => $options,
    '#default_value' => $settings['field'],
    '#description' => t('The field that will be updated.'),
  );

  $form['settings']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#default_value' => $settings['value'],
    '#description' => t('The value to update the field with.'),
  );
}

/**
 * Action to update a user profile field.
 *
 * @param $user
 *   The user whose profile field will be updated.
 * @param $settings
 *   The configured rule settings.
 */
function sf_user_update_profile_field_action($oid, $settings) {
  $field = $settings['field'];
  $value = $settings['value'];
  db_query("DELETE FROM {profile_values} WHERE fid = %d and uid = %d", $field, $oid);
  db_query("INSERT INTO {profile_values} (fid, uid, value) VALUES (%d, %d, '%s')", $field, $oid, $value);
}