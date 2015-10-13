<?php

/**
 * @file
 * Drush commands for Webform User.
 */

/**
 * Implements hook_drush_command().
 */
function webform_user_drush_command() {
  $items = array();

  $items['create-missing-users'] = array(
    'callback' => '_webform_user_create_missing_users',
    'description' => 'Create missing users from webform submissions.',
  );

  return $items;
}

/**
 * Cron callback function.
 */
function _webform_user_create_missing_users() {
  
}
