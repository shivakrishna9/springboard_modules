<?php

/**
 * @file
 * Administration functions for page wrappers module.
 *
 * Provides admin functionality for the page wrappers module.
 */
 
/**
 * Admin configuration form for page wrappers.
 */ 
function page_wrappers_settings_form() {
  $types = node_type_get_types();
  unset($types['page_wrapper']);
  $options = array();
  foreach ($types as $db_name => $type) {
    $options[$db_name] = $type->name;
  }

  $form['page_wrapper_enabled_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Page wrapper-enabled content types'),
    '#options' => $options,
    '#default_value' => variable_get('page_wrapper_enabled_node_types', array('donation_form')),
    '#description' => t('A list of page wrappers will be available on these content types.'),
  );
  
  return system_settings_form($form);
}