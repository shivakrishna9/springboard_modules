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
  $options = node_get_types('names');

  $form['page_wrapper_enabled_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Page wrapper-enabled content types'),
    '#options' => $options,
    '#default_value' => variable_get('page_wrapper_enabled_node_types', array('donation_form')),
    '#description' => t('A list of page wrappers will be available on these content types.'),
  );

  return system_settings_form($form);
}
