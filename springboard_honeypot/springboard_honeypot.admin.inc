<?php

/**
 * @file
 * Admin functions for springboard_honeypot module.
 */

/**
 * Admin settings for springboard_honeypot.
 */
function springboard_honeypot_admin_form($form, &$form_state) {
  $form['springboard_honeypot_count_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Honeypot blacklist count threshold'),
    '#description' => t('Maximum number of Honeypot validation failures tolerated before adding IP to the system blacklist.'),
    '#default_value' => variable_get('springboard_honeypot_count_threshold', 50),
    '#required' => TRUE,
    '#size' => 5,
  );

  return system_settings_form($form);
}
