<?php

/**
 * @file
 * Defines "Springboard Data Warehouse" Views.
 */

/**
 * Implements hook_views_data().
 */
function springboard_dw_views_data() {
  // Define a pseudo-DB table and assign the custom query handler which queries the remote API server:
  $data['springboard_dw_psudo']['table'] = array(
    'group' => t('Springboard Data Warehouse'),
    'base' => array(
      'title' => t('Springboard Data Warehouse'),
      'help' => t('Data for this view type is fetched from the Data Warehouse rather than a Drupal DB table.'),
      'query class' => 'springboard_dw_views_plugin_query',
    ),
  );

  // Fields:
  $data['springboard_dw_psudo']['count'] = array(
    'title' => t('Donation Count'),
    'help' => t('The amount of donations.'),
    'field' => array(
      'handler' => 'springboard_dw_views_handler_field',
    ),
  );  
  return $data;
}

/**
 * Implements hook_views_plugins().
 *
 * Define a query plugin to fetch data from the data warehouse vs DB queries.
 */
function springboard_dw_views_plugins() {
  $plugins = array(
    // The query plugin replaces DB queries with API queries:
    'query' => array(
      'springboard_dw_views_plugin_query' => array(
        'title' => t('Springboard Data Warehouse API Request'),
        'help' => t('Uses Guzzle for querying remote data'),
        'handler' => 'springboard_dw_views_plugin_query',
        'path' => drupal_get_path('module', 'springboard_dw') . '/includes/views/plugins',
      ),
    ),
  );
  return $plugins;
}

