<?php

/**
 * @file
 */

/**
 * Implements hook_views_data().
 */
function springboard_petition_quicksign_views_data() {
  $data['springboard_quicksign_submissions']['table'] = array(
    'group' => t('Springboard Petition Quicksign'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
      'webform_submissions' => array(
        'left_field' => 'sid',
        'field' => 'sid',
      ),
    ),
  );
  $data['springboard_quicksign_submissions']['nid'] = array(
    'title' => t('Petition node'),
    'help' => t('Petition form node id.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Petition node'),
    ),
  );
  $data['springboard_quicksign_submissions']['sid'] = array(
    'title' => t('Webform submission'),
    'help' => t('Webform submission id.'),
    'relationship' => array(
      'base' => 'webform_submissions',
      'base field' => 'sid',
      'handler' => 'views_handler_relationship',
      'label' => t('Webform submission'),
    ),
  );
  return $data;
}
