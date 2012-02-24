<?php

/**
 * @file
 * Provide a Webform A/B Test win condition: Minimum number of sustainer
 * donations
 */

// Plugin details
$plugin = array(
  'title' => t('Number of Sustainer Donations'),
  'description' => t('A donation form must receive at least this many sustainer donations to be can be declared the winner.'),
  'config form' => 'fundraiser_ab_sustainers_count_config',
  'config validation form' => 'fundraiser_ab_sustainers_count_config_validate',
  'config summary' => 'fundraiser_ab_sustainers_count_config_summary',
  'check win criteria' => 'fundraiser_ab_sustainers_count_check_win',
);

/**
 * Return the config options form for this win condition
 * 
 * @param $current_config
 *   Array containing current config options (if set). Default values for form
 *   elements should come from here.
 */
function fundraiser_ab_sustainers_count_config($current_config) {
  $form = array();
  $form['sustainers'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Sustainer Donations'),
    '#description' => t('The minimum number of sustainer donations.'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => isset($current_config['sustainers']) ? $current_config['sustainers'] : '',
  );
  return $form;
}

/**
 * Validate the config for this plugin
 */
function fundraiser_ab_sustainers_count_config_validate($config, $field_name_prefix) {
  if (!ctype_digit($config['sustainers']) || !($config['sustainers'] > 0)) {
    form_set_error($field_name_prefix . 'hits', t('Please enter an integer for Number of Sustainer Donations win condition.'));
  }
}


/**
 * Return a summary of the plugin config, for listing on the admin view of a
 * Webform A/B test node
 */
function fundraiser_ab_sustainers_count_config_summary($config) {
  return $config['sustainers'];
}


/**
 * Check if the given Webform A/B Test node has a winner
 * 
 * @param $config
 *   Array of config options for this plugin for the Webform A/B test being
 *   checked for a win. In this case, the array will contain 'sustainers'
 * @param $webform_details
 *   Array of info about the webform to check.
 */
function fundraiser_ab_sustainers_count_check_win($config, $webform_details) {
  return ($webform_details['fundraiser']['sustainer_count'] >= $config['sustainers']);
}
