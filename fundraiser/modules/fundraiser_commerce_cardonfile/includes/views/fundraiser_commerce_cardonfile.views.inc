<?php

/**
 * @file
 * Views integration for Fundraiser Commerce Card on File.
 */

/**
 * Implements hook_views_data_alter().
 */
function fundraiser_commerce_cardonfile_views_data_alter(&$data) {
  $data['commerce_cardonfile']['used_by'] = array(
    'title' => t('Used by'),
    'help' => t('What this card is used for, i.e. quick donations or recurring donations.'),
    'field' => array(
      'title' => t('Used By'),
      'help' => t('Display which fundraiser modules use this card.'),
      'handler' => 'fundraiser_commerce_cardonfile_handler_field_used_by',
    ),
  );

  return $data;
}
