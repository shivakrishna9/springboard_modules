<?php

/**
 * Implementation of hook_views_data().
 */
function fundraiser_views_views_data() {
  $data = array();

  // Table metadata
  $data['fundraiser'] = array(
    'table' => array(
      'group' => t('Fundraiser'),
      'join' => array(
        'node' => array(
          'left_field' => 'nid',
          'field' => 'nid',
        ),
      ),
    ),
  );

  // Fields
  $data['fundraiser']['internal_name'] = array(
    'title' => t('Internal name'),
    'help' => t('Internal name of the donation form.'),
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
  $data['fundraiser']['show_other_amount'] = array(
    'title' => t('Show other amount'),
    'help' => t('True or false, whether the donation form has the other_amount field.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  // TODO: Add other fields here.

  return $data;
}
