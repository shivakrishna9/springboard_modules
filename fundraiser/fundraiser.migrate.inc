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
        'title' => t('Fundraiser Example Sustainer Migration'),
      ),
      'irusa' => array(
        'title' => t('IRUSA Sustainer Migration'),
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
      'IrusaDonation' => array(
        'class_name' => 'IrusaDonationMigration',
        'group_name' => 'irusa',
        'dependencies' => array(
          'IrusaDonor',
        ),
      ),
      'IrusaDonor' => array(
        'class_name' => 'IrusaDonorMigration',
        'group_name' => 'irusa',
      ),
    ),
  );

  return $api;
}