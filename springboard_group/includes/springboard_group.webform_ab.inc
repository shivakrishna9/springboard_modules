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
  $gid = NULL;
  // If the test has a group, set the $gid.
  if (!empty($webform_ab_groups['node'])) {
    $gid = reset($webform_ab_groups['node']);
  }

  // Alter the form options.
  $allowed_forms = array();
  foreach ($webforms as $key => $form_name) {
    $webform = node_load($key);

    $webform_groups = og_get_entity_groups('node', $webform);
    // If the test has a group, and the testable form has
    // the same group, allow it in the form options.
    if ($gid && !empty($webform_groups['node'])) {
      $form_gid = reset($webform_groups['node']);
      if ($gid && $form_gid == $gid) {
        $allowed_forms[$key] = $webforms[$key];
      }
    }
    // If the test does not have a group, and the testable form does not
    // a group, allow it in the form options.
    elseif (!$gid && empty($webform_groups['node'])) {
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
    $gid = reset($webform_groups['node']);
  }
  if (!empty($gid)) {
    $tests = db_query('SELECT test_nid FROM {webform_ab_forms} WHERE  webform_nid = :webform_nid',
      array(
        ':webform_nid' => $webform->nid,
      ));

    foreach ($tests as $test) {
      $webform_ab = node_load($test->nid);
      $test_groups = og_get_entity_groups('node', $webform_ab);
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