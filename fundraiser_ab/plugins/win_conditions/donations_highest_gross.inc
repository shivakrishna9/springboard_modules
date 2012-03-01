<?php

/**
 * @file
 * Provide a Webform A/B Test win condition: Highest sum of donations
 */

// Plugin details
$plugin = array(
  'title' => t('Highest Sum of Donation Amounts'),
  'description' => t('A donation form will be declared the winner if it meets all other win conditions, and has the highest sum of donation amounts among the donation forms included in the A/B test.'),
  'config summary' => 'fundraiser_ab_donations_highest_gross_config_summary',
  'check criteria' => 'fundraiser_ab_donations_highest_gross_check_win',
);



/**
 * Return a summary of the plugin config, for listing on the admin view of a
 * Webform A/B test node
 */
function fundraiser_ab_donations_highest_gross_config_summary($config) {
  return 'Yes';
}


/**
 * Check if the given Webform A/B Test node has a winner
 * Checks the total donation amount for every donation form included in the
 * A/B test, and checks if the one being checked has the highest
 * 
 * @param $config
 *   Array of config options for this plugin for the Webform A/B test being
 *   checked for a win. In this case, the array will be empty.
 * @param $webform_details
 *   Array of info about the webform to check.
 * @param $test_node
 *   Webform A/B Test node
 */
function fundraiser_ab_donations_highest_gross_check_win($config, $webform_details, $test_node) {
  $highest_gross = 0;
  $highest_gross_nid = 0;

  // Loop over all the forms in this A/B test
  foreach ($test_node->included_webforms as $nid => $details) {
    if (is_array($details['fundraiser'])) {
      // If this form has earned more than the last highest, save it
      if ($details['fundraiser']['total_donations'] > $highest_gross) {
        $highest_gross = $details['fundraiser']['total_donations'];
        $highest_gross_nid = $nid;
      }
    }
  }

  // If the webform in question has the highest average, return TRUE
  return ($highest_gross_nid == $webform_details['webform_nid']);
}
