<?php
/**
 * @file
 * Provides Springboard administrative forms.
 */

/**
 * Admin settings form.
 */
function springboard_admin_settings() {
  // TODO: add support for pane weights.
  $panes = module_invoke_all('springboard_dashboard_panes');
  drupal_alter('springboard_dashboard_panes', $panes);
  foreach ($panes as $name => $info) {
    $panes[$name] = $info['label'];
  }
  $form = array();
  $form['panes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Panes'),
  );
  $form['panes']['springboard_enabled_panes'] = array(
    '#description' => t('Enable/disable visible panes on the Springboard Dashboard'),
    '#type' => 'checkboxes',
    '#options' => $panes,
    '#default_value' => variable_get('springboard_enabled_panes', array()),
  );
  $form['dashboard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Threshold Settings'),
    '#description' => t('Configure display settings for your dashboard.'),
  );
  $form['dashboard']['springboard_conversion_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Conversion Percentage Threshold'),
    '#description' => t('Set the minimum acceptable conversion rate percentage. Any forms with lower ' .
      'conversion rates will be flagged on the dashboard.'),
    '#default_value' => variable_get('springboard_conversion_threshold', 0),
  );
  $form['dashboard']['springboard_local_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Local Failure Threshold'),
    '#description' => t('Set the maximum acceptable local failure percentage. Any forms with higher ' .
      'local failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_local_failure_threshold', 0),
  );
  $form['dashboard']['springboard_gateway_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway Failure Percentage Threshold'),
    '#description' => t('Set the maximum acceptable gateway failure percentage. Any forms with higher ' .
      'gateway failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_gateway_failure_threshold', 0),
  );
  $form['dashboard']['springboard_load_time_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Load Time Threshold'),
    '#description' => t('Set the maximum acceptable time (in ms) to render a donation form. Any forms ' .
      'taking longer than this to load will be flagged.'),
    '#default_value' => variable_get('springboard_load_time_threshold', 500),
  );
  $form['#validate'][] = 'springboard_admin_settings_validate';
  return system_settings_form($form);
}

/**
 * Validation handler for the springboard admin settings form.
 */
function springboard_admin_settings_validate(&$form, $form_state) {
  if ($form_state['values']['springboard_conversion_threshold'] && !is_numeric($form_state['values']['springboard_conversion_threshold'])) {
    form_set_error('springboard_conversion_threshold', t('The conversion threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_local_failure_threshold'] && !is_numeric($form_state['values']['springboard_local_failure_threshold'])) {
    form_set_error('springboard_local_failure_threshold', t('The local failure threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_gateway_failure_threshold'] && !is_numeric($form_state['values']['springboard_gateway_failure_threshold'])) {
    form_set_error('springboard_gateway_failure_threshold', t('The gateway failure threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_load_time_threshold'] && !is_numeric($form_state['values']['springboard_load_time_threshold'])) {
    form_set_error('springboard_load_time_threshold', t('The load time threshold must be numeric!'));
  }
}