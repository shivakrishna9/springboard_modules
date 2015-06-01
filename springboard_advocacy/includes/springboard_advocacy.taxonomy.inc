<?php

/**
 * Helper function defines advocacy taxonomy fields
 * added via hook_form_alter above.
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function springboard_advocacy_taxonomy_form(&$form, &$form_state, $form_id) {
  module_load_include('inc', 'springboard_advocacy', 'includes/springboard_advocacy.webform');

  $language = $form['language']['#value'];
  // Add a vertical-tab fieldset for "Action options."
  $form['springboard_advocacy_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Action options'),
    '#weight' => -1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'additional_settings',
  );
  // Include JS to layout long lists of checkboxes in columns.
  drupal_add_js(drupal_get_path('module', 'springboard_advocacy') . '/js/jquery.columnizer.min.js');
  drupal_add_js(drupal_get_path('module', 'springboard_advocacy') . '/js/message-action-edit-form.js');
  // Include CSS for special layout.
  drupal_add_css(drupal_get_path('module', 'springboard_advocacy') . '/css/message-action-edit-form.css');

  // Legislative Issues selection
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
    $form['springboard_advocacy_options']['legislative_issues'] = array(
      '#type' => 'fieldset',
      '#title' => t('Choose Legislative Issue(s)'),
      '#description' => t(
        "Many members of Congress require an issue/topic to be selected for each message they receive. "
        . "If your action is targeting members of Congress, please select the issue that best matches the topic of your message. "
        . "Based on your choice, Springboard will choose the best available match on each legislator's web form."
      ),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['field_sba_legislative_issues'][$language]['#title'] = '';
    $form['springboard_advocacy_options']['legislative_issues']['field_sba_legislative_issues'] =
      $form['field_sba_legislative_issues']
    ;
    unset($form['field_sba_legislative_issues']);
  }

  // Organization Issues selection
  if (isset($form['field_sba_organization_issues']) && !empty($form['field_sba_organization_issues'][$language])) {
    $form['field_sba_organization_issues']['#pre_render'][] = 'springboard_advocacy_sba_organization_issues_pre_render';
    // Remove any leading dashes from option labels. (We'll indent them in the pre-render instead.)
    $options =& $form['field_sba_organization_issues'][$language]['#options'];
    foreach ($options as $tid => $label) {
      $options[$tid] = preg_replace("/^\\-+/", '', $options[$tid]);
    }

    // Move the field to a special fieldset inside "Action options."
    $form['springboard_advocacy_options']['organization_issues'] = array(
      '#type' => 'fieldset',
      '#title' => t('Choose Organization Issue(s)'),
      '#description' => t(
        "Select the issues related to this action. These are your organization's issues, "
        . "which can be used to generate feeds and lists of related actions in your action center "
        . "and throughout your website."
      ),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['field_sba_organization_issues'][$language]['#title'] = '';
    $form['springboard_advocacy_options']['organization_issues']['field_sba_organization_issues'] =
      $form['field_sba_organization_issues']
    ;
    unset($form['field_sba_organization_issues']);
  }
}


/**
 * @param $element
 * @return mixed
 */
function springboard_advocacy_sba_organization_issues_pre_render($element) {
  $vocabulary = taxonomy_vocabulary_machine_name_load('sba_organization_issues');
  // Add indentation class to non-top-tier org issue terms.
  $terms = array();
  foreach (taxonomy_get_tree($vocabulary->vid) as $term) {
    $terms[$term->tid] = $term;
  }
  foreach ($element[$element['#language']]['#options'] as $tid => $name) {
    if ($tid != '_none' && $terms[$tid]->depth > 0) {
      $element[$element['#language']][$tid]['#attributes']['class'][] = 'sba-term-indent';
    }
  }
  return $element;
}
