<?php

/**
 * @file
 * Modifications to the Advocacy Dashboard for groups.
 */

/**
 * Implements hook_springboard_target_api_save_target_group_alter().
 */
function springboard_group_advocacy_springboard_target_api_save_target_group_alter(&$group) {
  // @todo add the og_gid as "affiliate_id" as to the group array.
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_group_advocacy_form_springboard_target_create_custom_groups_form_alter(&$form, &$form_state) {
  springboard_group_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_group_advocacy_form_springboard_target_edit_custom_groups_form_alter(&$form, &$form_state) {
  springboard_group_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_group_advocacy_form_springboard_target_create_custom_targets_form_alter(&$form, &$form_state) {
  springboard_group_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_group_advocacy_form_springboard_target_edit_custom_targets_form_alter(&$form, &$form_state) {
  springboard_group_advocacy_create_og_form_elements($form);
}

/**
 * Helper function to add OG-required elements to advocacy forms.
 *
 * Modifies the custom target and custom group add/edit forms.
 */
function springboard_group_advocacy_create_og_form_elements(&$form) {

  // Get the current user's current group affiliation.
  $session_gid = springboard_group_get_group_context();

  // Check which form we're on.
  if (strpos($form['#form_id'], 'custom_targets') !== FALSE) {
    $type = 'target';
  }
  else {
    $type = 'group';
  }

  // If this is an edit form get the API object's affiliate_id (group id).
  if (!empty($form[$type]['#default_value'])) {
    $gid = $form[$type]['#default_value']->affiliate_id;
    if ($gid != $session_gid && !is_null($session_gid)) {
      drupal_set_message('Session affiliate ID does not match saved affiliate ID.');
      // @todo Deny access if gid is not in users groups.
    }
  }
  else {
    // There's no default value, use the group id from session.
    $gid = $session_gid;
  }

  $title = 'None';
  if (!empty($gid)) {
    $group = node_load($gid);
    $title = $group->title;
  }

  $form['context_hint'] = array(
    '#markup' => t('Editing custom @type for Springboard Group "@group_title"', array(
      '@type' => $type,
      '@group_title' => check_plain($title),
    )),
    '#weight' => -100,
  );

  $form['affiliate_id'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );

}