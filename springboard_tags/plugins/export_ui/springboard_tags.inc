<?php

$placements = _springboard_tags_placement_options();
$plugin = array(
  'schema' => 'springboard_tags',
  'access' => 'administer springboard tags',

  'menu' => array(
    'menu prefix' => 'admin/springboard',
    'menu item' => 'springboard-tags',
    'menu title' => 'Springboard Tags',
    'menu description' => 'Add, edit or delete tags for use with third party tracking systems.',
  ),

  'title singular' => t('tag'),
  'title singular proper' => t('Tag'),
  'title plural' => t('tags'),
  'title plural proper' => t('Tags'),

  'defaults' => array(
    'settings' => array(
      'placement' => key($placements),
      'page_visibility' => BLOCK_VISIBILITY_NOTLISTED,
      'pages' => '',
      'node_type' => array(),
    ),
  ),

  'handler' => 'springboard_tags_export_ui',
);
