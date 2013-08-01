<?php

/**
 * @file
 *
 * Admin functions for the salesforce purge module.
 */

/**
 * Salesforce purge settings form.
 */
function sf_purge_settings_form() {
  $available_purge_types = drupal_map_assoc(variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node')));
  $purge_types = variable_get('sf_purge_drupal_types', array());

  // Get the number of records per type to display in the labels.
  $count_by_type = sf_purge_count_by_type();
  foreach ($available_purge_types as $type) {
    if (array_key_exists($type, $count_by_type)) {
      $available_purge_types[$type] = $type . ' (' . number_format($count_by_type[$type]) . ')';
    }
  }

  $form['sf_purge_drupal_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Types to purge'),
    '#description' => t('Select the types that will be purged from the system.'),
    '#options' => $available_purge_types,
    '#default_value' => $purge_types,
  );

  return system_settings_form($form);
}