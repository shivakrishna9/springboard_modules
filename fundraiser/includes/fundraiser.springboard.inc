<?php
/**
 * @file
 * Springboard hooks for Fundraiser.
 */

/**
 * Implements hook_springboard_admin_alias_patterns().
 */
function fundraiser_springboard_admin_alias_patterns() {
  return array(
    'admin/commerce/config/currency' => array(
      'path' => array(
        'regex' => '|^/admin/commerce/config/currency|',
        'replacement' => 'admin/commerce/config/currency',
      ),
      'alias' => array(
        'regex' => '|^springboard/settings/config/currency|',
        'replacement' => 'springboard/settings/config/currency',
      ),
    ),
  );
}

/**
 * Implements hook_springboard_admin_admin_menu_items_alter().
 */
function fundraiser_springboard_admin_admin_menu_items_alter(&$items) {
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/commerce/config/currency'] = array(
    'link_path' => 'admin/commerce/config/currency',
    'link_title' => 'Manage Currencies',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );
}
