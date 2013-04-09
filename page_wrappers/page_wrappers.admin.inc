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

  if (module_exists('og')) {
    $form['page_wrappers_filter_by_og'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter available Page Wrappers by group'),
      '#description' => t('When checked, the list of available page wrappers displayed on node edit forms will be limited to page wrappers that are assigned to one or more groups the user is enrolled in.'),
      '#default_value' => variable_get('page_wrappers_filter_by_og', FALSE),
    );
  }

  return system_settings_form($form);
}
