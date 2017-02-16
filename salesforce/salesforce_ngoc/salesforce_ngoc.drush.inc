<?php
/**
 * @file
 * Defines drush commands for salesforce_ngoc
 */

/**
 * Implements hook_drush_command().
 */
function salesforce_ngoc_drush_command() {

  $items['poll-batch-upload'] = array(
    'description' => 'Poll Salesforce NGOC for status changes to Batch Upload objects',
    'aliases' => array('ngocp'),
  );
  return $items;
}

/**
 * Drush command callback for silpop-send-queued--messages.
 */
function drush_salesforce_ngoc_poll_batch_upload() {
  salesforce_ngoc_poll_batch_upload();
}
