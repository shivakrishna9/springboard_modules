<?php
/**
 * @file
 * Fundraiser-specific hooks for Fundraiser Commerce Cardonfile.
 */

/**
 * Implements hook_fundraiser_donation_record_create_alter().
 */
function fundraiser_commerce_cardonfile_fundraiser_donation_record_create_alter($donation, &$donation_record) {
  fundraiser_commerce_cardonfile_fundraiser_donation_record_update_alter($donation, $donation_record);
}

/**
 * Implements hook_fundraiser_donation_record_update_alter().
 */
function fundraiser_commerce_cardonfile_fundraiser_donation_record_update_alter($donation, &$donation_record) {
  if (!empty($donation->data['cardonfile'])) {
    $donation->data['cardonfile'];
  }

  dpm($donation);
  dpm($donation_record);
}
