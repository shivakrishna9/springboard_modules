<?php

/**
 * @file
 * Ctools access plugin for checking if a user is considered to be on a mobile device
 */

$plugin = array(
  'title' => t('Mobile user (tablet only)'),
  'description' => t('Controls access by checking for mobile devices, limited to tablet devices'),
  'callback' => 'mobile_rulesets_user_tablet_ctools_access_check',
  'summary' => t('summary'),
);

/**
 * Custom callback defined by 'callback' in the $plugin array.
 *
 * Check for access.
 */
function mobile_rulesets_user_tablet_ctools_access_check($conf, $context) {
  // If the user is on a mobile device
  return mobile_rulesets_is_tablet();
}

