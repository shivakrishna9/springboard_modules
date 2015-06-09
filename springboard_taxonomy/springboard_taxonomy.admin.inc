<?php
/**
 * @file
 * Admin functions for Springboard advocacy.
 */

/**
 * Page callback for the admin form.
 */
function springboard_taxonomy_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-taxonomy-settings-page',
      '#type' => 'markup',
      '#markup' => '<h3>' . t('Configure settings for Springboard Taxonomy') . '</h3>',
      '#prefix' => '<div class="springboard-taxonomy-settings-page">',
      '#suffix' => '</div>',
    ),
    'springboard_taxonomy_settings_form' => drupal_get_form('springboard_taxonomy_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard taxonomy.
 */
function springboard_taxonomy_settings_form() {
  $form = array();
  $types = node_type_get_names();
  $webform_types = variable_get('webform_node_types', array());
  unset($webform_types['webform']);
  $webform_types = array_flip($webform_types);
  $options = array_intersect_key($types, $webform_types);
  $form['springboard_taxonomy_node_types'] = array(
    '#title' => t('Enable the Organization Issues taxonomy for the following form types:'),
    '#description' => t('Unchecking any checked items will permanently remove all Organization Issues data from that content type.'),
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => variable_get('springboard_taxonomy_node_types', ''),
  );

  $form['#submit'][] = 'springboard_taxonomy_settings_submit';

  return system_settings_form($form);
}

/**
 * Submit Function.
 *
 * Create or delete field instances.
 *
 * @param array $form
 * @param array $form_state
 */
function springboard_taxonomy_settings_submit($form, $form_state) {
  $types = $form_state['values']['springboard_taxonomy_node_types'];
  foreach ($types as $type => $status) {
    if ($status === 0) {
      _springboard_taxonomy_delete_instances($type);
    }
    else {
      _springboard_taxonomy_create_instances($type);
    }
  }
}

/**
 * Create taxonomy fields.
 */
function _springboard_taxonomy_create_instances($type) {
  module_load_include('inc', 'field', 'field.crud');
  module_load_include('inc', 'field', 'field.info');
  module_load_include('install', 'springboard_taxonomy');

  foreach (_springboard_taxonomy_field_instance($type) as $field_name => $field) {

    // If somehow the base field was deleted, recreate it.
    if (!field_info_field($field['field_instance']['field_name'])) {
      $base = _springboard_taxonomy_field_base();
      field_create_field($base[$field_name]['field_config']);
    }

    // Create the instance.
    if (!field_info_instance('node', $field['field_instance']['field_name'], $type)) {
      field_create_instance($field['field_instance']);
    }
  }
}

/**
 * Delete taxonomy fields.
 */
function _springboard_taxonomy_delete_instances($type) {
  module_load_include('inc', 'field', 'field.crud');
  module_load_include('inc', 'field', 'field.info');
  module_load_include('install', 'springboard_taxonomy');
  foreach (_springboard_taxonomy_field_instance($type) as $field_name => $field) {
    if (field_info_instance('node', $field['field_instance']['field_name'], $type)) {
      // Set false to avoid deleting base field if no more instances exist.
      field_delete_instance($field['field_instance'], FALSE);
    }
  }
}
