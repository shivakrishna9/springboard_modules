<?php

/**
 * Implements hook_page_wrappers_node_settings_form_alter().
 *
 * Alter the wrappers again via an ajax form element.
 */
function springboard_group_page_wrappers_node_settings_form_alter(&$wrappers, &$form, $form_state) {

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
          // @todo we need some js/ajax here to deal with page reloads/validation reloads after the group selector has been changed.
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
 * @param int $gid
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
 * Implements hook_email_wrappers_templates_list_alter().
 */
function springboard_group_email_wrappers_templates_list_alter(&$templates) {
  $wrappers_are_grouped = og_get_group_type('node', 'email_wrapper', 'group content');
  if ($wrappers_are_grouped) {
    $webform = node_load(arg(1));
    $webform_groups = og_get_entity_groups('node', $webform);
    foreach ($templates as $nid => $name) {
      if ($nid != 0) {
        $wrapper = node_load($nid);
        $wrapper_groups = og_get_entity_groups('node', $wrapper);
        if (empty($webform_groups['node']) && !empty($wrapper_groups['node'])) {
          unset($templates[$nid]);
        }
        elseif (!empty($webform_groups['node']) && empty($wrapper_groups['node'])) {
          unset($templates[$nid]);
        }
        elseif (!empty($webform_groups['node']) && !empty($wrapper_groups['node'])) {
          if (!array_intersect($webform_groups['node'], $wrapper_groups['node'])) {
            unset($templates[$nid]);
          }
        }
      }
    }
  }
}