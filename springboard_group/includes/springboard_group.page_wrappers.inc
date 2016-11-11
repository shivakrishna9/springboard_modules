<?php
/**
 * @file
 * Page wrapper functions for springboard groups.
 */

/**
 * Implements hook_page_wrappers_node_settings_form_wrappers_alter().
 *
 * Alter the wrappers options if a node is grouped.
 */
function springboard_group_page_wrappers_node_settings_form_wrappers_alter(&$wrappers, $form, $form_state) {
  $wrappers = springboard_group_page_wrappers_filter($wrappers, $form, $form_state);
}

/**
 * Alter a wrapped node's edit form.
 *
 * A. Add a vaildate function to verify group selection on submission.
 * B. Rebuild the form if an ajax callback has fired and the group has changed.
 * C. When editing a page wrapper itself, validate that switching groups won't
 * cause a conflict.
 */
function springboard_group_page_wrappers_node_form__alter(&$form, &$form_state, $callback) {
  // We're editing a *wrapped* node and need to validate the wrapper's groups.
  if ($form['#node']->type != 'page_wrapper') {
    $form['#validate'][] = 'springboard_group_page_wrappers_validate_submission';
  }
  // Ajax reload has occurred.
  if (!empty($form_state['values']['page_wrappers']) && $form['#node']->type != 'page_wrapper') {
    if ($callback != 'springboard_group_og_group_field_ajax_callback') {
      return;
    }
    // Get the updated wrappers list.
    $wrappers = _page_wrappers_available_wrappers();
    $wrappers_filtered = springboard_group_page_wrappers_filter($wrappers, $form, $form_state);
    // Rebuild the form.
    page_wrappers_build_node_form($form, $form_state, $wrappers_filtered);
    // Show a message about outdated wrappers if necessary.
    springboard_group_page_wrappers_validate_group_switch($form, $form_state);
  }
  // We on a page *wrapper* and need to validate nodes that are
  // already wrapped.
  elseif (!empty($form_state['values']) && $form['#node']->type == 'page_wrapper') {
    springboard_group_wrapper_validation_helper($form, $form_state, 'node_page_wrappers');
  }
}

/**
 * Validate page wrappers used on a node.
 *
 * Check if a previously configured wrapper is compatible with the new group
 * when switching groups on a wrapped node. Show a message if it isn't.
 */
function springboard_group_page_wrappers_validate_group_switch(&$form, &$form_state) {
  $x = 0;
  foreach ($form_state['values']['page_wrappers'] as $key => $wrapper) {
    $current_wrapper = !empty($form_state['values']['page_wrappers'][$key]['wrapper_nid']) ? $form_state['values']['page_wrappers'][$key]['wrapper_nid'] : NULL;
    $default = !empty($form['#node']->page_wrappers[$x]['wrapper_nid']) ? $form['#node']->page_wrappers[$x]['wrapper_nid'] : NULL;
    $options = $form['springboard_display']['page_wrappers_settings']['page_wrappers'][$key]['wrapper_nid']['#options'];
    if (!in_array($current_wrapper, array_keys($options))) {
      $form_state['group_ajax_messages']['page_wrappers'] = t('A page wrapper selection is not eligible for this group. Please update your page wrapper settings prior to saving this form.');
    }
    elseif (in_array($default, array_keys($options))) {
      $form_state['input']['page_wrappers'][$key]['wrapper_nid'] = $default;
    }
    $x++;
  }
}

/**
 * Validate page wrappers on non-ajax form submission.
 *
 * If an invaild pagewrapper has been selected, somehow, show and error and
 * rebuild the page wrapper selection form.
 */
function springboard_group_page_wrappers_validate_submission(&$form, &$form_state) {
  if (!empty($form_state['triggering_element']['#ajax']['callback'])) {
    return;
  }

  $gids = springboard_group_page_wrappers_get_gids($form, $form_state);
  $allowed_wrappers = springboard_group_page_wrappers_list_wrappers_by_group($gids);
  $error = FALSE;
  foreach ($form_state['values']['page_wrappers'] as $key => $wrapper) {
    if (!empty($wrapper['wrapper_nid']) && !isset($allowed_wrappers[$wrapper['wrapper_nid']])) {
      form_set_error('page_wrappers', 'An invalid page wrapper has been selected.');
      $error = TRUE;
    }
  }
  if ($error) {
    $wrappers = _page_wrappers_available_wrappers();
    $filtered_wrappers = springboard_group_page_wrappers_filter($wrappers, $form, $form_state);
    page_wrappers_build_node_form($form, $form_state, $filtered_wrappers);
  }
}

