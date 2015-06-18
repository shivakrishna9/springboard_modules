<?php

/**
 * @file
 * Helper functions for message action taxonomy form.
 */

/**
 * Shows the taxonomy form on the node/add/edit page.
 */
function sba_message_action_taxonomy_form(&$form, &$form_state, $form_id) {

  $form['springboard_taxonomy_options']['#title'] = t('Action options');
  form_load_include($form_state, 'inc', 'springboard_advocacy',
    'includes/springboard_advocacy.webform');
  $language = $form['language']['#value'];
  // Legislative Issues selection.
  if (isset($form['field_sba_legislative_issues']) && !empty($form['field_sba_legislative_issues'][$language])) {
    // Filter out all but the top-tier terms.
    $options =& $form['field_sba_legislative_issues'][$language]['#options'];
    $vocabulary = taxonomy_vocabulary_machine_name_load('sba_legislative_issues');
    // Add indentation class to non-top-tier org issue terms.
    $terms = array();

    foreach (taxonomy_get_tree($vocabulary->vid) as $term) {
      $terms[$term->tid] = $term;
    }
    foreach ($options as $tid => $label) {
      if ($terms[$tid]->depth > 0) {
        unset($options[$tid]);
      }
    }

    // Move the field to a special fieldset inside "Action options."
    $form['springboard_taxonomy_options']['legislative_issues'] = array(
      '#type' => 'fieldset',
      '#title' => t('Choose Legislative Issue(s)'),
      '#description' => t("Many members of Congress require an issue/topic to be selected for each message they receive. If your action is targeting members of Congress, please select the issue that best matches the topic of your message. Based on your choice, Springboard will choose the best available match on each legislator's web form."),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => -1,
    );
    $form['field_sba_legislative_issues'][$language]['#title'] = '';
    $form['springboard_taxonomy_options']['legislative_issues']['field_sba_legislative_issues']
      = $form['field_sba_legislative_issues'];
    unset($form['field_sba_legislative_issues']);

  }
}
