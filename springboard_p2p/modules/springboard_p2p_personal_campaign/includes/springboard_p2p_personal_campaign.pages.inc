<?php

/**
 * @file
 * Page callbacks and associated helper functions.
 */

/**
 * Page callback for personal campaign donor dashboard.
 *
 * @param object $account
 *   User object loaded from uid in path.
 *
 * @param object $node
 *   Personal campaign node.
 *
 * @return array
 *   Render array.
 */
function springboard_p2p_personal_campaign_donors_dashboard($account, $node) {
  drupal_set_title($node->title);
  $dashboard_contents = views_embed_view('personal_campaign_donor_dashboard', 'default', $node->nid);
 return array(
   'content' => array(
     '#markup' => $dashboard_contents,
   ),
 );
}
