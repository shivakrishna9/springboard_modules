<?php
/**
 * @file
 * Webform ab functions.
 */

/**
 * Implements hook_webform_ab_valid_webforms_alter().
 */
function springboard_group_webform_ab_valid_webforms_alter(&$indexed_forms, $types) {
  $ab_tests_are_grouped = og_get_group_type('node', 'webform_ab', 'group content');
  // If the webform_ab content type is goup enabled, alter the options.
  if ($ab_tests_are_grouped) {
    $test_node = node_load(arg(1));
    $indexed_forms = springboard_group_webform_ab_get_groups($indexed_forms, $test_node);
  }
}

/**
 * Determine which forms are avilable to test.
 *
 * @param array $webforms
 *   The options array of the webform_ab form.
 * @param object $webform_ab
 *   The webform_ab node.
 *
 * @return array
 *    The altered options.
 */
function springboard_group_webform_ab_get_groups($webforms, $webform_ab) {
  $webform_ab_groups = og_get_entity_groups('node', $webform_ab);
  $gids = NULL;
  // If the test has a group, set the $gid.
  if (!empty($webform_ab_groups['node'])) {
    $gids = $webform_ab_groups['node'];
  }
  // Alter the form options.
  $allowed_forms = array();
  foreach ($webforms as $key => $form_name) {
    $webform = node_load($key);

    $webform_groups = og_get_entity_groups('node', $webform);

    // If the test has a group, and the testable form has
    // the same group, allow it in the form options.
    if ($gids && !empty($webform_groups['node'])) {
      $form_gids = $webform_groups['node'];
      if ($gids && array_intersect($form_gids, $gids)) {
        $allowed_forms[$key] = $webforms[$key];
      }
    }
    // If the test does not have a group, and the testable form does not
    // a group, allow it in the form options.
    elseif (!$gids && empty($webform_groups['node'])) {
      $allowed_forms[$key] = $webforms[$key];
    }
  }
  return $allowed_forms;
}


/**
 * Delete a webform from a test if the group no longer matches.
 *
 * @todo To be called from hook_node_update().
 *
 * @param object $webform
 *   The webform node object.
 */
function springboard_group_webform_ab_remove_form_from_test($webform) {

  $webform_groups = og_get_entity_groups('node', $webform);
  if (!empty($webform_groups['node'])) {
    $gids = $webform_groups['node'];
  }
  if (!empty($gids)) {
    $tests = db_query('SELECT test_nid FROM {webform_ab_forms} WHERE  webform_nid = :webform_nid',
      array(
        ':webform_nid' => $webform->nid,
      ));

    foreach ($tests as $test) {
      $webform_ab = node_load($test->nid);
      $test_groups = og_get_entity_groups('node', $webform_ab);
      // If the test has a group, set the $gid.
      if (!empty($test_groups['node'])) {
        $test_gids = $test_groups['node'];
        if (!array_intersect($test_gids, $gids)) {
          $tables_to_update = array(
            'webform_ab_forms',
            'webform_ab_conversion',
            'webform_ab_hits',
          );
          foreach ($tables_to_update as $table_name) {
            db_delete($table_name)
              ->condition('test_nid', $test->nid)
              ->condition('webform_nid', $webform->nid)
              ->execute();
          }
        }
      }
    }
  }
}

/**
 * Check if group selection removes configured email wrappers.
 *
 * Called from springboard_group_form_node_form_alter(). If an email wrapper
 * is no longer eligible due to group restrictions after changing the groups
 * selector on a node form, add a message which is displayed in a cTools modal
 * triggered by the ajax callback on the groups selector. Also delete the bad
 * wrapper on form submit.
 */
function springboard_group_node_form_webform_ab_validate(&$form, &$form_state) {
  $form['#submit'][] = 'springboard_group_webform_ab_ineligible_tests';

  // An ajax reload has occurred.
  // Make sure this is nt a new node. New nodes can't have email wrappers,
  // yet.
  if (!empty($form_state['values']) && !empty($form['#node']->nid)) {
    $included = $form['#node']->included_webforms;
    if (!empty($included)) {
      $test_gids = [];
      $included_to_delete = [];
      // Get the group IDs from the form values.
      $language = $form['og_group_ref_' . $form['#node']->type]['#language'];
      $options = $form_state['values']['og_group_ref_' . $form['#node']->type][$language];
      foreach ($options as $option) {
        $test_gids[] = $option['target_id'];
      }

      foreach ($included as $webform) {
        $grouped = springboard_group_webform_ab_get_grouped($test_gids, $webform);
        if (count($grouped) == 0) {
          $included_to_delete[] = $webform;
        }
      }

      $form_state['included_to_delete'] = $included_to_delete;

      // Set the message.
      if (!empty($included_to_delete)) {
        $count = count($included_to_delete);
        $format = format_plural($count, 'form', 'form');
        $form_state['group_ajax_messages'][] = t('@count @format belonging to ineligible groups will be removed when this form is saved. Please update your included webforms after saving.', array(
          '@count' => $count,
          '@format' => $format,
        ));
      }
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
function springboard_group_webform_ab_get_grouped($gids, $webform) {
  $grouped = [];
  foreach ($gids as $gid) {
    $in_group = og_get_membership('node', $gid, 'node', $webform['webform_nid']);
    if ($in_group) {
      $grouped[$gid][$webform['webform_nid']] = $in_group;
    }
  }
  return $grouped;
}

/**
 * Delete ineligible email wrappers on node form submssion.
 */
function springboard_group_webform_ab_ineligible_tests($form, $form_state) {
  if (!empty($form_state['included_to_delete'])) {
    foreach ($form_state['included_to_delete'] as $webform) {
      $tables_to_update = array(
        'webform_ab_forms',
        'webform_ab_conversion',
        'webform_ab_hits',
      );
      foreach ($tables_to_update as $table_name) {
        db_delete($table_name)
          ->condition('test_nid', $form['#node']->nid)
          ->condition('webform_nid', $webform['webform_nid'])
          ->execute();
      }
    }
  }
}
