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
  $dashboard_content = views_embed_view('p2p_personal_campaigns_by_campaigner', 'page', $user->uid);

  $full_name = springboard_p2p_format_user_full_name($user);
  if (!empty($full_name)) {
    drupal_set_title('Personal campaign dashboard for ' . $full_name);
  }

  return array(
    'content' => array(
      '#markup' => $dashboard_content,
    ),
  );
}
