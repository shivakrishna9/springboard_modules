<?php

/**
 * @file
 * Functions for target updates.
 */

function springboard_target_edit_custom_targets_form($form, &$form_state, $target_id) {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');

  $form = springboard_target_form($target_id);

  $name = array(
    $form['row_one']['salutation']['#default_value'],
    $form['row_one']['first_name']['#default_value'],
    $form['row_one']['last_name']['#default_value']
  );
  $form['header'] = array(
    '#prefix' => '<h2>',
    '#suffix' => '</h2>',
    '#markup'=> check_plain(implode(' ', $name)),
    '#weight' => '-100' ,
  );

  if (!isset($form['not_found'])) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Update'));
  }
  $form['#validate'][] = 'springboard_target_form_validate';

  return $form;

}

/**
 * Submit function for target edit form
 * @param $form
 * @param $form_state
 */
function springboard_target_edit_custom_targets_form_submit($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  $input = $form_state['values'];
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $target = $client->updateCustomTarget($input, $id);
  if (!empty($target->data)) {
    $message = implode(' ', (array)$target->data->status);
  }
  elseif(!empty($target->error)) {
    $message = isset($target->error->message) ? $target->error->message : t('An error occurred when attempting to update the target. Please try again.');
  }
  else {
    $message = t('A response was not received from the API server when attempting to update the target. Please try again.');
  }
  drupal_set_message($message);
}

