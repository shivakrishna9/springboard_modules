<?php


function springboard_m2t_create_custom_targets_form() {

  module_load_include('inc', 'springboard_m2t', 'includes/springboard_m2t.custom_targets');

  $form = springboard_m2t_target_form();

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

function springboard_m2t_create_custom_targets_form_validate($form, &$form_state) {

}

function springboard_m2t_create_custom_targets_form_submit($form, &$form_state) {
  $input = $form_state['values'];
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $target = $client->createCustomTarget($input);
  if (isset($target->id)) {
    $form_state['redirect'] = 'admin/springboard/advocacy/custom-targets/' . $target->id . '/edit';
  }
}

