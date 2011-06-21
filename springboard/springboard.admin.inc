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
