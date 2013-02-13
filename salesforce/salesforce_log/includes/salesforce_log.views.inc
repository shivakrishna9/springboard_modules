<?php

/**
 * file
 */

/**
 * 
 */ 
function salesforce_log_views_data() {
  // ----------------------------------------------------------------------
  // Main Aggregator Item base table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['salesforce_log_batch']['table']['group']  = t('Salesforce logs');

  // Advertise this table as a possible base table
  $data['salesforce_log_batch']['table']['base'] = array(
    'field' => 'bid',
    'title' => t('Salesforce batch'), 
    'help' => t("Salesforce batches contain logs of data synced to Salesforce."),
  );

  // ----------------------------------------------------------------
  // Fields

  // batch id.
  $data['salesforce_log_batch']['bid'] = array(
    'title' => t('Batch Item ID'), 
    'help' => t('The unique ID of the batch.'), 
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

  // item timestamp
  $data['salesforce_log_batch']['start'] = array(
    'title' => t('Start Timestamp'), // The item it appears as on the UI, 
    'help' => t('The date the batch started.)'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field_date', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  // end timestamp
  $data['salesforce_log_batch']['end'] = array(
    'title' => t('End Timestamp'), // The item it appears as on the UI, 
    'help' => t('The date the batch finished.)'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field_date', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ), 
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  // count
  $data['salesforce_log_batch']['count'] = array(
    'title' => t('Count'), 
    'help' => t('Total count of items in the batch.'), 
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

  // successes
  $data['salesforce_log_batch']['successes'] = array(
    'title' => t('Successes'), 
    'help' => t('Amount of successesfully synced items in the batch.'), 
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

  // failures
  $data['salesforce_log_batch']['failures'] = array(
    'title' => t('Failures'), 
    'help' => t('Amount of failed items in the batch.'), 
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

  // module
  $data['salesforce_log_batch']['module'] = array(
    'title' => t('Module'), // The item it appears as on the UI, 
    'help' => t('The module linked to the items in this batch.'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // delta
  $data['salesforce_log_batch']['delta'] = array(
    'title' => t('Delta'), // The item it appears as on the UI, 
    'help' => t('The delta of the items in this batch.'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // object_type
  $data['salesforce_log_batch']['object_type'] = array(
    'title' => t('Object type'), // The item it appears as on the UI, 
    'help' => t('The Salesforce object of the items in this batch.'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // operation
  $data['salesforce_log_batch']['operation'] = array(
    'title' => t('Operation'), // The item it appears as on the UI, 
    'help' => t('The sync operation of this batch.'),
    // Information for displaying a title as a field 
    'field' => array(
      'handler' => 'views_handler_field', 
      'click sortable' => TRUE,
    ), 
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter 
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;  
}