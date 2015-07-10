<?php
/**
 * @file
 * custom_demo.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function custom_demo_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sb_closed|node|donation_form|form';
  $field_group->group_name = 'group_sb_closed';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'donation_form';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Schedule',
    'weight' => '23',
    'children' => array(
      0 => 'field_sb_form_is_closed',
      1 => 'field_sb_form_close_date',
      2 => 'field_sb_form_closed_options',
      3 => 'field_sb_form_closed_message',
      4 => 'field_sb_form_closed_redirect',
    ),
    'format_type' => 'tab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-sb-closed field-group-tab',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_sb_closed|node|donation_form|form'] = $field_group;

  return $export;
}
