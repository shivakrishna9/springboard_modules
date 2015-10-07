<?php

/**
 * @file
 * Install helper functions for profile fields.
 */

/**
 * Create the message action user profile fields.
 */
function _sbp_message_action_create_user_profile_fields() {
  _sba_message_action_create_zip_plus_four();
}

/**
 * Create the zip plus four user profile field.
 *
 * Done separately so can be used in update hook.
 */
function _sba_message_action_create_zip_plus_four() {

  $profile_zip_plus_four = array(
    'label' => t('Zip+4'),
    'field_name' => 'sbp_zip_plus_four',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
      'teaser' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );

  $zip_exists = field_info_field('sbp_zip_plus_four');

  if (empty($zip_exists)) {
    field_create_field($profile_zip_plus_four);
  }

  $instance_exists = field_info_instance('user', 'sbp_zip_plus_four', 'user');
  if (empty($instance_exists)) {
    $instance = array(
      'entity_type' => 'user',
      'bundle' => 'user',
    );
    $instance = array_merge($profile_zip_plus_four, $instance);
    field_create_instance($instance);
  }

  $field_group = field_group_load_field_group('group_profile_admin', 'user', 'user', 'form');
  $table = 'field_group';

  if ($field_group) {
    if(empty($field_group->children['sbp_zip_plus_four'])) {
      $field_group->children[] = 'sbp_zip_plus_four';
      ctools_export_crud_save($table, $field_group);
    }
  }
}
