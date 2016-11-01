<?php
/**
 * @file
 * An example of an approach to dealing with p2p. Incomplete.
 */

/**
 * Implements hook_form_alter().
 */
function springboard_group_form_p2p_campaign_landing_node_form_alter(&$form, &$form_state, $form_id) {
  springboard_group_p2p_form_prepare($form, $form_state);
}

function springboard_group_form_p2p_campaign_node_form_alter(&$form, &$form_state, $form_id) {
  springboard_group_p2p_form_prepare($form, $form_state);
}

function springboard_group_p2p_form_prepare(&$form, &$form_state) {
  $form['og_group_ref_' . substr($form['#node']->type, 0, 19)]['#weight'] = -1;
  $form['#fieldgroups']['group_tabs']->weight = 2;
  $form['#attached']['js'][] = drupal_get_path('module', 'springboard_group') . '/js/autocomplete-cache.js';
}
