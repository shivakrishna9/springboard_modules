<?php

/**
 * Springboard admin menu.
 * 
 * @param $base_url
 * Menu path of parent menu item (example: 'admin/springboard')
 */
function springboard_admin() {
  
  $main_menu = menu_get_item('admin/springboard');
  $content = system_admin_menu_block((array)$main_menu);
  $output = theme('admin_block_content', $content);
  $output = system_admin_menu_block_page();
  return $output;
}
