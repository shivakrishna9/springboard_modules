<?php

// Needed for SpringboardGroup_EntityReference_SelectionHandler_Generic.class.php
require_once drupal_get_path('module', 'entityreference') . '/plugins/selection/EntityReference_SelectionHandler_Generic.class.php';
/**
 * Implements hook_query_TAG_alter().
 *
 * Not currently used but would be required for p2p autocompletes and other
 * entity reference fields.
 */
function springboard_group_query_entityreference_alter(&$query) {
  if (isset($query->alterTags['entityreference'])) {

    $referencefields = array();

    if (in_array($query->alterMetaData['field']['field_name'], $referencefields)) {

      // Get the current active group IDs.
      $context = (array) $_SESSION['springboard_group_entityreference_context'];
      if (!empty($context)) {
        // Build the query.
        $condition = db_and();
        if ($context == '_none' || $context == '_all') {
          if ($context == '_none') {
            $condition->isNull('og_membership.gid');
          }
          else {
            $all_groups = springboard_group_get_all_springboard_groups();
            $condition->condition('og_membership.gid', $all_groups, 'IN');
            $condition->condition('og_membership.entity_type', 'node');
          }
        }
        else {
          $condition->condition('og.gid', $context, 'IN');
          $condition->condition('og.entity_type', 'node');
        }

        $query->join('og_membership', 'og', 'node.nid = og.etid');
        $query->condition($condition);
      }
    }
  }
}

