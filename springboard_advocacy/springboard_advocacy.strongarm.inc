<?php
/**
 * @file
 * springboard_advocacy.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function springboard_advocacy_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'additional_settings__active_tab_springboard_petition';
  $strongarm->value = 'edit-webform-user';
  $export['additional_settings__active_tab_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'field_bundle_settings_node__springboard_petition';
  $strongarm->value = array();
  $export['field_bundle_settings_node__springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_springboard_petition';
  $strongarm->value = array(
    0 => 'main-menu',
  );
  $export['menu_options_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_springboard_petition';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_springboard_petition';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_springboard_petition';
  $strongarm->value = '1';
  $export['node_preview_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_springboard_petition';
  $strongarm->value = 0;
  $export['node_submitted_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'webform_user_default_fields_springboard_petition';
  $strongarm->value = array(
    0 => 'mail',
    1 => 'sbp_first_name',
    2 => 'sbp_last_name',
  );
  $export['webform_user_default_fields_springboard_petition'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'webform_user_springboard_petition';
  $strongarm->value = 1;
  $export['webform_user_springboard_petition'] = $strongarm;

  return $export;
}
