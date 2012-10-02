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
  $data['fundraiser_donation']['sid'] = array(
    'title' => t('Donation submission'),
    'help' => t('Webform submission related to a donation.'),
    'relationship' => array(
      'base' => 'webform_submissions',
      'base field' => 'sid',
      'handler' => 'views_handler_relationship',
      'label' => t('Donation submission'),
    ),
  );
  /**
   * Webform submission tables.
   */
  $data['webform_submissions']['table'] = array(
    'group' => t('Webform submission'),
    'base' => array(
      'field' => 'sid',
      'title' => t('Webform submissions'),
      'help' => t('Webform submissions made.'),
    ),
  );
  $data['webform_submissions']['uid'] = array(
    'title' => t('Submitting user'),
    'help' => t('Submitting user related to a donation.'),
    'relationship' => array(
      'base' => 'user',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Submitting user'),
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
