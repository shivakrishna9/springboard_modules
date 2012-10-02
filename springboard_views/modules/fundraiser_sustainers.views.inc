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
  $data['fundraiser_sustainers']['table'] = array(
    'group' => t('Fundraiser recurring donations'),
    'join' => array(
      'fundraiser_donation' => array(
        'left_field' => 'did',
        'field' => 'did',
      ),
    ),
  );
  $data['fundraiser_sustainers']['master_did'] = array(
    'title' => t('Master donation'),
    'help' => t('Master donation for this series.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_sustainers']['did'] = array(
    'title' => t('Recurring donation'),
    'help' => t('A single recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Order is recurring'),
      'accept null' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_sustainers']['next_charge'] = array(
    'title' => t('Date of next charge'),
    'help' => t('Date of the next recurring charge.'),
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
  $data['fundraiser_sustainers']['gateway_resp'] = array(
    'title' => t('Gateway response'),
    'help' => t('Response from the payment gateway for a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
      'options callback' => 'springboard_views_gateway_response_options',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_sustainers']['attempts'] = array(
    'title' => t('Attempts'),
    'help' => t('Number of attempted charges for a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_sustainers']['cancellation_reason'] = array(
    'title' => t('Cancellation reason'),
    'help' => t('The reason provided for the cancellation of a recurring donation.'),
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
