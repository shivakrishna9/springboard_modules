<?php
/**
 * @file
 * Defines drush command for purging old data.
 */

define(SPRINGBOARD_PURGE_DEFAULT_DAYS_OLD, 90);
define(SPRINGBOARD_PURGE_DEFAULT_BATCH_SIZE, 250);

/**
 * Implements hook_drush_command().
 */
function springboard_purge_drush_command() {
  $items  = array();
  $items['springboard-purge'] = array(
    'callback'    => 'springboard_purge',
    'description' => dt('Delete old records to reduce database size, oldest first.'),
    'aliases' => array('sbp'),
    'arguments' => array(
      'type' => 'The type of record to delete. (users, webforms, or orders)',
    ),
    'options'   => array(
      'days-old' => 'The number of days old the record should be to be considered for deletion. Defaults to ' . SPRINGBOARD_PURGE_DEFAULT_DAYS_OLD,
      'batch-size' => 'The number of records to delete. Defaults to ' . SPRINGBOARD_PURGE_DEFAULT_BATCH_SIZE,
      // @TODO custom-roles.
    ),
    'examples' => array(
      'drush sbp user' => 'Deletes ' . SPRINGBOARD_PURGE_DEFAULT_BATCH_SIZE . ' users that have not logged in for at least ' . SPRINGBOARD_PURGE_DEFAULT_DAYS_OLD . ' days.',
      'drush sbp webforms --days-old=100 --batch-size=500' => 'Deletes 500 webform submissions at least 100 days old.',
    ),
  );
  return $items;
}

/**
 * Drush command callback.
 */
function springboard_purge($type = NULL) {
  // Check for existence of argument.
  if (!$type) {
    $options = array(
      'users' => dt('Users'),
      'webforms' => dt('Webforms'),
      'orders' => dt('Orders'),
    );
    $type = drush_choice($options, dt('What kind of records do you want to purge?'));
  }
  // Get arguments.
  $days_old = drush_get_option('days-old', SPRINGBOARD_PURGE_DEFAULT_DAYS_OLD);
  $batch_size = drush_get_option('batch-size', SPRINGBOARD_PURGE_DEFAULT_BATCH_SIZE);
  $custom_roles = drush_get_option('custom-roles');

  switch ($type) {
    case 'users':
      $users_to_keep = springboard_purge_get_user_whitelist($days_old, $custom_roles);
      $count_of_active_users = count($users_to_keep);
      drush_print("Identified $count_of_active_users active users.");
      // Perform the delete.
      springboard_purge_delete_users($users_to_keep, $batch_size);
      break;

    case 'webforms':
      $submissions_to_keep = springboard_purge_get_submissions_with_active_order($days_old);
      $submissions_to_delete = springboard_purge_get_old_submisions($days_old, $batch_size, $submissions_to_keep);
      // Perform the delete.
      springboard_purge_delete_webform_submissions($submissions_to_delete);
      springboard_purge_check_webform_confirmations_submissions_purge_enabled();
      break;

    case 'orders':
      // Do not delete donations that are part of an active series.
      $donations_to_keep = springboard_purge_get_donations_in_nonexpired_series();
      $commerce_orders_to_delete = springboard_purge_get_old_commerce_orders($days_old, $batch_size, $donations_to_keep);

      // Perform the delete.
      springboard_purge_delete_commerce_orders($commerce_orders_to_delete);
      break;

    default:
  }
}
