<?php
/**
 * @file
 * springboard_stats.fields.inc
 */

/**
 * Add stats fields to a webform node type.
 *
 * @param $type String
 *    The node type
 * @param $fundraiser_specific Boolean
 *    True if fundraiser-specific stats fields should be added. False if fields generic to all webform types should be.
 */
function _springboard_stats_add_default_fields($type, $fundraiser_specific = FALSE) {
  // These should be added on each page request, but just to be sure...
  module_load_include('inc', 'field', 'field.crud');
  module_load_include('inc', 'field', 'field.info');
  
  $fields = $fundraiser_specific ? _springboard_stats_fundraiser_fields_settings($type) : _springboard_stats_webform_fields_settings($type);
  foreach ($fields as $field_name => $field) {
    // If the field doesn't exist create it.
    if (!field_info_field($field_name)) {
      field_create_field($field['field_config']);
    }
    // Before adding the field, check that it doesn't exist on the node type.
    if (!field_info_instance('node', $field_name, $type)) {
      field_create_instance($field['field_instance']);
    }
  }
}

/**
 * Returns an array of field configurations
 *
 * @param $type String
 *    The type of node these fields will be added to, used to populate the bundle property
 */
function _springboard_stats_webform_fields_settings($type) {
  $fields = array();
  $t = get_t();
  
  $fields['sbstats_count_submissions'] = array(
    'field_config' => array(
      'type' => 'number_integer',
      'cardinality' => 1,
      'locked' => TRUE,
      'settings' => array(
        'min' => '0',
      ),
      'entity_types' => array(),
      'field_name' => 'sbstats_count_submissions',
      'foreign keys' => array(),
      'indexes' => array(),
      'translatable' => FALSE,
    ),
    'field_instance' => array(
      'bundle' => $type,
      'default_value' => 0,
      'description' => $t('The total number of submissions received per form.'),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => '2',
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'sbstats_count_submissions',
      'label' => $t('Total Number of Submissions'),
      'required' => FALSE,
      'settings' => array(
        'text_processing' => FALSE,
        'user_register_form' => FALSE,
      ),
/*      'widget' => array(
 *        'active' => 1,
 *        'module' => 'text',
 *        'settings' => array(
 *          'size' => '60',
 *        ),
 *        'type' => 'text_textfield',
 *        'weight' => '0',
 *      ),
 */
    ),
  );

  return $fields;
}

/**
 * Returns an array of field configurations
 *
 * @param $type String
 *    The type of node these fields will be added to, used to populate the bundle property
 */
function _springboard_stats_fundraiser_fields_settings($type) {
  $fields = array();
  $t = get_t();
  
  $fields['sbstats_total_transactions'] = array(
    'field_config' => array(
      'type' => 'number_decimal',
      'cardinality' => 1,
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
      ),
      'entity_types' => array(),
      'field_name' => 'sbstats_total_transactions',
      'foreign keys' => array(),
      'indexes' => array(),
      'translatable' => FALSE,
    ),
    'field_instance' => array(
      'bundle' => $type,
      'default_value' => 0.0,
      'description' => $t('The total amount of completed commerce transactions that resulted from submissions per form.'),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => '2',
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'sbstats_total_transactions',
      'label' => $t('Total of Transactions'),
      'required' => FALSE,
      'settings' => array(
        'text_processing' => FALSE,
        'user_register_form' => FALSE,
      ),
/*      'widget' => array(
 *        'active' => 1,
 *        'module' => 'text',
 *        'settings' => array(
 *          'size' => '60',
 *        ),
 *        'type' => 'text_textfield',
 *        'weight' => '0',
 *      ),
 */
    ),
  );

  return $fields;
}
