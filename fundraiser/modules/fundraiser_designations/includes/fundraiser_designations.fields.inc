<?php
/**
 * @file
 * fundraiser_designations.fields.inc
 */

/**
 * Add fields needed by fundraiser designations to a node type.
 *
 * @param string $type
 *   The type of node to check if the fundraiser designations
 *   fields exist, and if not add them to.
 */
function _fundraiser_designations_add_default_fields($type) {
  $fields = _fundraiser_designations_default_fields_settings($type);
  foreach ($fields as $field_name => $field) {
    // These should be added on each page request, but to be sure do it again.
    module_load_include('inc', 'field', 'field.crud');
    module_load_include('inc', 'field', 'field.info');

    // If the field doesn't exist create it.
    if (!field_info_field($field_name)) {
      field_create_field($field['field_config']);
    }
    // Before adding the field check that it does not exist on the node type.
    if (!field_info_instance('node', $field_name, $type)) {
      field_create_instance($field['field_instance']);
    }
  }
}

/**
 * Returns an array of field configurations.
 *
 * This is copied from a features export.
 *
 * @param string $type
 *   The type of node these fields will be added to,
 *   used to populate the bundle property.
 *
 * @return array
 *   Field definitions.
 */
function _fundraiser_designations_default_fields_settings($type) {
  $fields = array();

  return $fields;
}
