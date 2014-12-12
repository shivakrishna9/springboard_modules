<?php

/**
 * @file
 * Drush commands for the Fundraiser Sustainer module.
 */

/**
 * Implements hook_drush_help().
 */
function fundraiser_sustainers_drush_help($command) {
  switch ($command) {
    case 'drush:fundraiser-sustainers-process':
      return dt('Processes the sustainer queue.');
  }
}

/**
 * Implements hook_drush_command().
 */
function fundraiser_sustainers_drush_command() {
  $items = array();
  $items['fundraiser-sustainers-process'] = array(
    'description' => dt('Processes the sustainer queue.'),
    'arguments' => array(
      'max_queue' => dt('Max amount of sustainers to attempt to process. If left empty this defaults to the site setting.'),
    ),
    'examples' => array(
      'Standard example' => 'drush fundraiser-sustainers-process',
      'Argument example' => 'drush fundraiser-sustainers-process 2000',
    ),
    'aliases' => array('fsp'),
  );
  return $items;
}

/**
 * Callback function for drush fundraiser-sustainer-process.
 *
 * Processes the Fundraiser Sustainer queue.
 *
 * @param numeric $max_queue
 *   An optional max amount of donations to attempt to process. Defaults to the site setting.
 */
function drush_fundraiser_sustainers_process($max_queue = NULL) {
  if (variable_get('fundraiser_sustainers_cron_type', NULL) !== 'drush') {
    drush_log('The Fundraiser Sustainers cron setting is not enabled for drush. Nothing has been processed.', 'error');
    return;
  }

  if (empty($max_queue)) {
    $max_queue = variable_get('fundraiser_sustainers_max_queue', 1000);
  }

  _fundraiser_sustainers_process_recurring($max_queue);
}
