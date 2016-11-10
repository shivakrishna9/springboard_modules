<?php

/**
 * Implements hook_drush_command().
 *
 * Adds a drush command which saves a CSV file containing donation data from the Drupal DB.
 */
function springboard_views_drush_command() {
  return array(
    'sb-export-donations-csv' => array(
      'aliases' => array('sb-edc'),
      'description' => t('Exports a CSV file containing donation data from the Drupal DB.'),
      'callback' => '_springboard_views_drush_export_donations_csv',
      'options' => array(
        'node_id' => t('The node ID of the target donation form.'),        'order_status' => t('Filter by order status, using one of the following:') . ' ' .
          'payment_received, pending_future_payment, partially_refunded, refunded, failed, canceled, failed_refund',
        'internal_form_name' => t('Filter by the internal form name field.'),
        'order_date_min' => t('Filter by orders placed after a given date; for example: 11/3/2016'),
        'order_date_max' => t('Filter by orders placed before a a given date; for example: 11/3/2016'),
        'search' => t('Filter results by keyword search.'),
        'sort' => t('Sort results by one of the following: unique_id, form_id, group_id, form_type, name, internal_name, url, created_at, updated_at'),
      ),
    ),
  );
}

/**
 * Callback for sb-export-donations-csv.
 */
function _springboard_views_drush_export_donations_csv() {
  $view = views_get_view('sbv_donations');
  if (isset($view) && $view != FALSE && is_object($view)) {
    $view->set_display('page');
     
    $view->pre_execute();
    //$view->pre_execute(array($node->nid)); // view arg
    $view->execute();
    $view->render();
    print_r($view->result);
  }
}
