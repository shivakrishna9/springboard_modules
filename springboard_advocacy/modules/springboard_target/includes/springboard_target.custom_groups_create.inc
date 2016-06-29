<?php

/**
 * @file
 * Functions for target creation.
 */

/**
 * Create custom group form.
 *
 * Add create groups form items to the generic form defined
 * in springboard_target.custom_groups.inc.
 *
 * @return array
 *   The Group add form.
 */
function springboard_target_create_custom_groups_form() {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_groups');

  $form = springboard_target_group_form();
  if (!isset($form['not_found'])) {
    $form['#validate'][] = 'springboard_target_group_form_validate';
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => 10,
      '#attributes' => array('class' => 'group-add'),
    );
  }
  return $form;
}


/**
 * Submit function for group create form.
 *
 * @param $form
 * @param $form_state
 */
function springboard_target_create_custom_groups_form_submit($form, &$form_state) {
  $input = $form_state['values'];

  $api_call = springboard_advocacy_api_call();
  $group = $api_call->invokeClientMethod('createCustomGroup', $input); // @TODO Create createCustomGroup method like createCustomTarget

  if (!empty($group->data)) {
    $message = implode(' ', (array) $group->data->status);
    drupal_set_message($message);
    if (isset($group->data->id)) {
      $form_state['redirect'] = 'admin/springboard/advocacy/targets/custom-groups/';
    }
  }
}
