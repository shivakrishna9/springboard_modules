<?php
/**
 * @file
 * springboard_p2p_donation_form.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function springboard_p2p_donation_form_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function springboard_p2p_donation_form_node_info() {
  $items = array(
    'p2p_donation_form' => array(
      'name' => t('Peer to peer donation form'),
      'base' => 'node_content',
      'description' => t('A donation form specifically for P2P.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
