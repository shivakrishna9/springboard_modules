<?php
/**
 * @file
 */

/**
 * Implements hook_nodeapi().
 */
function sustainer_upsell_nodeapi(&$node, $op, $teaser, $page) {
  $donation_node = fundraiser_is_donation_type($node->type);
  if ($donation_node) {
    switch($op) {
      case 'load':
        sustainer_upsell_node_load($node);
        break;

      case 'insert':
        sustainer_upsell_node_insert($node);
        break;

      case 'update':
        sustainer_upsell_node_update($node);
        break;

      case 'delete':
        sustainer_upsell_node_delete($node);
        break;
    }
  }
}


/**
 * Implements the load $op in hook_nodeapi().
 *
 * @param $node
 *   The node being loaded.
 */
function sustainer_upsell_node_load(&$node) {
  $sustainer = db_fetch_array(db_query("SELECT sustainer_upsell_enabled, sustainer_upsell_content FROM {sustainer_upsell_form_settings} WHERE nid = %d", $node->nid));
  if(!empty($sustainer)) {
    foreach ($sustainer as $field => $value) {
      $node->{$field} = $value;
    }
  }
}

/**
 * Implements the update $op in hook_nodeapi().
 *
 * @param $node
 *   The node being updated.
 */
function sustainer_upsell_node_update(&$node) {
  $sustainer = db_fetch_array(db_query("SELECT sustainer_upsell_enabled, sustainer_upsell_content FROM {sustainer_upsell_form_settings} WHERE nid = %d", $node->nid));
  if(!empty($sustainer)) {
    db_query(
    "
      UPDATE {sustainer_upsell_form_settings} SET 
        sustainer_upsell_enabled = '%s',
        sustainer_upsell_content = '%s'
      WHERE nid = %d
    ",
      $node->sustainer_upsell_enabled,
      $node->sustainer_upsell_content,
      $node->nid
    );
  }
  else {
    db_query(
    "
      INSERT INTO {sustainer_upsell_form_settings}
      (
        nid,
        sustainer_upsell_enabled,
        sustainer_upsell_content
      )
      VALUES (%d, '%s', '%s')
    ",
      $node->nid,
      $node->sustainer_upsell_enabled,
      $node->sustainer_upsell_content
    );
  }
}

/**
 * Deletes a sustainer setting record when the node is deleted.
 */
function sustainer_upsell_node_delete(&$node) {
  db_query("DELETE FROM {sustainer_upsell_form_settings} WHERE nid = %d", $node->nid);
}

/**
 * Inserts data for new donation forms.
 *
 * @param $node
 *   The new donation node being inserted.
 */
function sustainer_upsell_node_insert(&$node) {
  db_query(
    "
      INSERT INTO {sustainer_upsell_form_settings}
      (
        nid,
        sustainer_upsell_enabled,
        sustainer_upsell_thank_you_content
      )
      VALUES (%d, '%s', '%s')
    ",
      $node->nid,
      $node->sustainer_upsell_enabled,
      $node->sustainer_upsell_content
  );
}