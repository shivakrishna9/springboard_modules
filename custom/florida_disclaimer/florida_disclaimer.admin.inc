<?php
/**
 * @file
 * Admin settings forms.
 */

/**
 * Admin settings form.
 */
function florida_disclaimer_admin() {
  $form = array();
  $form['florida_disclaimer_text'] = array(
  '#type' => 'textarea',
  '#title' => t('Text to show below submit button on donation forms.'),
  '#description' => t('Used to supply a disclaimer below the submit button on donation forms when a donor selects Florida OR the form is affiliated to Florida.'),
  '#default_value' => variable_get('florida_disclaimer_text', ''),
  );

  return system_settings_form($form);
}
