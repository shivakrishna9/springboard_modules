<?php

/**
 * Springboard admin menu.
 *
 * @param $base_url
 * Menu path of parent menu item (example: 'admin/springboard')
 */
function springboard_admin() {
  drupal_add_css(drupal_get_path('module', 'springboard') . '/css/springboard.css', 'module');

  $main_menu = menu_get_item('admin/springboard');
  if (variable_get('springboard_salesforce_enabled', 0)) {
    $content = drupal_get_form('springboard_welcome');
  }

  $content .= '<div class="dashboard">';

  // We'll need the enabled panes for the dashboard.
  $results = db_query('SELECT * FROM {springboard_panes} WHERE status = 1 order by weight, delta');
  while ($pane = db_fetch_object($results)) {
    $content .= theme($pane->delta);
  }
  $content .= '</div>';


  //$content .= theme('admin_block_content', $content);
  return $content;
}


/**
 * Admin settings form.
 */
function springboard_admin_settings() {
  // TODO: add support for pane weights.

  $results = db_query('SELECT delta, status FROM {springboard_panes}');
  while ($item = db_fetch_object($results)) {
    $panes[$item->delta] = $item->delta;

    if ($item->status) {
      $enabled_panes[$item->delta] = $item->delta;
    }
    else {
      $enabled_panes[$item->delta] = 0;
    }


  }


  $form = array();
  $form['panes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Panes'),
    '#description' => t('Enable/disable visible panes on the Springboard Dashboard'),
  );
  $form['panes']['springboard_enabled_panes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled Panes'),
    '#options' => $panes,
    '#default_value' => $enabled_panes,
  );
  $form['dashboard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Threshold Settings'),
    '#description' => t('Configure display settings for your dashboard.'),
  );
  $form['dashboard']['springboard_conversion_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Conversion Percentage Threshold'),
    '#description' => t('Set the minimum acceptable conversion rate percentage. Any forms with lower conversion rates will be flagged on the dashboard.'),
    '#default_value' => variable_get('springboard_conversion_threshold', 0),
  );
  $form['dashboard']['springboard_local_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Local Failure Threshold'),
    '#description' => t('Set the maximum acceptable local failure percentage. Any forms with higher local failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_local_failure_threshold', 0),
  );
  $form['dashboard']['springboard_gateway_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway Failure Percentage Threshold'),
    '#description' => t('Set the maximum acceptable gateway failure percentage. Any forms with higher gateway failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_gateway_failure_threshold', 0),
  );
  $form['dashboard']['springboard_load_time_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Load Time Threshold'),
    '#description' => t('Set the maximum acceptable time (in ms) to render a donation form. Any forms taking longer than this to load will be flagged.'),
    '#default_value' => variable_get('springboard_load_time_threshold', 500),
  );

  $form['#validate'] = array('springboard_admin_settings_validate');
  $form['#submit'][] = 'springboard_admin_settings_submit';
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

function springboard_admin_settings_submit($form, $form_state) {

  foreach($form_state['values']['springboard_enabled_panes'] as $delta => $status) {
    if ($status) {
      $status = 1;
    }
    db_query("UPDATE {springboard_panes} SET status=%d WHERE delta = '%s'", $status, $delta);
  }
}

/**
 * Page callback for springboard help.
 */
function springboard_help_page() {
  return theme_springboard_help();
}