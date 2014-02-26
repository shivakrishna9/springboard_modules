<?php
/**
 * @file
 * The admin configuration form for the module
 */

/**
 * Admin config form.
 */
function webform_ipe_admin_form() {
  $form['webform_ipe_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Webform IPE enabled content types'),
    '#description' => t('Choose the types on which you would like to allow Webform IPE.'),
    '#options' => _webform_ipe_content_types(),
    '#default_value' => variable_get('webform_ipe_node_types', array()),
  );
  $form['webform_ipe_disallow_editing'] = array(
    '#type' => 'textarea',
    '#title' => t('Disallow editing'),
    '#default_value' => variable_get('webform_ipe_disallow_editing'),
    '#description' => t('Enter the form keys that you would like to "lock" and prevent editing. One per line.'),
  );
//   $form['webform_ipe_group_wrapper'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Group wrapper'),
//     '#default_value' => variable_get('webform_ipe_group_wrapper', '.webform-component-fieldset'),
//     '#description' => t('Enter the jQuery selector for the groups that can be arranged and edited.'),
//   );
//   $form['webform_ipe_component_wrapper'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Component wrapper'),
//     '#default_value' => variable_get('webform_ipe_component_wrapper', '.webform-component'),
//     '#description' => t('Enter the jQuery selector for the components that can be arranged and edited.'),
//   );
  return system_settings_form($form);
}

/**
 * Helper function to generate a list of content types that are set as webforms.
 *
 * @return array $types
 *   Return an array of valid webform content types.
 */
function _webform_ipe_content_types() {
  $types = webform_variable_get('webform_node_types');
  // Clean up a bit.
  unset($types['webform']);
  $types = array_flip($types);
  // Rewrite the array to include the names and proper structure.
  foreach ($types as $type => $name) {
    $args = array(":type" => $type);
    $name = db_query('SELECT name FROM {node_type} WHERE type = :type', $args)->fetchField();
    $types[$type] = $name;
  }
  return $types;
}
