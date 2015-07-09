<?php

/**
 * @file
 * Add views integration for page wrapper data.
 */

/**
 * Implements hook_views_data().
 */
function custom_demo_views_data() {
  $data = array();

  // Expose the page wrapper tables to views.
  $data['node_page_wrappers']['table'] = array(
    'group' => t('Page wrappers'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
    'base' => array(
      'field' => 'nid',
      'title' => t('Assigned Page Wrappers'), 
      'weight' => -10,
    ),
  );

  $data['node_page_wrappers']['wrapper_nid'] = array(
    'field' => array(
      'handler' => 'views_handler_field',
      'title' => t('Assigned Page Wrapper'),
      'help' => t('Display a link to the assigned page wrappers. If you need more fields than the nid add the content: Page Wrapper relationship.'),
      'handler' => 'views_handler_field_node',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('Page wrapper'),
      'group' => t('Content'),
      'title' => t('Assigned Page Wrapper'),
      'help' => t('A bridge to the Page Wrapper node that is assigned'),
    ),
  );

  return $data;
}
