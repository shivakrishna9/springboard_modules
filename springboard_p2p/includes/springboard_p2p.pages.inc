<?php
/**
 * @file
 * User facing page callbacks and forms for Springboard P2P.
 */


/**
 * Page callback for the personal dashboard.
 *
 * @param object $user
 *   The user that matches the uid in the path.
 *
 * @return array
 *   Render array.
 */
function springboard_p2p_personal_dashboard($user) {
  return array(
    'comingsoon' => array(
      '#markup' => '<h2>Personal dashboard for ' . $user->name . ' coming soon</h2>',
    ),
  );
}

/**
 * Page placeholder for creating a new personal campaign under a campaign.
 */
function springboard_p2p_create_personal_campaign($campaign) {
  $output = array(
    'message' => array(
      '#markup' => 'Placeholder page for creating a new personal campaign. The parent campaign is ' . l($campaign->title, 'node/' . $campaign->nid),
    ),
  );

  return $output;
}
