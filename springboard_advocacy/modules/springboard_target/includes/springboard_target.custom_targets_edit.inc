<?php


function springboard_target_edit_custom_targets_form($form, &$form_state, $target_id) {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');

  $form = springboard_target_form($target_id);
  $form['submit'] = array('#type' => 'submit', '#value' => t('Update'));

  return $form;

}

function springboard_target_edit_custom_targets_form_validate($form, &$form_state) {

}

function springboard_target_edit_custom_targets_form_submit($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  $input = $form_state['values'];
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $target = $client->updateCustomTarget($input, $id);
}

