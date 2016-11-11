<?php

/**
 * Validates group switching on page wrapper & email wrapper nodes.
 *
 * Check to see what nodes are using this wrapper, and if the wrapper's
 * new group is incompatible with any of the wrapped items,
 * display a message.
 */
function springboard_group_wrappers_common_validation_helper(&$form, &$form_state, $table = 'node_page_wrapper') {

  // Get all wrappers.
  $wrappers = db_select($table)->fields($table)->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  // Get the active groups selection from the form_state.
  $gids = springboard_group_get_gids_from_element($form, $form_state);

  // Compare the wrapped node's ownership with the new group selection.
  if (!empty($wrappers)) {
    foreach ($wrappers as $wrapper) {
      $grouped = _springboard_group_wrappers_common_get_grouped_nodes($gids, (array) $wrapper);
      if (count($grouped) == 0) {
        $in_use[] = (array) $wrapper;
      }
    }
  }

  // If we have incompatible nodes already using this wrapper,
  // display a message.
  if (!empty($in_use)) {
    $count = count($in_use);
    $format = format_plural($count, 'node', 'nodes');
    $rows = [];
    foreach ($in_use as $wrapped) {
      $node = node_load($wrapped['nid']);
      $rows[] = l(check_plain($node->title), 'node/' . $wrapped['nid']);
    }

    $message = t('@count @format belonging to an incompatible group use this wrapper.', array(
      '@count' => $count,
      '@format' => $format,
    ));

    if ($count < 11) {
      $message .= theme('item_list', array('items' => $rows));
    }
    $form_state['group_ajax_messages'][] = $message;
  }
}

/**
 * Given a set of $gids, determine if a wrapped node belongs in them.
 *
 * @param array $gids
 *   The group ids of the current wrapper.
 * @param array $wrapper
 *   A node's wrapper table record.
 *
 * @return array
 *   An array of group objects for wrapped nodes belonging to the
 *   wrapper's $gids.
 */
function _springboard_group_wrappers_common_get_grouped_nodes($gids, $wrapper) {
  $grouped = [];
  foreach ($gids as $gid) {
    $in_group = og_get_membership('node', $gid, 'node', $wrapper['nid']);
    if ($in_group) {
      $grouped[$gid][$wrapper['nid']] = $in_group;
    }
  }
  return $grouped;
}
