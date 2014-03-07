<?php
/**
 * @file
 * springboard_p2p.features.inc
 */

/**
 * Implements hook_node_info().
 */
function springboard_p2p_node_info() {
  $items = array(
    'p2p_category' => array(
      'name' => t('Peer to peer category'),
      'base' => 'node_content',
      'description' => t('A peer to peer category.'),
      'has_title' => '1',
      'title_label' => t('Name'),
      'help' => t('Peer to peer category submission guidelines.'),
    ),
  );
  return $items;
}
