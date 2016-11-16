<?php
/**
 * @file
 * Functions which replace the site's Advocacy API details with the group's.
 */

/**
 * Get a particular group's subscription data.
 *
 * @param int $gid
 *   The group id.
 *
 * @return array
 *   The group's subscripton data.
 */
function springboard_group_advocacy_get_saved_group_subscription($gid) {
  $subscription = db_query('select * from {sb_og_advocacy} where gid = :gid', (array(':gid' => $gid)))->fetchAssoc();
  return $subscription;
}

/**
 * Implements hook_springboard_advocacy_subscription_alter().
 *
 * Intercepts the API subscription response and alters it to match the locally
 * saved subscription level of a particular group.
 */
function springboard_group_advocacy_springboard_advocacy_subscription_alter(&$subscription) {

  // Check for a group context from session.
  $gid = springboard_group_get_group_context();

  if (!empty($gid) && ($gid != '_none' && $gid != '_all')) {
    // Get the current node to check that it is not itself a group.
    $node = menu_get_object();
    if (!empty($node->nid) && !og_is_group('node', $node->nid)) {
      // We're on a message add or edit page.
      springboard_group_advocacy_build_altered_subscription($gid, $subscription);
    }
    elseif (empty($node)) {
      // We're on the custom groups add or edit pages.
      springboard_group_advocacy_build_altered_subscription($gid, $subscription);
    }
  }
  else {
    $subscription['is_grouped'] = FALSE;
  }
}

/**
 * Assemble the altered subscription.
 *
 * @param int $gid
 *   The group ID.
 * @param array $subscription
 *   The subscription array as returned from the API.
 */
function springboard_group_advocacy_build_altered_subscription($gid, &$subscription) {
  $group_subscription = springboard_group_advocacy_get_saved_group_subscription($gid);
  $states = unserialize($group_subscription['allowed_states']);
  $subscription['states_custom'] = !empty($states) ? array_keys($states) : [];
  $subscription['subscription_type'] = $group_subscription['group_subscription_type'];
  $subscription['is_grouped'] = TRUE;
}
