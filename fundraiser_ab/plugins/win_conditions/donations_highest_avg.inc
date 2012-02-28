<?php

/**
 * @file
 * Provide a Webform A/B Test win condition: Highest average of donations
 */

// Plugin details
$plugin = array(
  'title' => t('Highest Average of Donation Amounts'),
  'description' => t('A donation form will be declared the winner if it meets all other win conditions, and has the highest average donation amount among the donation forms included in the A/B test.'),
  'config summary' => 'fundraiser_ab_donations_highest_avg_config_summary',
  'check criteria' => 'fundraiser_ab_donations_highest_avg_check_win',
);



/**
 * Return a summary of the plugin config, for listing on the admin view of a
 * Webform A/B test node
 * @todo: format this as a dollar amount
 */
function fundraiser_ab_donations_highest_avg_config_summary($config) {
  return 'Yes';
}


/**
 * Check if the given Webform A/B Test node has a winner
 * Checks the average donation amount for every donation form included in the
 * A/B test, and checks if the one being checked has the highest average
 * 
 * @param $config
 *   Array of config options for this plugin for the Webform A/B test being
 *   checked for a win. In this case, the array will be empty.
 * @param $webform_details
 *   Array of info about the webform to check.
 * @param $test_node
 *   Webform A/B Test node
 */
function fundraiser_ab_donations_highest_avg_check_win($config, $webform_details, $test_node) {
  $highest_avg = 0;
  $highest_avg_nid = 0;

  // Loop over all the forms in this A/B test
  foreach ($test_node->included_webforms as $nid => $details) {
    if (is_array($details['fundraiser']) && $details['fundraiser']['donation_count'] > 0) {
      // Determine the average donation amount for this donation form
      $avg = $details['fundraiser']['total_donations'] / $details['fundraiser']['donation_count'];
      
      // If this average is higher than the last highest, save it
      if ($avg > $highest_avg) {
        $highest_avg = $avg;
        $highest_avg_nid = $nid;
      }
    }
  }

  // If the webform in question has the highest average, return TRUE
  return ($highest_avg_nid == $webform_details['webform_nid']);
}
