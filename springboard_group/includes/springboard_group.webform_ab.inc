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
 * Determine which forms are available to test for the selected group.
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
 * Check if new group selection invalidates test configurations.
 *
 * Called from springboard_group_form_node_form_alter(). If a webform
 * is no longer eligible due to group restrictions after changing the groups
 * selector on a test or on a test's child forms, add a message which is
 * displayed in a cTools modal triggered by the ajax callback on the groups
 * selector. Also remove the bad forms from the test on form submit.
 */
function springboard_group_webform_ab_node_form__alter(&$form, &$form_state) {
  $form['#submit'][] = 'springboard_group_webform_ab_ineligible_tests';
  // An ajax reload has occurred.
  // Make sure this is not a new node. New nodes can't child forms,
  // yet.
  if (!empty($form_state['values']) && !empty($form['#node']->nid)) {

    $gids = springboard_group_get_gids_from_element($form, $form_state);

    // If we've changed the group on a test.
    if ($form['#node']->type == 'webform_ab') {
      _springboard_group_webform_ab_valdiate_test($form, $form_state, $gids);
    }
    // If we've changed the group on a child form.
    else {
      _springboard_group_webform_ab_validate_child_form($form, $form_state, $gids);
    }
  }
}

/**
 * Validate ab tests when they are edited.
 *
 * When changing the group ownership of an ab test, make sure any child forms
 * are still in eligible groups.
 */
function _springboard_group_webform_ab_valdiate_test(&$form, &$form_state, $gids) {
  $included_to_delete = [];
  $included = $form['#node']->included_webforms;
  if (!empty($included)) {
    foreach ($included as $webform) {
      $grouped = springboard_group_webform_ab_get_grouped($gids, $webform, 'test');
      if (count($grouped) == 0) {
        $included_to_delete[] = $webform;
      }
    }
  }
  $form_state['included_to_delete'] = $included_to_delete;

  if (!empty($included_to_delete)) {
    $count = count($included_to_delete);
    $format = format_plural($count, 'webform', 'webforms');
    $uses = format_plural($count, 'is', 'are');
    $rows = [];
    foreach ($included_to_delete as $included) {
      $node = node_load($included['webform_nid']);
      $rows[] = l(check_plain($node->title), 'node/' . $included['webform_nid'], array('attributes' => array('target' => '_blank')));
    }

    $message = t('@count @format belonging to an incompatible group @use included in this test, and will be removed when this form is saved.', array(
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

/**
 * Validate child forms when they are edited.
 *
 * Validate a form that is included in a test if the group affiliation on the
 * form is changed, to see if it still eligble to be included in the test.
 */
function _springboard_group_webform_ab_validate_child_form(&$form, &$form_state, $gids) {
  $ineligible_tests = [];
  $included_in_tests = db_query('SELECT * FROM {webform_ab_forms} f WHERE f.webform_nid = :webform_nid ',
    array(':webform_nid' => $form['#node']->nid));
  foreach ($included_in_tests as $webform) {
    $grouped = springboard_group_webform_ab_get_grouped($gids, (array) $webform, 'webform');
    if (count($grouped) == 0) {
      $ineligible_tests[] = (array) $webform;
    }
  }

  $form_state['included_to_delete'] = $ineligible_tests;
  // Set the message.
  if (!empty($ineligible_tests)) {
    $count = count($ineligible_tests);
    $format = format_plural($count, 'test', 'tests');
    $uses = format_plural($count, 'is', 'are');
    $rows = [];
    foreach ($ineligible_tests as $ineligible) {
      $node = node_load($ineligible['test_nid']);
      $rows[] = l(check_plain($node->title), 'node/' . $ineligible['test_nid'], array('attributes' => array('target' => '_blank')));
    }
    $message = t('This form will be removed from @count multivariate @format belonging to ineligible groups when this form is saved.', array(
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


/**
 * Check if a test is in a set of gids.
 *
 * @param array $gids
 *   Group Ids.
 * @param array $setting
 *   Email wrapper setting.
 *
 * @return array
 *   An array containing active group config.
 */
function springboard_group_webform_ab_get_grouped($gids, $webform, $type = 'test') {
  $grouped = [];
  if ($type == 'test') {
    $id = $webform['webform_nid'];
  }
  else {
    $id = $webform['test_nid'];
  }

  foreach ($gids as $gid) {
    $in_group = og_get_membership('node', $gid, 'node', $id);
    if ($in_group) {
      $grouped[$gid][$id] = $in_group;
    }
  }
  return $grouped;
}

/**
 * Delete ineligible forms on node form submssion.
 */
function springboard_group_webform_ab_ineligible_tests($form, $form_state) {
  if (!empty($form_state['included_to_delete'])) {
    foreach ($form_state['included_to_delete'] as $webform) {
      if ($form['#node']->type == 'webform_ab') {
        $test_id = $form['#node']->nid;
      }
      else {
        $test_id = $webform['test_nid'];
      }
      $tables_to_update = array(
        'webform_ab_forms',
        'webform_ab_conversion',
        'webform_ab_hits',
      );
      foreach ($tables_to_update as $table_name) {
        db_delete($table_name)
          ->condition('test_nid', $test_id)
          ->condition('webform_nid', $webform['webform_nid'])
          ->execute();
      }
    }
  }
}


/**
 * Delete a webform from a test if the group no longer matches.
 *
 * @todo do we need to call this from hook_node_update to handle programmatic
 * resaves?
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
