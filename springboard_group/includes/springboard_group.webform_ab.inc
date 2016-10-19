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
 * @param array $testable_forms
 *   The options array of the webform_ab form.
 * @param object $test_node
 *   The webform_ab node.
 *
 * @return array
 *    The altered options.
 */
function springboard_group_webform_ab_get_groups($testable_forms, $test_node) {
  $this_test_groups = og_get_entity_groups('node', $test_node);
  $gid = NULL;
  // If the test has a group, set the $gid.
  if (!empty($this_test_groups['node'])) {
    $gid = reset($this_test_groups['node']);
  }

  // Alter the form options.
  $re_indexed_forms = array();
  foreach ($testable_forms as $key => $form_name) {
    $testable_form = node_load($key);

    $testable_form_groups = og_get_entity_groups('node', $testable_form);
    // If the test has a group, and the testable form has
    // the same group, allow it in the form options.
    if ($gid && !empty($testable_form_groups['node'])) {
      $form_gid = reset($testable_form_groups['node']);
      if ($gid && $form_gid == $gid) {
        $re_indexed_forms[$key] = $testable_forms[$key];
      }
    }
    // If the test does not have a group, and the testable form does not
    // a group, allow it in the form options.
    elseif (!$gid && empty($testable_form_groups['node'])) {
      $re_indexed_forms[$key] = $testable_forms[$key];
    }
  }
  return $re_indexed_forms;
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
    $gid = reset($webform_groups['node']);
  }
  if (!empty($gid)) {
    $tests = db_query('SELECT test_nid FROM {webform_ab_forms} WHERE  webform_nid = :webform_nid',
      array(
        ':webform_nid' => $webform->nid,
      ));

    foreach ($tests as $test) {
      $test_node = node_load($test->nid);
      $test_groups = og_get_entity_groups('node', $test_node);
      // If the test has a group, set the $gid.
      if (!empty($test_groups['node'])) {
        $test_gid = reset($test_groups['node']);
        if ($test_gid != $gid) {
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