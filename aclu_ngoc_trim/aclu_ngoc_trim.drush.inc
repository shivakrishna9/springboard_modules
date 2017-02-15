<?php

/**
 * Implements hook_drush_command().
 */
function aclu_ngoc_trim_drush_command() {

  $items  = array();
  $items['ngoc-synced-user-trim'] = array(
    'callback' => '_aclu_ngoc_trim_drush_process',
    'description' => dt('Deletes user profile field data and revisions for succesfully synced Salesforce contacts'),
    'aliases' => array('upt'),
    //'options' => array(
    //  'batch-size' => 'The number of users to process in one run. Defaults to ' . NGOC_TRIM_MAX_ITEMS,
    //),
  );
  return $items;
}

/**
 * Command callback for drush ngoc-synced-user-trim command.
 */
function _aclu_ngoc_trim_drush_process() {
  _aclu_ngoc_trim_process();
}
