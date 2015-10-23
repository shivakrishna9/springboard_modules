<?php
/**
 * @file
 * Drush commands for Springboard.
 */

/**
 * Implements hook_drush_command().
 */
function springboard_drush_command() {
  $items = array();

  $items['set-springboard-client-id'] = array(
    'callback' => 'springboard_set_client_id',
    'description' => 'Sets the client ID of this Springboard instance.',
  );

  return $items;
}

function springboard_set_client_id($client_id) {
  variable_set('springboard_client_id', $client_id);
}
