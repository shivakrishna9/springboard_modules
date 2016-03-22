<?php

/**
 * @file
 * Provide a Webform A/B Test win condition: Total actions completed
 */

// Plugin details
$plugin = array(
  'title' => t('Total Actions Completed'),
  'description' => t('This many actions must be completed by a webform before it is declared the winner.'),
  'config form' => 'sba_message_action_ab_total_actions_completed_config',
  'config form validation' => 'sba_message_action_ab_total_actions_completed_config_validate',
  'config summary' => 'sba_message_action_ab_total_actions_completed_config_summary',
  'current value' => 'sba_message_action_ab_total_actions_completed_current_value',
  'check criteria' => 'sba_message_action_ab_total_actions_completed_check_win',
);


/**
 * Return the config options form for this win condition
 *
 * @param $current_config
 *   Array containing current config options (if set). Default values for form
 *   elements should come from here.
 */
function sba_message_action_ab_total_actions_completed_config($current_config) {
  $form = array();
  $form['actions_completed'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Actions Completed'),
    '#description' => t(''),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => isset($current_config['actions_completed']) ? $current_config['actions_completed'] : '',
  );
  return $form;
}


/**
 * Validate the config for this plugin
 */
function sba_message_action_ab_total_actions_completed_config_validate($config, $field_name_prefix) {
  if (!ctype_digit($config['actions_completed']) || !($config['actions_completed'] > 0)) {
    form_set_error($field_name_prefix . 'actions_completed', t('Please enter an integer for the Number of Actions Completed win condition.'));
  }
  return TRUE;
}


/**
 * Return a summary of the plugin config, for listing on the admin view of a
 * Webform A/B test node
 */
function sba_message_action_ab_total_actions_completed_config_summary($config) {
  return $config['actions_completed'];
}



/**
 * Returns the current value of this win condition.
 *
 * @param $webform_nid
 *   Note ID for the webform being tested.
 */

function sba_message_action_ab_total_actions_completed_current_value($webform_nid) {
  return 0;
}

/**
 * Check if the given Webform A/B Test node has a winner
 *
 * @param $config
 *   Array of config options for this plugin for the Webform A/B test being
 *   checked for a win. In this case, the array will contain 'conversions'
 * @param $webform_details
 *   Array of info about the webform to check.
 * @param $test_node
 *   Webform A/B Test node
 */
function sba_message_action_ab_total_actions_completed_check_win($config, $webform_details, $test_node) {
  return isset($webform_details['actions_completed']) ? $webform_details['actions_completed'] >= $config['actions_completed'] : FALSE;
}
