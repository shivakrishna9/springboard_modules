<?php

/**
 * @file
 * Form definition for Group creation and editing.
 */

/**
 * Serves a dual function as group add and edit form.
 *
 * @param null $group_id
 * @return array
 */
function springboard_target_group_form($group_id = NULL) {
  $form = array();

  // Attach our target form js and css.
  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_target') . '/css/sba_target.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'springboard_target') . '/js/sba_target.js';

  // Display a warning if the API is unconfigured and reurn an empty form.
  $unconfigured = springboard_advocacy_api_unconfigured('springboard');
  if(!empty($unconfigured)) {
    $items = implode(', ', $unconfigured);
    $message = t('Please <a href="!url">configure</a> the Springboard API settings. There are empty values in :items. Configuration is required prior to group creation.', array('!url' => url('admin/config/services/advocacy'), ':items' => $items));
    $form = array();
    $form['not_found']['#markup'] = $message;
    return $form;
  }

  $form['#attributes']['class'][] = 'group-edit-form';

  // Load the target if Group ID is present.
  if (isset($group_id) && is_numeric($group_id)) {
    $api_call = springboard_advocacy_api_call();

    $group = $api_call->invokeClientMethod('getTargetGroup', $group_id);

    if (isset($group->error)) {
      $form = array();
      $form['not_found']['#markup'] = $group->error->message;
      return $form;
    }
    $group = $group->data;
  }

  // Set up the form. Use containers to facilitate styling.
  $form['row_one'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'target-row-one',
        'target-row',
      ),
    ),
    '#weight' => -5,
  );

  $form['row_one']['group_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Group Name'),
    '#default_value' => isset($group->group_name) ? $group->group_name : '',
    '#size' => 40,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array('class' => array('group-add', 'group-name')),
  );

  return $form;
}

/**
 * Target edit form validation.
 * @param $form
 * @param $form_state
 */
function springboard_target_group_form_validate($form, &$form_state) {

}

/**
 * Delete a custom target.
 *
 * @param $form
 * @param $form_state
 * @param $group_id
 * @return mixed
 *
 * @TODO - prevent deletion if the group is used in any messages.
 */
function springboard_target_group_delete($form, $form_state, $target_id) {
  $form['group_id'] = array(
    '#type' => 'value',
    '#value' => $group_id,
  );

  $confirmation_question = t('Delete This Group?');
  $cancel_path = 'springboard/advocacy/targets/custom-groups';
  $description = t('Are you sure you want to delete this group?');
  $yes_button = t('Yes');
  $no_button = t('Cancel');
  $confirm_name = 'springboard_target_group_delete';
  return confirm_form($form, $confirmation_question, $cancel_path, $description, $yes_button, $no_button, $confirm_name);
}

/**
 * Submit function for target deletion.
 *
 * @param $form
 * @param $form_state
 */
function springboard_target_group_delete_submit(&$form, &$form_state) {
  $form_state['redirect'] = 'springboard/advocacy/targets/custom-groups';

  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('deleteTargetGroup', $form_state['values']['group_id']);

  if (isset($response->data)) {
    drupal_set_message('Group deleted.');
  }
}
