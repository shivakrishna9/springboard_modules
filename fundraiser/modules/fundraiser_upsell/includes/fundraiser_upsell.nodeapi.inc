<?php
/**
 * @file
 * fundraiser_upsell.nodeapi.inc - Nodeapi functions
 */


/**
 * Implements hook_nodeapi().
 */
function fundraiser_upsell_nodeapi(&$node, $op, $teaser, $page) {
  $donation_node = fundraiser_is_donation_type($node->type);
  if ($donation_node) {
    switch ($op) {
      case 'load':
        fundraiser_upsell_node_load($node);
        break;

      case 'insert':
        fundraiser_upsell_node_insert($node);
        break;

      case 'update':
        fundraiser_upsell_node_update($node);
        break;

      case 'delete':
        fundraiser_upsell_node_delete($node);
        break;
    }
  }
}

/**
 * Implements hook_node_insert().
 */
function fundraiser_upsell_node_insert($node) {
  // Only process if upsell is enabled
  if (_fundraiser_upsell_enabled($node)) {
    // Insert the record
    $record = fundraiser_upsell_node_record($node);
    drupal_write_record('fundraiser_upsell', $record);
  }
}

/**
 * Implements hook_node_update().
 */
function fundraiser_upsell_node_update($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    // Check if the upsell data already exists
    $exists = db_result(
      db_query('SELECT nid FROM {fundraiser_upsell} WHERE nid = %d', $node->nid)
    );
    $record = fundraiser_upsell_node_record($node);
    // If upsell is enabled and the record doesn't exist then insert new row
    if (empty($exists) && _fundraiser_upsell_enabled($node)) {
      drupal_write_record('fundraiser_upsell', $record);
    }
    // If the record does exist, then update it
    elseif (!empty($exists)) {
      drupal_write_record('fundraiser_upsell', $record, 'nid');
    }
  }
}

/**
 * Implements hook_node_load().
 */
function fundraiser_upsell_node_load($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    $upsell = db_fetch_array(
      db_query('SELECT * FROM {fundraiser_upsell} WHERE nid = %d', $node->nid)
    );
    // Merge data from upsell settings with node.
    unset($upsell['nid']);
    $node->upsell = $upsell;
  }
}

/**
 * Implements hook_node_delete().
 */
function fundraiser_upsell_node_delete(&$node) {
  db_query('DELETE FROM {fundraiser_upsell} WHERE nid = %d', $node->nid);
}

/**
 * Output an array of the DB structure mapped to the $node object
 */
function fundraiser_upsell_node_record($node) {
  return array(
    'nid' => $node->nid,
    'enabled' => $node->upsell['enabled'],
    'content' => $node->upsell['content'],
    'class' => $node->upsell['class'],
    'modal_width' => $node->upsell['modal_width'],
    'modal_height' => $node->upsell['modal_height'],
    'decoupled' => $node->upsell['decoupled'],
    'campaign_id' => $node->upsell['campaign_id'],
  );
}
