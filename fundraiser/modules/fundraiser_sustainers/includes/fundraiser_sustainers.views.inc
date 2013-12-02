<?php

/**
 * @file
 * Views definition.
 */

/**
 * Implements hook_views_data().
 */
function fundraiser_sustainers_views_data() {
  // Fundraiser sustainers log table.
  $data['fundraiser_sustainers_log']['table']['group']  = t('Fundraiser sustainers log');

  // Advertise this table as a possible base table.
  $data['fundraiser_sustainers_log']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Sustainer log'),
    'help' => t("Log of sustainer donations processed."),
  );

  // Fields.
  // ----------------------------------------------------------------
  // Field log id.
  $data['fundraiser_sustainers_log']['lid'] = array(
    'title' => t('Sustainer Log ID'),
    'help' => t('The unique ID of the log entry.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Field did.
  $data['fundraiser_sustainers_log']['did'] = array(
    'title' => t('Donation ID'),
    'help' => t('The ID of the processed donation.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'title' => t('Order'),
      'help' => t("Relate this log to its full order"),
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_order',
      'base field' => 'order_id',
      'field' => 'did',
      'label' => t('Order'),
    ),
  );

  // Field gateway.
  $data['fundraiser_sustainers_log']['gateway'] = array(
    'title' => t('Gateway'),
    'help' => t('The gateway used to process the donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // Field date_processed.
  $data['fundraiser_sustainers_log']['date_processed'] = array(
    'title' => t('Date processed'),
    'help' => t('Timestamp of when the donation was processed.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  // Field success.
  $data['fundraiser_sustainers_log']['success'] = array(
    'title' => t('Success'),
    'help' => t('Flag to indicate whether or not the charge was successful.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Field lock_id.
  $data['fundraiser_sustainers_log']['lock_id'] = array(
    'title' => t('Lock ID'),
    'help' => t('The lock that was placed on the record during processing.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;
}
