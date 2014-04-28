<?php

function commerce_litle_views_data() {

  $data['commerce_litle_fraud']['table']['group'] = t('Commerce Litle Advanced Fraud');

  $data['commerce_litle_fraud']['table']['base'] = array(
    'field' => 'order_id', // This is the identifier field for the view.
    'title' => t('Order ID'),
    'weight' => -10,
  );

  $data['commerce_litle_fraud']['table']['join'] = array(
    'commerce_order' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );

  $data['commerce_litle_fraud']['order_id'] = array(
    'title' => t('Order ID'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['example_table']['litle_txn_id'] = array(
    'title' => t('Transaction ID'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['commerce_litle_fraud']['avs_result'] = array(
    'title' => t('AVS result'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

    $data['commerce_litle_fraud']['device_review_status'] = array(
    'title' => t('Device Review Status'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['commerce_litle_fraud']['device_reputation_score'] = array(
    'title' => t('Device Reputation Score'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  return $data;
}