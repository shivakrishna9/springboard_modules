<?php
/**
 * @file
 * Email wrapper functions for springboard groups.
 */

/**
 * Implements hook_email_wrappers_templates_list_alter().
 *
 * Remove ineligible email wrappers from the email wrapper options form after
 * switching groups.
 */
function springboard_group_email_wrappers_templates_list_alter(&$templates) {
  $wrappers_are_grouped = og_get_group_type('node', 'email_wrapper', 'group content');
  if ($wrappers_are_grouped && arg(1) != 'ajax') {
    $webform = node_load(arg(1));
    $webform_groups = og_get_entity_groups('node', $webform);
    foreach ($templates as $nid => $name) {
      if ($nid != 0) {
        $wrapper = node_load($nid);
        $wrapper_groups = og_get_entity_groups('node', $wrapper);
        // This webform does not have a group, remove grouped templates from
        // the list.
        if (empty($webform_groups['node']) && !empty($wrapper_groups['node'])) {
          unset($templates[$nid]);
        }
        // This wrapper does not have a group but the webform does, remove the
        // wrapper from the list.
        elseif (!empty($webform_groups['node']) && empty($wrapper_groups['node'])) {
          unset($templates[$nid]);
        }
        // Both webform and the email wrapper have groups. Remove the wrapper
        // if the groups do not have a match.
        elseif (!empty($webform_groups['node']) && !empty($wrapper_groups['node'])) {
          if (!array_intersect($webform_groups['node'], $wrapper_groups['node'])) {
            unset($templates[$nid]);
          }
        }
      }
    }
  }
}

/**
 * Check if group selection is incompatible with already
 * configured wrappers or wrapped nodes.
 *
 * Called from springboard_group_form_node_form_alter().
 */
function springboard_group_email_wrappers_node_form__alter(&$form, &$form_state) {

  // We're editing a *wrapped* node and need to validate the wrapper's groups.
  if (!empty($form_state['values']) && !empty($form['#node']->nid) && $form['#node']->type != 'email_wrapper') {
    _springboard_group_email_wrappers_validate_wrapped($form, $form_state);
  }
  // We on a *wrapper* and need to validate nodes that are
  // already wrapped.
  elseif (!empty($form_state['values']) && !empty($form['#node']->nid) && $form['#node']->type == 'email_wrapper') {
    springboard_group_wrappers_common_validation_helper($form, $form_state, 'email_wrappers_webform');
  }
}


/**
 * We're editing a *wrapped* node and need to validate the wrapper's groups.
 * If an email wrapper is no longer eligible due to group restrictions after
 * changing the groups selector on a node form, add a message which is
 * displayed in a cTools modal triggered by the ajax callback on the groups
 * selector. Also delete the bad wrapper on form submit.
 */
function _springboard_group_email_wrappers_validate_wrapped(&$form, &$form_state) {
  $form['#submit'][] = 'springboard_group_delete_email_wrappers';
  $settings = email_wrappers_load_settings($form['#node']->nid);
  if ($settings) {
    $wrappers_to_delete = [];
    // Get the group IDs from the form values.
    $gids = springboard_group_get_gids_from_element($form, $form_state);
    // Get the existing settings. Check the group membership for each wrapper
    // in the setting and compare it to each eligible group.
    foreach ($settings as $setting) {
      $grouped = springboard_group_email_wrappers_get_grouped($gids, $setting);
      // If the wrapper is not in any of the current groups, add it to
      // the removal list.
      if (count($grouped) == 0) {
        $wrappers_to_delete[] = $setting;
      }
    }
    $form_state['wrappers_to_delete'] = $wrappers_to_delete;
    // Set the message.
    if (!empty($wrappers_to_delete)) {
      $count = count($wrappers_to_delete);
      $format = format_plural($count, 'email wrapper', 'email wrappers');
      $form_state['group_ajax_messages'][] = t('@count @format belonging to ineligible groups will be removed when this form is saved. Please update your email wrappers after saving.', array(
        '@count' => $count,
        '@format' => $format,
      ));
    }
  }
}

/**
 * Check if a wrapper is in a set of gids.
 *
 * @param array $gids
 *   Group Ids.
 * @param array $setting
 *   Email wrapper setting.
 *
 * @return array
 *   An array containing active group config.
 */
function springboard_group_email_wrappers_get_grouped($gids, $setting) {
  $grouped = [];
  foreach ($gids as $gid) {
    $in_group = og_get_membership('node', $gid, 'node', $setting['tid']);
    if ($in_group) {
      $grouped[$gid][$setting['tid']] = $in_group;
    }
  }
  return $grouped;
}

/**
 * Delete ineligible email wrappers on node form submssion.
 */
function springboard_group_delete_email_wrappers($form, $form_state) {
  if (!empty($form_state['wrappers_to_delete'])) {
    foreach ($form_state['wrappers_to_delete'] as $settings) {
      email_wrappers_delete_settings($settings['nid'], $settings['eid']);
    }
  }
}
