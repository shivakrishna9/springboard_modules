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
 * Cron callback function. Creates missing users from webform submissions.
 */
function _webform_user_create_missing_users() {
  $query = 'SELECT `sd`.`sid`, `cm`.`map_id`, `sd`.`data`
    FROM `webform_submissions` `s`
    JOIN `webform_submitted_data` `sd` ON `sd`.`sid` = `s`.`sid`
    JOIN `webform_user_component_map` `cm` ON `cm`.`nid` = `sd`.`nid` AND `cm`.`cid` = `sd`.`cid`
    WHERE `s`.`uid` = :uid
    ORDER BY `s`.`sid` ASC;';

  $result = db_query($query, array(':uid' => 0));

  $submissions = array();

  foreach ($result as $record) {
    if (!isset($submissions[$record->sid])) {
      $submissions[$record->sid] = array();
    }

    $submissions[$record->sid][$record->map_id] = $record->data;
  }
}
