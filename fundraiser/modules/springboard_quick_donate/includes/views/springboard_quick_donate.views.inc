<?php

/**
 * @file
 * Views-related hooks for Springboard Quick Donate.
 */

/**
 * Implements hook_views_data().
 */
function springboard_quick_donate_views_data() {
  $data['fundraiser_springboard_quick_donate_cardonfile']['table'] = array(
    'group' => t('Springboard Quick Donations'),
    'base' => array(
      'title' => t('Springboard Quck Donation Opt In'),
      'help' => t('Tracks users opted into quick donations.'),
    ),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
      'users' => array(
        'left_field' => 'uid',
        'field' => 'uid',
      ),
      'commerce_cardonfile' => array(
        'left_field' => 'card_id',
        'field' => 'card_id',
      ),
      'fundraiser_donation' => array(
        'left_field' => 'did',
        'field' => 'did',
      ),
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['nid'] = array(
    'title' => t('Fundraiser node'),
    'help' => t('Fundraiser form node id.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Fundraiser node'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['uid'] = array(
    'title' => t('Quick donation opt-in user'),
    'help' => t('User associated with quick donation opt-in record.'),
    'relationship' => array(
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in user'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['status'] = array(
    'title' => t('Quick donation opt-in status'),
    'help' => t('Whether the user is actively enrolled in quick donations.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['card_id'] = array(
    'title' => t('Quick donation opt-in card ID'),
    'help' => t('The card ID associated with the quick donation.'),
    'relationship' => array(
      'base' => 'commerce_cardonfile',
      'base field' => 'card_id',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in card ID'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['did'] = array(
    'title' => t('Quick donation opt-in donation ID'),
    'help' => t('The donation ID associated with the quick donation.'),
    'relationship' => array(
      'base' => 'fundraiser_donation',
      'base field' => 'did',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in donation ID'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['optin_date'] = array(
    'title' => t('Quick donation opt-in time'),
    'help' => t('Timestamp of when user opted into quick donations.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  $data['fundraiser_springboard_quick_donate_cardonfile']['ms'] = array(
    'title' => t('Quick donation market source'),
    'help' => t('Market source value for the quick donation record.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
