<?php

/**
 * @file
 * Menu functions for Springboard data warehouse.
 */

/**
 * Implements hook_menu().
 */
function springboard_dw_menu()
{
  $items['admin/config/services/springboard-dw'] = array(
    'title'            => 'Springboard Data Warehouse',
    'description'      => 'Configuration settings for Springboard Data Warehousing.',
    'page callback'    => 'springboard_dw_settings_page',
    'access arguments' => array('administer springboard data warehouse'),
    'type'             => MENU_NORMAL_ITEM,
    'file'             => 'includes/springboard_dw.admin.inc',
  );

  return $items;
}