<?php
/**
 * @file
 * Drush commands for the fundraiser_commerce_cardonfile module.
 */

const FUNDRAISER_COMMERCE_CARDONFILE_DRUSH_DESCRIPTION = 'Processes cardonfile cards to set their appropriate adress profile_id values.';

/**
 * Implements hook_drush_help().
 */
function fundraiser_commerce_cardonfile_drush_help($command) {
  switch ($command) {
    case 'drush:fundraiser-commerce-cardonfile-process':
      return dt(FUNDRAISER_COMMERCE_CARDONFILE_DRUSH_DESCRIPTION);
  }
}

/**
 * Implements hook_drush_command().
 */
function fundraiser_commerce_cardonfile_drush_command() {
  $items = array();
  $items['fundraiser-commerce-cardonfile-process'] = array(
    'description' => FUNDRAISER_COMMERCE_CARDONFILE_DRUSH_DESCRIPTION,
    'options' => array(
      'limit' => 'The maximum number of cards to process.',
    ),
    'examples' => array(
      'Standard example' => 'fundraiser-commerce-cardonfile-process',
    ),
    'aliases' => array('fccp'),
  );

  return $items;
}

/**
 * Drush command to process recurring donations.
 */
function drush_fundraiser_commerce_cardonfile_process() {
  $limit = drush_get_option('limit', 200);

  module_load_include('inc', 'fundraiser_commerce_cardonfile', 'includes/fundraiser_commerce_cardonfile.admin');
  $context = _fundraiser_commerce_cardonfile_update_card_addresses($limit);

  if (!empty($context['progress']) && !empty($context['max'])) {
    return 'Processed ' . $context['progress'] . ' of ' . $context['max'] . ' cards.';
  }
  else {
    return 'No cards needed processing.';
  }
}
