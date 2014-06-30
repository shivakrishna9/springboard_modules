<?php
/**
 * @file
 * springboard_advocacy.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function springboard_petition_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function springboard_petition_node_info() {
  $items = array(
    'springboard_petition' => array(
      'name' => t('Petition'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
