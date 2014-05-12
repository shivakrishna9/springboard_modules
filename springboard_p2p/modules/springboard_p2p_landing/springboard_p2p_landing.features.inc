<?php
/**
 * @file
 * springboard_p2p_landing.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function springboard_p2p_landing_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "panelizer" && $api == "panelizer") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function springboard_p2p_landing_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function springboard_p2p_landing_node_info() {
  $items = array(
    'p2p_campaign_landing' => array(
      'name' => t('Peer to Peer Campaign Landing'),
      'base' => 'node_content',
      'description' => t('Peer to Peer landing page content type that will aggregate and pull in data from various areas of the site.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
