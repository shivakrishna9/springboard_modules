<?php


/**
 * Implements hook_views_handlers().
 */
function springboard_views_salesforce_management_api_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'springboard_views') . '/modules/salesforce_management_api',
    ),
    'handlers' => array(
      // Filter handlers
      'salesforce_management_api_handler_filter_drupal_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data().
 */
function springboard_views_salesforce_management_api_views_data() {
  $data = array();

  // Salesforce_management_object_map table metadata
  $data['salesforce_management_object_map']['table'] = array(
    'group' => t('Salesforce management object map'),
    'base' => array(
      'title' => t('Salesforce object'),
      'help' => t('Salesforce object data.'),
    ),
  );

  // Salesforce_management_object_map fields
  $data['drupal_type'] = array(
    'title' => t('Drupal type'),
    'help' => t('Drupal object type (e.g. "node", "comment").'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'fundraiser_views_handler_filter_drupal_type',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['oid'] = array();
  $data['sfid'] = array(
    'title' => t('Salesforce object ID'),
    'help' => t('The Salesforce object ID.'),
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
  $data['fieldmap'] = array();

  // --------------------------------------------------------------------------
  // Salesforce_management_field_map table metadata
  // TODO

  // --------------------------------------------------------------------------
  // Salesforce_management_api_error table metadata
  // TODO

  return $data;
}
