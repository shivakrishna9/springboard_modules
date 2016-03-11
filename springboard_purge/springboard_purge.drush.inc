<?php

function springboard_purge_drush_command() {
  $items  = array();
  $items['springboard-purge-users'] = array(
    'callback'    => 'springboard_purge_users',
    'description' => dt('Delete old inactive users'),
    'aliases' => array('sbpu'),
    'options'   => array(
      'active-user-days' => "The number of days the last login day should be less than. Defaults to 90.",
      'batch-size' => "The number of inactive users to delete.",
      // @TODO custom_roles.
    ),
  );
  $items['springboard-purge-webforms'] = array(
    'callback'    => 'springboard_purge_webform_submissions',
    'description' => dt('Delete old webform submissions'),
    'aliases' => array('sbpw'),
    'options' => array(
      'days-old' => "The number of days old the submission should be to be deleted. Defaults to 90.",
      'batch-size' => "The number of webform submissions to delete.",
    ),
  );
  $items['springboard-purge-orders'] = array(
    'callback' => 'springboard_purge_orders',
    'description' => dt('Delete old orders'),
    'aliases' => array('sbpo'),
    'options' => array(
      'days-old' => "The number of days old the order should be to be deleted. Defaults to 90.",
      'batch-size' => "The number of orders to delete.",
    ),
  );

  return $items;
}

/**
 * Drush command callback.
 */
function springboard_purge_users($active_user_days = 90, $batch_size = 250, $custom_roles = array()) {
  $users_to_keep = springboard_purge_get_user_whitelist($active_user_days, $custom_roles);
  $count_of_active_users = count($users_to_keep);
  drush_print("Identified $count_of_active_users active users.");

  // Perform the delete.
  springboard_purge_delete_users($users_to_keep, $batch_size);
}

/**
 * Drush command callback.
 */
function springboard_purge_webform_submissions($days_old = 90, $batch_size = 250) {
  $submissions_to_keep = springboard_purge_get_submissions_with_active_order($days_old);
  $submissions_to_delete = springboard_purge_get_old_submisions($days_old, $batch_size, $submissions_to_keep);

  // Perform the delete.
  springboard_purge_delete_webform_submissions($submissions_to_delete);

  springboard_purge_check_webform_confirmations_submissions_purge_enabled();
}

/**
 * Drush command callback.
 */
function springboard_purge_orders($days_old = 90, $batch_size = 10) {
  // Do not delete donations that are part of an active series.
  $donations_to_keep = springboard_purge_get_donations_in_nonexpired_series();
  $commerce_orders_to_delete = springboard_purge_get_old_commerce_orders($days_old, $batch_size, $donations_to_keep);

  // Perform the delete.
  springboard_purge_delete_commerce_orders($commerce_orders_to_delete);
}

