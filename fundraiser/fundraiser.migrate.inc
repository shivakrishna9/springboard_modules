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
    'groups' => array(
      'fundraiser_example' => array(
        'title' => t('Fundraiser Example'),
      ),
      'fundraiser_luminate' => array(
        'title' => t('Fundraiser Luminate'),
      ),
    ),
    'migrations' => array(
      'FundraiserExampleDonation' => array(
        'class_name' => 'FundraiserExampleDonation',
        'group_name' => 'fundraiser_example',
      ),
      'FundraiserExampleDonor' => array(
        'class_name' => 'FundraiserExampleDonor',
        'group_name' => 'fundraiser_example',
      ),
      'FundraiserLuminateDonation' => array(
        'class_name' => 'FundraiserLuminateDonation',
        'group_name' => 'fundraiser_luminate',
      ),
      'FundraiserLuminateDonor' => array(
        'class_name' => 'FundraiserLuminateDonor',
        'group_name' => 'fundraiser_luminate',
      ),
    ),
  );

  return $api;
}
