<?php
/**
 * @file
 * Webform ab functions.
 *
 * @todo deal with group affiliation changes on webforms.
 */

/**
 * Implements hook_webform_ab_valid_webforms_alter().
 */
function springboard_group_webform_ab_valid_webforms_alter(&$indexed_forms, $types) {
  $ab_tests_are_grouped = og_get_group_type('node', 'webform_ab', 'group content');
  // If the webform_ab content type is goup enabled, alter the options.
  if ($ab_tests_are_grouped) {
    $node = node_load(arg(1));
    $indexed_forms = springboard_group_webform_ab_get_groups($indexed_forms, $node);
  }
}

/**
 * Determine which forms are avilable to test.
 *
 * @param array $testable_forms
 *   The options array of the webform_ab form.
 * @param object $node
 *   The webform_ab node.
 *
 * @return array
 *    The altered options.
 */
function springboard_group_webform_ab_get_groups($testable_forms, $node) {

  $this_test_groups = og_get_entity_groups('node', $node);
  $gid = NULL;
  // If the test has a group, set the $gid.
  if (!empty($this_test_groups['node'])) {
    $gid = reset($this_test_groups['node']);
  }

  // Alter the form options.
  $re_indexed_forms = array();
  foreach ($testable_forms as $key => $form) {

    $form = node_load($key);
    $form_groups = og_get_entity_groups('node', $form);
    // If the test has a group, and the testable form has
    // the same group, allow it in the form options.
    if ($gid && !empty($form_groups['node'])) {
      $form_gid = reset($form_groups['node']);
      if ($gid && $form_gid == $gid) {
        $re_indexed_forms[$key] = $testable_forms[$key];
      }
    }
    // If the test does not have a group, and the testable form does not
    // a group, allow it in the form options.
    elseif (!$gid && empty($form_groups['node'])) {
      $re_indexed_forms[$key] = $testable_forms[$key];
    }
  }
  return $re_indexed_forms;
}