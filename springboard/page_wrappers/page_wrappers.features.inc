<?php

/**
 * Implementation of hook_node_info().
 */
function page_wrappers_node_info() {
  $items = array(
    'page_wrapper' => array(
      'name' => t('Page Wrapper'),
      'module' => 'features',
      'description' => t('Allows a user to define custom page templates that can then be assigned to specific nodes.'),
      'has_title' => '1',
      'title_label' => t('Internal Title'),
      'has_body' => '1',
      'body_label' => t('Template HTML'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function page_wrappers_views_api() {
  return array(
    'api' => '2',
  );
}
