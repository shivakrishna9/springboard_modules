<?php

/**
 * @file
 *
 * Admin functions for the fundraiser purge module.
 */

/**
 * Fundraiser purge settings form.
 */
function fundraiser_purge_settings_form() {
  $form['fundraiser_purge_age'] = array(
    '#type' => 'textfield',
    '#title' => t('Purge age'),
    '#default_value' => variable_get('fundraiser_purge_age', 0),
    '#description' => t('Enter the number of days that posted donations should remain in the system. Posted donations older then the number of days configured here will be permanently deleted from the system. Enter 0 for never purge.'),
    '#size' => 5,
  );

  return system_settings_form($form);
}

/**
 * Validation function for purge settings form.
 *
 * Ensure purge age is a positive integer.
 */
function fundraiser_purge_settings_form_validate($form, &$form_state) {
  $purge_age = $form_state['values']['fundraiser_purge_age'];

  if (!is_numeric($purge_age) || !((int) ($purge_age) == $purge_age) || (int) $purge_age < 0) {
    form_set_error('fundraiser_purge_age', 'Purge age must be a positive integer');
  }
}
