<?php

/**
 * Build the group selector form for advocacy dashboard page.
 *
 * @param array $groups
 *   The current user's possible groups.
 *
 * @return array
 *   The form.
 */
function springboard_og_advocacy_group_selector_form($form, &$form_state, $groups) {

  $form = [];
  $form['title']['#markup'] = '<strong>' . t('Choose an affiliate group to work in:') . '</strong>';

  // Build the selector form from groups found in user session.
  $options = [];
  foreach ($groups as $nid) {
    $node = node_load($nid);
    $options[$nid] = check_plain($node->title);
  }

  // The ajax enabled group selector which will reload the dashboard
  // subsequent to a group choice. The group ID will get put into session,
  // where the search query plugin will pick it up and filter the group listing
  // by group ID.
  $form['groups'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => springboard_og_advocacy_get_group_context(),
    '#ajax' => array(
      'callback' => 'springboard_og_advocacy_group_selector',
      'wrapper' => 'sba-dashboard',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#prefix' => '<div id="group-selector-div">',
    '#suffix' => '</div>',
  );

  // Get the page argument that the dashboard callback expects.
  // We'll need it later when we  reload the dashboard via ajax.
  if (isset($form_state['values']['path'])) {
    $path = $form_state['values']['path'];
  }
  else {
    $item = menu_get_item();
    $path = $item['page_arguments'][0];
  }
  $form['path'] = array(
    '#type' => 'value',
    '#value' => $path,
  );

  return $form;
}


/**
 * AJAX callback for group selector form.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function springboard_og_advocacy_group_selector($form, &$form_state) {
  // Put the chosen group into session, so it can be retrieved by
  // springboard_og_advocacy_og_context_handler().
  $_SESSION['springboard_og_advocacy_working_group'] = $form_state['values']['groups'];

  // Reload the dashboard so the views result can filter by the selected group.
  $dashboard = springboard_advocacy_dashboard($form_state['values']['path']);
  return $dashboard;
}

/**
 * Implements hook_springboard_target_api_save_target_group_alter().
 */
function springboard_og_advocacy_springboard_target_api_save_target_group_alter(&$group) {
  // @todo add the og_gid as "affiliate_id" as to the group array.
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_og_advocacy_form_springboard_target_create_custom_groups_form_alter(&$form, &$form_state) {
  springboard_og_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_og_advocacy_form_springboard_target_edit_custom_groups_form_alter(&$form, &$form_state) {
  springboard_og_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_og_advocacy_form_springboard_target_create_custom_targets_form_alter(&$form, &$form_state) {
  springboard_og_advocacy_create_og_form_elements($form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the extra form elements we need to the advocacy CRUD forms.
 */
function springboard_og_advocacy_form_springboard_target_edit_custom_targets_form_alter(&$form, &$form_state) {
  springboard_og_advocacy_create_og_form_elements($form);
}

/**
 * Helper function to add OG-required elements to advocacy forms.
 */
function springboard_og_advocacy_create_og_form_elements(&$form) {

  // Get the current user's current group affiliation.
  $session_gid = springboard_og_advocacy_get_group_context();

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