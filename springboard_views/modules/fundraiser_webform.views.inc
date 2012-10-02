<?php
/**
 * @file
 * Provides views table data for fundraiser related tables.
 */

/**
 * Implements hook_views_data().
 */
function springboard_views_fundraiser_webform_views_data() {
  $data = array();
  $data['webform_submissions']['table'] = array(
    'group' => t('Fundraiser donation'),
    'join' => array(
      'fundraiser_donation' => array(
        'left_field' => 'sid',
        'field' => 'sid',
      ),
    ),
  );
  $data['webform_submissions']['uid'] = array(
    'title' => t('Submitting user'),
    'help' => t('Submitting user related to a donation.'),
    'relationship' => array(
      'base' => 'user',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Submission entry'),
    ),
  );
/*
  $data['webform_submissions']['submitted'] = array(
    'title' => t('Submitted date'),
    'help' => t('The submitted date of a donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
*/
  return $data;
}
