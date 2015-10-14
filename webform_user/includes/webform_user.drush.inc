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

  $items['webform-create-missing-users'] = array(
    'callback' => '_webform_user_create_missing_users',
    'description' => 'Create missing users from webform submissions.',
  );

  return $items;
}

/**
 * Cron callback function. Creates missing users from webform submissions.
 */
function _webform_user_create_missing_users($node_id) {
  module_load_include('inc', 'webform', 'includes/webform.submissions');

  // Create a log table.
  if (!db_table_exists('webform_user_create_missing_users_log')) {
    $schema = array(
      'fields' => array(
        'nid' => array(
          'description' => 'The node ID of the webform.',
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'default' => 0,
        ),
        'uid' => array(
          'description' => 'The ID of the user created.',
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'default' => 0,
        ),
        'timestamp' => array(
          'description' => 'The timestamp when the user was created.',
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'default' => 0,
        ),
      ),
      'primary key' => array('nid', 'uid'),
    );

    db_create_table('webform_user_create_missing_users_log', $schema);
  }

  // Get all webform submissions with an empty user ID for the given node ID.
  $query = 'SELECT `sd`.`sid`, `cm`.`map_id`, `sd`.`data`
    FROM `webform_submissions` `s`
    RIGHT JOIN `webform_submitted_data` `sd` ON `sd`.`sid` = `s`.`sid`
    RIGHT JOIN `webform_user_component_map` `cm` ON `cm`.`nid` = `sd`.`nid` AND `cm`.`cid` = `sd`.`cid`
    WHERE `s`.`nid` = :nid
    AND `s`.`uid` = :uid
    ORDER BY `s`.`sid` ASC;';

  $result = db_query($query, array(
    ':nid' => $node_id,
    ':uid' => 0,
  ));

  $submissions = array();

  foreach ($result as $record) {
    if (!isset($submissions[$record->sid])) {
      $submissions[$record->sid] = array();
    }

    $submissions[$record->sid][$record->map_id] = $record->data;
  }

  // Create users from the webform submissions.
  foreach ($submissions as $sid => $submission) {
    $account = user_load_by_mail($submission['mail']);

    if ($account == NULL) {
      $user_fields = array(
        'name' => $submission['mail'],
        'mail' => $submission['mail'],
        'init' => $submission['mail'],
        'pass' => user_password(8),
        'status' => 1,
      );

      $account = user_save('', $user_fields);

      _webform_user_save_profile_map($account->uid, $submission);

      $submission = webform_get_submission($node_id, $sid);
      $submission->uid = $account->uid;
      // Skip the current user check; this is an offline user creation.
      $submission->webform_user_skip_user_check = TRUE;

      $node = node_load($node_id);

      // Update the webform submission with the new user ID.
      webform_submission_update($node, $submission);

      // Log the new user creation.
      db_insert('webform_user_create_missing_users_log')->fields(array(
        'nid' => $node_id,
        'uid' => $account->uid,
        'timestamp' => time(),
      ))->execute();
    }
  }
}
