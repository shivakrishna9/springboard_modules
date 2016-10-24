<?php
/**
 * @file
 * Page wrapper functions for springboard groups.
 */

/**
 * Implements hook_page_wrappers_node_settings_form_alter().
 *
 * Alter the wrappers again via an ajax form element.
 */
function springboard_group_page_wrappers_node_settings_form_alter(&$wrappers, &$form, &$form_state) {

  $wrappers_are_grouped = og_get_group_type('node', 'page_wrapper', 'group content');
  // If the pagewrapper content type is group enabled,
  // alter the available options.
  if ($wrappers_are_grouped) {

    // If a group has been selected limit available wrappers to
    // ones assigned to the selected group.
    // @see springboard_group_ajax_callback().
    if (!empty($form_state['values'])) {
      $language = $form['og_group_ref_' . $form['#node']->type]['#language'];
      $option = $form_state['values']['og_group_ref_' . $form['#node']->type][$language];
      $gids = isset($option[0]['target_id']) ? $option : NULL;
      foreach ($gids as $key => $gid) {
        $gids[$key] = $gid['target_id'];
      }

      $filtered_wrappers = springboard_group_page_wrappers_list_wrappers_by_group($gids);
      $wrappers = $filtered_wrappers;
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
 * Display a message about ineligible page wrappers when switching groups.
 */
function springboard_group_node_form_page_wrappers_validate(&$form, &$form_state) {

  // An ajax reload has occurred.
  if (!empty($form_state['values']['page_wrappers'])) {
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
}

