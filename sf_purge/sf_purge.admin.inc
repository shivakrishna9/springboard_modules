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
  // consider switching to SELECT DISTINCT drupal_types FROM {salesforce_management_object_map} ?
  $available_purge_types = drupal_map_assoc(variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node')));
  $purge_types = variable_get('sf_purge_drupal_types', array());

  // Get the number of records per type to display in the labels.
  $count_by_type = sf_purge_count_by_type();

  foreach ($available_purge_types as $type) {
    if (array_key_exists($type, $count_by_type)) {
      $available_purge_types[$type] = $type . ' (' . number_format($count_by_type[$type]) . ')';
    }
  }

  $form['purge_now'] = array(
    '#type' => 'button',
    '#value' => t('Purge now'),
    '#suffix' => '<p>' . t('Purge Salesforce management object maps now.') . '</p>',
    '#submit' => array('sf_purge_purge_now'),
    '#executes_submit_callback' => TRUE,
  );

  $form['sf_purge_record_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Record count'),
    '#size' => 5,
    '#description' => t('Number of records to purge per cron run. Note: setting this too high may cause performance issues'),
    '#default_value' => variable_get('sf_purge_record_count', 2000),
    '#required' => TRUE,
  );

  $form['sf_purge_drupal_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Types to purge'),
    '#description' => t('Select the types that will be purged from the system.'),
    '#options' => $available_purge_types,
    '#default_value' => $purge_types,
  );

  $form['#validate'][] = 'sf_purge_admin_validate';
  return system_settings_form($form);
}

/**
 * Validate count field
 */
function sf_purge_admin_validate(&$form, $form_state) {
  if (!is_numeric($form_state['values']['sf_purge_record_count'])) {
    form_set_error('sf_purge_record_count', t('The record count field must contain a numeric value.'));
  }
  elseif ($form_state['values']['sf_purge_record_count'] <= 0) {
    form_set_error('sf_purge_record_count', t('The record count must be a positive integer'));
  }
}