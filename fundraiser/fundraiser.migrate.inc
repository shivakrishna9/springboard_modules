<?php

/**
 * @file
 *
 * Contains the functions for Drupal Migrate integration.
 */

/**
 * Implements hook_migrate_api().
 */
function fundraiser_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'FundraiserExampleDonation' => array('class_name' => 'FundraiserExampleDonation'),
      'FundraiserExampleDonor' => array('class_name' => 'FundraiserExampleDonor'),
    ),
  );

  return $api;
}