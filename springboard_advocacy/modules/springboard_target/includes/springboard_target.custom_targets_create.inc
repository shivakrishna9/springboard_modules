<?php

/**
 * @file
 * Functions for target creation.
 */

function springboard_target_create_custom_targets_form() {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');

  $form = springboard_target_form();

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

function springboard_target_create_custom_targets_form_validate($form, &$form_state) {

}

function springboard_target_create_custom_targets_form_submit($form, &$form_state) {
  $input = $form_state['values'];
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $target = $client->createCustomTarget($input);
  
  if (empty($target->error)) {
    $message = implode(' ', (array)$target->data->status);
    if (isset($target->data->id)) {
      $form_state['redirect'] = 'admin/springboard/advocacy/custom-targets/' . $target->data->id . '/edit';
     }
  }
  else {
    $message = $target->error->message;
  }
  drupal_set_message($message);
}