/**
 * Filter a list of wrappers by the node's group membership.
 */
function springboard_group_page_wrappers_filter(&$wrappers, $form, $form_state) {
  $wrappers_are_grouped = og_get_group_type('node', 'page_wrapper', 'group content');
  // If the pagewrapper content type is group enabled,
  // alter the available options.
  if ($wrappers_are_grouped) {

    // If a group has been selected limit available wrappers to
    // ones assigned to the selected group.
    // @see springboard_group_ajax_callback().
    if (!empty($form_state['values'])) {
      $gids = springboard_group_page_wrappers_get_gids($form, $form_state);
      $wrappers = springboard_group_page_wrappers_list_wrappers_by_group($gids);
    }
    else {
      // We're on a freshly loaded page.
      $gids = NULL;
      $node = $form['#node'];
      if (!empty($node->nid)) {
        // Check if the node we want to wrap is in a group.
        $node_groups = og_get_entity_groups('node', $node);
        // If the node has a group, set the $gid.
        if (!empty($node_groups['node'])) {
          $gids = $node_groups['node'];
        }
      }
      // Alter the form options.
      $wrappers = springboard_group_page_wrappers_list_wrappers_by_group($gids);
    }
  }
  return $wrappers;
}

/**
 * Get the group ids from the OG form element.
 */
function springboard_group_page_wrappers_get_gids($form, $form_state) {
  $language = $form['og_group_ref_' . $form['#node']->type]['#language'];
  $option = $form_state['values']['og_group_ref_' . substr($form['#node']->type, 0, 19)][$language];
  $gids = isset($option[0]['target_id']) ? $option : NULL;
  foreach ($gids as $key => $gid) {
    $gids[$key] = $gid['target_id'];
  }
  return $gids;
}

/**
 * List page wrappers by group.
 *
 * @param int $gids
 *   Group id.
 *
 * @return array
 *   Associative array of page wrapper titles keyed by node id or FALSE.
 */
function springboard_group_page_wrappers_list_wrappers_by_group($gids) {
  $wrappers = array();
  $query = db_select('node', 'n');
  $query->addField('n', 'nid', 'nid');
  $query->addField('n', 'title', 'title');
  $query->leftJoin('og_membership', 'o', 'o.etid=n.nid');
  $query->condition('n.type', 'page_wrapper');
  if (!empty($gids)) {
    $query->condition('o.gid', $gids);
    $query->groupBy('n.nid');
  }
  else {
    $query->IsNull('o.etid');
  }
  $results = $query->execute();
  while ($record = $results->fetchAssoc()) {
    $wrappers[$record['nid']] = $record['title'];
  }

  return count($wrappers) ? $wrappers : array();
}

/**
 * Validates group switching on page wrapper & email wrapper nodes.
 *
 * Check to see what nodes are using this wrapper, and if the wrapper's
 * new group is incompatible with any of the wrapped items,
 * display a message.
 */
function springboard_group_wrapper_validation_helper(&$form, &$form_state, $table = 'node_page_wrapper') {

  // Get all wrappers.
  $wrappers = db_select($table)->fields($table)->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  // Get the active groups selection from the form_state.
  $gids = springboard_group_page_wrappers_get_gids($form, $form_state);

  // Compare the wrapped node's ownership with the new group selection.
  if (!empty($wrappers)) {
    foreach ($wrappers as $wrapper) {
      $grouped = _springboard_group_wrappers_get_grouped_nodes($gids, (array) $wrapper);
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
function _springboard_group_wrappers_get_grouped_nodes($gids, $wrapper) {
  $grouped = [];
  foreach ($gids as $gid) {
    $in_group = og_get_membership('node', $gid, 'node', $wrapper['nid']);
    if ($in_group) {
      $grouped[$gid][$wrapper['nid']] = $in_group;
    }
  }
  return $grouped;
}
