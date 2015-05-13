<?php

$placements = _springboard_tag_placement_options();
$plugin = array(
  'schema' => 'springboard_tag',
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
    'tag' => '',
    'weight' => 0,
    'settings' => array(
      'placement' => key($placements),
      'visibility' => array(
        'path' => array(
          'page_specific' => BLOCK_VISIBILITY_NOTLISTED,
          'pages' => '',
        ),
        'node' => array(
          'type' => array(),
        ),
        'fundraiser' => array(
          'confirmation' => 0,
        ),
        'user' => array(
          'roles' => array(),
        ),
      ),
    ),
  ),

  'handler' => 'springboard_tag_export_ui',
);
