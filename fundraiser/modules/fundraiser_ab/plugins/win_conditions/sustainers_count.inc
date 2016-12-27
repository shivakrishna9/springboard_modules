<?php

/**
 * @file
 * Provide a Multivariate test win condition: Minimum number of sustainer
 * donations
 */

// Plugin details
$plugin = array(
  'title' => t('Number of Sustainer Donations'),
  'description' => t('A donation form must receive at least this many sustainer donations to be can be declared the winner.'),
  'config form' => 'fundraiser_ab_sustainers_count_config',
  'config form validation' => 'fundraiser_ab_sustainers_count_config_validate',
  'config summary' => 'fundraiser_ab_sustainers_count_config_summary',
  'current value' => 'fundraiser_ab_sustainers_count_current_value',
  'check criteria' => 'fundraiser_ab_sustainers_count_check_win',
);

/**
 * Return the config options form for this win condition.
 *
 * @param array $current_config
 *   Array containing current config options (if set). Default values for form
 *   elements should come from here.
 *
 * @return array
 *   The config values.
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
 * Multivariate test node
 */
function fundraiser_ab_sustainers_count_config_summary($config) {
  return $config['sustainers'];
}

/**
 * Returns the current value for this test.
 *
 * @param int $webform_nid
 *   The webform nid.
 * @param array $webform_details
 *   Webform details.
 * @param object $node
 *   The AB test node.
 *
 * @return int
 *   The total number of sustainers for this form since the test started.
 */
function fundraiser_ab_sustainers_count_current_value($webform_nid, $webform_details, $node) {
  $first_hit_timestamp = 0;
  if (variable_get('webform_ab_omit_prior_fundraiser', FALSE)) {
    $first_hit_timestamp = fundraiser_ab_get_first_hit_in_test($node);
    if (!$first_hit_timestamp) {
      return 0;
    }
  }

  $excluded = implode(',', fundraiser_ab_get_omitted_users($webform_nid));

  return db_query("SELECT nid FROM {fundraiser_donation} d, {fundraiser_sustainers} s " .
    "WHERE d.did = s.master_did AND d.nid = :webform_nid  and d.created > :first_hit AND uid NOT IN (:excluded)",
    array(':webform_nid' => $webform_nid, ':first_hit' => $first_hit_timestamp, ':excluded' => $excluded))
    ->rowCount();
}

/**
 * Check if the given Multivariate test node has a winner.
 *
 * @param array $config
 *   Array of config options for this plugin for the multivariate test being
 *   checked for a win. In this case, the array will contain 'sustainers'.
 * @param array $webform_details
 *   Array of info about the webform to check.
 * @param object $test_node
 *   Multivariate Test node.
 *
 * @return bool
 *   Whether we have a winner, true of false.
 *
 */
function fundraiser_ab_sustainers_count_check_win($config, $webform_details, $test_node) {
  return isset($webform_details['fundraiser']) && is_array($webform_details['fundraiser']) && ($webform_details['fundraiser']['sustainer_count'] >= $config['sustainers']);
}
