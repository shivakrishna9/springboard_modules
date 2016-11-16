<?php

/**
 * @file
 * Modifications to the Advocacy Dashboard for groups.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_group_advocacy_form_springboard_target_create_custom_groups_form_alter(&$form, &$form_state) {
  springboard_group_advocacy_create_og_form_elements($form);
}

function springboard_group_advocacy_form_sba_message_edit_form_alter(&$form, &$form_state) {
  $session_gid = springboard_group_get_group_context();
  $form['affiliate_id'] = array(
    '#type' => 'hidden',
    '#value' => $session_gid,
  );
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
    $gid = $form[$type]['#value']->affiliate_id;
    // @todo make this unecessary somehow.
    unset($form[$type]);
  }
  else {
    $type = 'group';
    $gid = $form[$type]['#default_value']->affiliate_id;
    unset($form[$type]);
  }
  // If this is an edit form get the API object's affiliate_id (group id).
  if ($gid) {
    if ($gid != $session_gid && !is_null($session_gid) && ($session_gid != '_none' && $session_gid != '_all')) {
      drupal_set_message('Session affiliate ID does not match saved affiliate ID.');
      // @todo Deny access if gid is not in users groups.
    }
  }
  else {
    // There's no default value, use the group id from session.
    if ($session_gid != '_none' && $session_gid != '_all') {
      $gid = $session_gid;
    }
    else {
      $gid = NULL;
    }
  }

  if (!empty($gid) && ($gid != '_none' && $gid != '_all')) {
    // @todo groups can be deleted, but affiliate id persist on api server?
    $group = node_load($gid);
    if (!empty($group) && $group->type == 'springboard_group') {
      $title = $group->title;
      $form['context_hint'] = array(
        '#markup' => t('Editing custom @type for Springboard Group "@group_title"', array(
          '@type' => $type,
          '@group_title' => check_plain($title),
        )),
        '#weight' => -100,
      );
    }
  }
  $form['affiliate_id'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );

}