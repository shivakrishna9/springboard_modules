<?php


/**
 * Implements hook_field_group_info().
 *
 * Add field groups to the springboard_group node add/edit form.
 */
function springboard_og_field_group_info() {
  $export = array();

  // Group contact field group.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sb_group_contact|node|springboard_group|form';
  $field_group->group_name = 'group_sb_group_contact';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'springboard_group';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_sb_group_details';
  $field_group->data = array(
    'label' => 'Contact',
    'weight' => '7',
    'children' => array(
      0 => 'field_sb_group_cname',
      1 => 'field_sb_group_cemail',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-sb-group-contact field-group-fieldset',
        'required_fields' => 1,
        'collapsed' => 1,
      ),
    ),
  );
  $export['group_sb_group_contact|node|springboard_group|form'] = $field_group;

  // Group details field group.
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sb_group_details|node|springboard_group|form';
  $field_group->group_name = 'group_sb_group_details';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'springboard_group';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Group Details',
    'weight' => '2',
    'children' => array(
      0 => 'field_sb_group_address',
      1 => 'field_sb_group_logo',
      2 => 'group_sb_group_contact',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-sb-group-details field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_sb_group_details|node|springboard_group|form'] = $field_group;

  return $export;
}
