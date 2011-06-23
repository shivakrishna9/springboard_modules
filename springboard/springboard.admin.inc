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

  $content .= theme('admin_block_content', $content);
  // We'll need the enabled panes for the dashboard.
  $results = db_query('SELECT * FROM {springboard_panes} WHERE status = 1 order by weight, delta');
  while ($pane = db_fetch_object($results)) {

    $content .= theme($pane->delta);
  }
  if (variable_get('springboard_config_homepage', 0)) {
   // $content .= drupal_get_form('springboard_welcome');
  }
 // $content .= system_admin_menu_block_page();

  return $content;
}


/**
 * Admin settings form.
 */
function springboard_admin_settings() {

  $form = array();
  $form['dashboard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Settings'),
    '#description' => t('Configure display settings for your dashboard.'),
    '#collapsible' => TRUE,
  );
  $form['dashboard']['springboard_load_time_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation Form Load Time Threshold'),
    '#description' => t('Set the maximum acceptable time (in ms) to render a donation form. Any forms taking longer than this to load will be flagged on the dashboard page.'),
    '#default_value' => variable_get('springboard_load_time_threshold', 0),
  );
  $form['#validate'] = array('springboard_admin_settings_validate');
  return system_settings_form($form);

}

/**
 * Validation handler for the springboard admin settings form.
 */
function springboard_admin_settings_validate(&$form, $form_state) {
  if ($form_state['values']['springboard_load_time_threshold'] && !is_numeric($form_state['values']['springboard_load_time_threshold'])) {
    form_set_error('springboard_load_time_threshold', t('The load time threshold must be numeric!'));
  }
}

