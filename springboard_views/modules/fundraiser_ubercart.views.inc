<?php
/**
 * @file
 * Provides views table data for fundraiser related tables.
 */

/**
 * Implements hook_views_data().
 */
function springboard_views_fundraiser_views_data() {
  $data = array();
  $data['uc_orders']['table'] = array(
    'group' => t('Fundraiser donation'),
    'join' => array(
      'fundraiser_donation' => array(
        'left_field' => 'order_id',
        'field' => 'did',
      ),
    ),
  );
  $data['uc_orders']['order_status'] = array(
    'title' => t('Donation status'),
    'help' => t('The current status of a donation.'),
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
  $data['uc_orders']['order_total'] = array(
    'title' => t('Donation amount'),
    'help' => t('The amount of a donation.'),
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
  $data['uc_orders']['primary_email'] = array(
    'title' => t('Donation email'),
    'help' => t('The associated email for a donation.'),
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
  $data['uc_orders']['billing_first_name'] = array(
    'title' => t('Billing first name'),
    'help' => t('The first name of billing information of a donation.'),
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
  $data['uc_orders']['billing_last_name'] = array(
    'title' => t('Billing last name'),
    'help' => t('The last name of billing information of a donation.'),
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
  $data['uc_orders']['billing_street1'] = array(
    'title' => t('Billing street 1'),
    'help' => t('The street 1 of billing information of a donation.'),
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
  $data['uc_orders']['billing_street2'] = array(
    'title' => t('Billing street 2'),
    'help' => t('The street 2 of billing information of a donation.'),
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
  $data['uc_orders']['billing_city'] = array(
    'title' => t('Billing city'),
    'help' => t('The city of billing information of a donation.'),
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
  $data['uc_orders']['billing_country'] = array(
    'title' => t('Billing country'),
    'help' => t('The country of billing information of a donation.'),
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
  $data['uc_orders']['billing_zone'] = array(
    'title' => t('Billing zone'),
    'help' => t('The zone of billing information of a donation.'),
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
/*
  $data['uc_orders']['created'] = array(
    'title' => t('Created date'),
    'help' => t('The created date of a donation.'),
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
  $data['uc_orders']['currency'] = array(
    'title' => t('Currency'),
    'help' => t('The currency type of a donation.'),
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
  return $data;
}
