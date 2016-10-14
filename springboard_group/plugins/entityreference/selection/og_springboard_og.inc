<?php
/**
 * @file
 * Tells ctools about our plugin..
 */

require_once drupal_get_path('module', 'entityreference') . '/plugins/selection/EntityReference_SelectionHandler_Generic.class.php';

$plugin = array(
  'title' => t('Springboard Groups'),
  'class' => 'OgSpringboardOgSelectionHandler',
);
