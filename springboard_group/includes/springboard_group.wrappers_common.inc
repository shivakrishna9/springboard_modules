<?php

/**
 * Validates group switching on page wrapper & email wrapper nodes.
 *
 * Check to see what nodes are using this wrapper, and if the wrapper's
 * new group is incompatible with any of the wrapped items,
 * display a message.
 */
function springboard_group_wrappers_common_validation_helper(&$form, &$form_state, $table = 'node_page_wrapper') {

  // Get all wrapper records.
  $all_wrapper_data = db_select($table)->fields($table)->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  // Get the wrapper's current groups.
  $this_wrapper_old_gids = og_get_entity_groups('node', $form['#node']);

  // Get the active groups selection from the form_state.
  $this_wrapper_new_gids = springboard_group_get_gids_from_element($form, $form_state);

  // Compare the wrapped node's ownership with the new group selection.
  if (!empty($all_wrapper_data)) {
    foreach ($all_wrapper_data as $wrapper_item) {
      // If the wrapper data belongs to this wrapper.
      $wrapper_nid = isset($wrapper_item['wrapper_nid']) ? $wrapper_item['wrapper_nid'] : $wrapper_item['tid'];
      if ($wrapper_nid == $form['#node']->nid) {

        // Get the wrapped node so we can get its groups.
        $wrapped_node = node_load($wrapper_item['nid']);
        $wrapped_node_groups = og_get_entity_groups('node', $wrapped_node);
        // Compare the wrapped nodes groups to this wrappers new groups.
        if (!empty($this_wrapper_old_gids)) {
          $valid = array_intersect($this_wrapper_new_gids, $wrapped_node_groups['node']);
          if (empty($valid)) {
            $in_use[] = (array) $wrapped_node;
          }
        }
        else {
          // This wrapper had no group to begin with.
          $valid = !empty($wrapped_node_groups['node']);
          // If there's no match we have a problem.
          if (empty($valid) && !empty($this_wrapper_new_gids)) {
            $in_use[] = (array) $wrapped_node;
          }
        }

      }
    }
  }

  // If we have incompatible nodes already using this wrapper,
  // display a message.
  if (!empty($in_use)) {
    $count = count($in_use);
    $format = format_plural($count, 'node', 'nodes');
    $uses = format_plural($count, 'uses', 'use');
    $rows = [];
    foreach ($in_use as $wrapped) {
      $node = node_load($wrapped['nid']);
      $rows[] = l(check_plain($node->title), 'node/' . $wrapped['nid'], array('attributes' => array('target' => '_blank')));
    }

    $message = t('@count @format belonging to an incompatible group @use this wrapper.', array(
      '@count' => $count,
      '@format' => $format,
      '@use' => $uses,
    ));

    if ($count < 11) {
      $message .= theme('item_list', array('items' => $rows));
    }
    $form_state['group_ajax_messages'][] = $message;
  }
}
