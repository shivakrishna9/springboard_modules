<?php

/**
 * Implementation of hook_views_data().
 */
function springboard_views_queue_api_views_data() {
  $data = array();

  // SF queue table metadata
  $data['sf_queue']['table'] = array(
    'group' => t('Salesforce queue'),
    'base' => array(
      'title' => t('Salesforce queue'),
      'help' => t('Items in the active queue to be synced to Salesforce.'),
    ),
  );

  // SF queue fields
  $data['sf_queue']['id'] = array(
    'title' => t('Primary key: ID'),
    'help' => t('The primary key of the sf_queue table.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
  );
  $data['sf_queue']['oid'] = array(
    'title' => t('Queued object'),
    'help' => t('The queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_queue']['type'] = array(
    'title' => t('Queued object type'),
    'help' => t('The queued object type.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_queue']['sfid'] = array(
    'title' => t('Salesforce ID'),
    'help' => t('The Salesforce ID associated with this queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_queue']['fieldmap'] = array(
    'title' => t('Fieldmap'),
    'help' => t('The ID of the fieldmap associated with this queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['sf_queue']['action'] = array(
    'title' => t('Action'),
    'help' => t('The action to perform when syncing.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // --------------------------------------------------------------------------
  // SF retry queue table metadata
  $data['sf_retry_queue']['table'] = array(
    'group' => t('Salesforce retry queue'),
    'base' => array(
      'title' => t('Salesforce retry queue'),
      'help' => t('Items in the retry queue to be synced to Salesforce.'),
    ),
  );

  // SF retry queue fields
  $data['sf_retry_queue']['id'] = array(
    'title' => t('Primary key: ID'),
    'help' => t('The primary key of the sf_retry_queue table.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
  );
  $data['sf_retry_queue']['oid'] = array(
    'title' => t('Queued object'),
    'help' => t('The queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_retry_queue']['type'] = array(
    'title' => t('Queued object type'),
    'help' => t('The queued object type.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_retry_queue']['sfid'] = array(
    'title' => t('Salesforce ID'),
    'help' => t('The Salesforce ID associated with this queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_retry_queue']['fieldmap'] = array(
    'title' => t('Fieldmap'),
    'help' => t('The ID of the fieldmap associated with this queued object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['sf_retry_queue']['action'] = array(
    'title' => t('Action'),
    'help' => t('The action to perform when syncing.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_retry_queue']['created'] = array(
    'title' => t('Created'),
    'help' => t('The date this item was added to the retry queue.'),
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
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );
  $data['sf_retry_queue']['retry_date'] = array(
    'title' => t('Retry date'),
    'help' => t('The date this item will be re-queued.'),
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
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );
  $data['sf_retry_queue']['attempts'] = array(
    'title' => t('Attempts'),
    'help' => t('Number of attempts on this item.'),
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

  // --------------------------------------------------------------------------
  // SF permanent failures table metadata
  $data['sf_permanent_failure']['table'] = array(
    'group' => t('Salesforce permanent failures'),
    'base' => array(
      'title' => t('Salesforce permanent failures'),
      'help' => t('Items in the retry queue to be synced to Salesforce.'),
    ),
  );

  // SF permanent failures fields
  $data['sf_permanent_failure']['id'] = array(
    'title' => t('Primary key: ID'),
    'help' => t('The primary key of the sf_permanent_failure table.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
  );
  $data['sf_permanent_failure']['oid'] = array(
    'title' => t('Failed object'),
    'help' => t('The failed object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_permanent_failure']['type'] = array(
    'title' => t('Failed object type'),
    'help' => t('The failed object type.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_permanent_failure']['fieldmap'] = array(
    'title' => t('Fieldmap'),
    'help' => t('The ID of the fieldmap associated with this failed object.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['sf_permanent_failure']['action'] = array(
    'title' => t('Action'),
    'help' => t('The action that was attempted when syncing.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data['sf_permanent_failure']['date_added'] = array(
    'title' => t('Date added'),
    'help' => t('The date this item was added to the permanent failures.'),
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
    'argument' => array(
      'handler' => 'views_handler_argument_date',
    ),
  );

  // --------------------------------------------------------------------------
  return $data;
}
