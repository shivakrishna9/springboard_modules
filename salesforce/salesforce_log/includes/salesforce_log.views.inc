<?php

/**
 * file
 */

/**
 * 
 */ 
function salesforce_log_views_data() {
  // ----------------------------------------------------------------------
  // Main Salesforce log batch base table

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
    'title' => t('Start Timestamp'),
    'help' => t('The date the batch started.)'),
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

  // end timestamp
  $data['salesforce_log_batch']['end'] = array(
    'title' => t('End Timestamp'),
    'help' => t('The date the batch finished.)'),
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
    'title' => t('Module'),
    'help' => t('The module linked to the items in this batch.'),
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

  // delta
  $data['salesforce_log_batch']['delta'] = array(
    'title' => t('Delta'),
    'help' => t('The delta of the items in this batch.'),
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

  // object_type
  $data['salesforce_log_batch']['object_type'] = array(
    'title' => t('Object type'),
    'help' => t('The Salesforce object of the items in this batch.'),
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

  // operation
  $data['salesforce_log_batch']['operation'] = array(
    'title' => t('Operation'),
    'help' => t('The sync operation of this batch.'),
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

  // ----------------------------------------------------------------------
  // Main Salesforce log item base table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['salesforce_log_item']['table']['group']  = t('Salesforce logs');

  // Advertise this table as a possible base table
  $data['salesforce_log_item']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Salesforce batch items'), 
    'help' => t("Salesforce items synced through the batch process."),
  );

  // Explain how this table joins to others.
  $data['salesforce_log_item']['table']['join'] = array(
    'salesforce_log_batch' => array(
      'left_field' => 'bid',
      'field' => 'bid',
    ),
  );

  // ----------------------------------------------------------------
  // Fields

  // log item id.
  $data['salesforce_log_item']['lid'] = array(
    'title' => t('Log Item ID'), 
    'help' => t('The unique ID of this item.'), 
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

  // queue item id.
  $data['salesforce_log_item']['item_id'] = array(
    'title' => t('Queue Item ID'), 
    'help' => t('The queue ID of this item.'), 
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

  // batch item id.
  $data['salesforce_log_item']['bid'] = array(
    'title' => t('Batch ID'), 
    'help' => t('The ID of the batch.'), 
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
      'base' => 'salesforce_log_batch', // The name of the table to join with.
      'base field' => 'bid', // The name of the field on the joined table.
      // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
      'handler' => 'views_handler_relationship',
      'label' => t('Batch'),
      'title' => t('Batch'),
      'help' => t('The batch containing this item.'),
    ),
  );

  // status
  $data['salesforce_log_item']['status'] = array(
    'title' => t('Status'),
    'help' => t('The status of this item after it was synced.'),
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

  // message
  $data['salesforce_log_item']['message'] = array(
    'title' => t('Message'),
    'help' => t('The message returned from salesforce when this item was synced.'),
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