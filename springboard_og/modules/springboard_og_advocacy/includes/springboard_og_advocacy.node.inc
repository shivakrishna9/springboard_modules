<?php

/**
 * Implements hook_node_insert().
 */
function springboard_og_advocacy_node_insert($node) {
  if ($node->type == 'springboard_group') {
    springboard_og_advocacy_save($node);
  }
}

/**
 * Implements hook_node_update().
 */
function springboard_og_advocacy_node_update($node) {
  if ($node->type == 'springboard_group') {
    springboard_og_advocacy_save($node);
  }
}

/**
 * Save the group's advocacy subscription data.
 *
 * @param object $node
 *   The Springboard Group node.
 */
function springboard_og_advocacy_save($node) {

  // Is there an existing subscription record?
  $exists = db_query('select gid from {sb_og_advocacy} where gid = :gid', (array(':gid' => $node->nid)))->fetchField();

  // If the subscription level permits state subscriptions, save them;
  // otherwise, remove any previously existing state data.
  $allowed_states = $node->group_subscription_type != 'federal-and-states' && $node->group_subscription_type != 'states-only' ? array_filter($node->us_states) : [];

  $data = array(
    'gid' => $node->nid,
    "allowed_states" => $allowed_states,
    'master_subscription_type' => $node->master_subscription_type,
    'group_subscription_type' => $node->group_subscription_type,
  );

  if (!empty($exists)) {
    drupal_write_record('sb_og_advocacy', $data, array('gid'));
  }
  else {
    drupal_write_record('sb_og_advocacy', $data);
  }
}
