<?php

/**
 * @file
 * Provide a Webform A/B Test win condition: Total sum of donations
 */

// Plugin details
$plugin = array(
  'title' => t('Sum of Donation Amounts'),
  'description' => t('A donation form must receive at least this much in donations to be can be declared the winner.'),
  'config form' => 'fundraiser_ab_donations_total_config',
  'config validation form' => 'fundraiser_ab_donations_total_config_validate',
  'config summary' => 'fundraiser_ab_donations_total_config_summary',
  'check win criteria' => 'fundraiser_ab_donations_total_check_win',
);

/**
 * Return the config options form for this win condition
 * 
 * @param $current_config
 *   Array containing current config options (if set). Default values for form
 *   elements should come from here.
 */
function fundraiser_ab_donations_total_config($current_config) {
  $form = array();
  $form['donation_sum'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum Sum of Donations'),
    '#description' => t('The minimum amount of donations that must be made.'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => isset($current_config['donation_sum']) ? $current_config['donation_sum'] : '',
  );
  return $form;
}

/**
 * Validate the config for this plugin
 */
function fundraiser_ab_donations_total_config_validate($config, $field_name_prefix) {
  if (!is_numeric($config['donation_sum']) || !($config['donation_sum'] > 0)) {
    form_set_error($field_name_prefix . 'hits', t('Please enter a positive number for Minimum Sum of Donations win condition.'));
  }
}


/**
 * Return a summary of the plugin config, for listing on the admin view of a
 * Webform A/B test node
 * @todo: format this as a dollar amount
 */
function fundraiser_ab_donations_total_config_summary($config) {
  return $config['donation_sum'];
}


/**
 * Check if the given Webform A/B Test node has a winner
 * 
 * @param $config
 *   Array of config options for this plugin for the Webform A/B test being
 *   checked for a win. In this case, the array will contain 'donation_sum'
 * @param $webform_details
 *   Array of info about the webform to check.
 */
function fundraiser_ab_donations_total_check_win($config, $webform_details) {
  return ($webform_details['fundraiser']['total_donations'] >= $config['donation_sum']);
}
