<?php

/**
 * @file
 * Functions for target creation.
 */

function springboard_target_create_custom_targets_form() {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');

  $form = springboard_target_form();
  $form['#validate'][] = 'springboard_target_form_validate';

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}


/**
 * Submit function for target create form
 * @param $form
 * @param $form_state
 */
function springboard_target_create_custom_targets_form_submit($form, &$form_state) {
  $input = $form_state['values'];
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $target = $client->createCustomTarget($input);

  if (!empty($target->data)) {
    $message = implode(' ', (array)$target->data->status);
    if (isset($target->data->id)) {
      $form_state['redirect'] = 'admin/springboard/advocacy/targets/custom-targets/';
     }
  }
  elseif(!empty($target->error)) {
    $message = isset($target->error->message) ? $target->error->message : t('An error occurred when attempting to create the target. Please try again.');
  }
  else {
    $message = t('A response was not received from the API server when attempting to create the target. Please try again.');
  }
  drupal_set_message($message);
  watchdog('advocacy', '@message', array('@message' => $message), WATCHDOG_ERROR);

}

