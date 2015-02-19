<?php

/**
 * @file
 * Functions for target updates.
 */

function springboard_target_edit_custom_targets_form($form, &$form_state, $target_id) {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');

  $form = springboard_target_form($target_id);
  //@todo theme function
  $form['header'] = array(
     '#markup'=> '<h2>'
       . check_plain($form['row_one']['salutation']['#default_value'])
      . ' '
       . check_plain($form['row_one']['first_name']['#default_value'])
       . ' '
       . check_plain($form['row_one']['last_name']['#default_value'])
       . '</h2>',
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
  if (empty($target->error)) {
    $message = implode(' ', (array)$target->data->status);
  }
  else {
    $message = $target->error->message;
  }
  drupal_set_message($message);
}

