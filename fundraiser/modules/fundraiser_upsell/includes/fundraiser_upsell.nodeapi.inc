<?php
/**
 * @file fundraiser_upsell.nodeapi.inc
 * Nodeapi hooks
 */

/**
 * Implements hook_node_insert().
 */
function fundraiser_upsell_node_insert($node) {
  // Only process if upsell is enabled
  if (!empty($node->fundraiser_upsell_enabled) && $node->fundraiser_upsell_enabled) {
    // Create the record aray for insertion
    $record = array(
      'nid' => $node->nid,
      'fundraiser_upsell_enabled' => $node->fundraiser_upsell_enabled,
      'fundraiser_upsell_content' => $node->fundraiser_upsell_content,
      'upsell_class' => $node->upsell_class,
      'modal_width' => $node->modal_width,
      'modal_height' => $node->modal_height,
    );
    // Insert the record 
    drupal_write_record('fundraiser_upsell_form_settings', $record);
  }
}

/**
 * Implements hook_node_update().
 */
function fundraiser_upsell_node_update($node) {  
  // Only process if upsell is enabled
  if (!empty($node->fundraiser_upsell_enabled) && $node->fundraiser_upsell_enabled) {
    // Check if the upsell data already exists
    $exists = db_query('
      SELECT nid
      FROM {fundraiser_upsell_form_settings}
      WHERE nid = :nid',
      array(':nid' => $node->nid)
    )->fetchField();
    // Create the record aray for insertion
    $record = array(
      'nid' => $node->nid,
      'fundraiser_upsell_enabled' => $node->fundraiser_upsell_enabled,
      'fundraiser_upsell_content' => $node->fundraiser_upsell_content,
      'upsell_class' => $node->upsell_class,
      'modal_width' => $node->modal_width,
      'modal_height' => $node->modal_height,
    );
    // Insert or update the record
    if (!empty($exists)) {
      drupal_write_record('fundraiser_upsell_form_settings', $record, 'nid');
    }
    elseif ($node->fundraiser_upsell_enabled > 0) {
      drupal_write_record('fundraiser_upsell_form_settings', $record);
    }
  }
}

/**
 * Implements hook_node_load().
 */
function fundraiser_upsell_node_load($nodes, $types) {
  foreach ($nodes as $node) {
    // If this isn't a fundraiser type, ignore it.
    if (fundraiser_is_donation_type($node->type)) {
      $upsell = db_query('
        SELECT fundraiser_upsell_enabled, fundraiser_upsell_content, upsell_class, modal_width, modal_height
        FROM {fundraiser_upsell_form_settings}
        WHERE nid = :nid',
        array(':nid' => $node->nid)
      )->fetchAssoc();
      // Merge data from upsell settings with node.
      $upsell = (array) $upsell;
      foreach ($upsell as $key => $value) {
        $nodes[$node->nid]->$key = $value;
      }
    }
  }
}

/**
 * Implements hook_node_delete().
 */
function fundraiser_upsell_node_delete(&$node) {
  db_query('DELETE FROM {fundraiser_upsell_form_settings} WHERE nid = :nid', array(':nid' => $node->nid));
}
