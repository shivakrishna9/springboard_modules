<?php

/**
 * @file
 *
 * Supplies various DSR reports.
 */

/**
 * Finds orders that do not exist in Salesforce within a given date range.
 *
 * @param $status
 *   The status of the orders you are looking for.
 * @param $start_date
 *   The start date of the range.
 * @param $end_date
 *   The end date of the range.
 */
function dsr_discrepancy_report($status, $start_date, $end_date) {
  $base = drupal_get_path('module', 'dsr');
  drupal_add_js($base . '/dsr.js');
  drupal_add_css($base . '/dsr.css');

  $drupal_dates = _dsr_drupal_date_format($start_date, $end_date);
  $salesforce_dates = _dsr_sf_date_format($start_date, $end_date);

  switch ($status) {
    case 'posted':
      $donations = dsr_get_posted_donations($salesforce_dates['start'], $salesforce_dates['end']);
      $orders = dsr_get_posted_orders($drupal_dates['start'], $drupal_dates['end']);
      break;

    case 'refunded':
      $donations = dsr_get_refunded_donations($salesforce_dates['start'], $salesforce_dates['end']);
      $orders = dsr_get_refunded_orders($drupal_dates['start'], $drupal_dates['end']);
      break;

    case 'partially_refunded':
      $donations = dsr_get_partially_refunded_donations($salesforce_dates['start'], $salesforce_dates['end']);
      $orders = dsr_get_partially_refunded_orders($drupal_dates['start'], $drupal_dates['end']);
      break;
  }



  $missing_orders = array_diff_key($orders, $donations);

  return theme('dsr_discrepancy_report', $missing_orders, $status, $start_date, $end_date);
}

/**
 * Theme function for dsr discrepancy report.
 */
function theme_dsr_discrepancy_report($orders, $status, $start_date, $end_date) {

  $output = _dsr_format_date_heading($start_date, $end_date);

  foreach ($orders as $order_id => $order_total) {
    $statues = array();
    $batches = dsr_get_order_batches($order_id, 'donation');
    $links = _dsr_discrepancy_report_links($order_id);

    // get any sf information. this will help us determine if an order was deleted from Salesforce
    $sf = dsr_get_order_sfid($order_id);

    //print "<pre>".print_r($batches, TRUE)."</pre>";


    $export_status = _dsr_create_export_status($batches);

    //print "<pre>".print_r($last_export_status, TRUE)."</pre>";

    $output .= '<div class="dsr-order">';
    $output .= '<h4>Order ' . $order_id . ' - $' . number_format($order_total, 2, '.', ',') . '</h4>';
    if (!empty($sf['sfid'])) {
      $output .= t('This order may have been deleted in Salesforce! We have it associated with ' . $sf['sfid']);
    }

    foreach ($export_status as $status => $value) {
      $statues[] = ucfirst($status) . ' - ' . $value;
    }

    $output .= theme_item_list($statues);
    $output .= $links;
    $output .= '<div class="dsr-batches">';

    $batch_markup = array();

    foreach ($batches as $batch_id => $batch) {
      $batch_markup[] = _dsr_create_batch_info($batch_id, $order_id, $batch);
    }

    foreach ($batch_markup as $markup) {
      $output .= $markup;
    }
    $output .= '</div>';
    $output .= '</div>';
  }

  return $output;
}

/**
 * Creates a list of links to operate on the order.
 *
 * @param $order_id
 *   The order being processed.
 */
function _dsr_discrepancy_report_links($order_id) {
  // get some information about the submission that generated the order
  $submission = dsr_get_order_submission_id($order_id);

  $links = array(
    l('view order', 'admin/store/orders/' . $order_id),
    l('view form submission', sprintf('node/%s/submission/%s', $submission['webform_nid'], $submission['sid'])),
    l('view batches', '', array('fragment' => ' ', 'external' => TRUE, 'attributes' => array('class' => 'batch-viewer'))),
  );

  return theme_item_list($links);
}

/**
 * Returns all that batches that a particular item was in.
 *
 * @param $order_id,
 *   The id of the order.
 * @param $type
 *   The type of object. Defaults to donation.
 */
function dsr_get_order_batches($order_id, $type = 'donation') {
  $batches = array();
  //$result = db_query("SELECT batch_id, data, success, date_processed, is_retry FROM {sf_batch_item} WHERE oid = '%s' AND type = '%s'", $order_id, $type);

  $result = db_query(
            "
              SELECT i.batch_id, i.data, i.success, i.date_processed, i.is_retry, e.status_code, e.message, e.fields
              FROM {sf_batch_item} i
              LEFT JOIN {sf_batch_error} e on e.batch_id = i.batch_id AND e.oid = i.oid
              WHERE i.oid = '%s' AND i.type = '%s' ORDER BY date_processed DESC
            ",
            $order_id,
            $type
  );

  while ($data = db_fetch_object($result)) {
    $batches[$data->batch_id] = array(
      'object' => unserialize($data->data),
      'success' => $data->success,
      'date_processed' => $data->date_processed,
      'retry' => $data->is_retry,
      'error_code' => $data->status_code,
      'error_message' => $data->message,
      'error_fields' => $data->fields,
    );
  }

  return $batches;
}

/**
 * Returns the submission id for a given order while taking into consideration the order might be part of a recurring series.
 *
 * @param $order_id
 *   The id of the order.
 */
function dsr_get_order_submission_id($order_id) {
  $result = db_query("
    SELECT sid, webform_nid FROM fundraiser_webform_order WHERE order_id = %d
    UNION
    SELECT f.sid, f.webform_nid FROM fundraiser_webform_order f
    INNER JOIN fundraiser_recurring r on r.master_order_id = f.order_id
    WHERE r.order_id = %d",
  $order_id);

  return db_fetch_array($result);
}

function dsr_get_order_sfid($order_id) {
  return salesforce_management_api_id_load('donation', $order_id);
}

/**
 * Parse information about a batch into a presentable format.
 */
function _dsr_create_export_status($batches) {
  $status = array();

  $attempts = count($batches);
  if ($attempts == 0) {
    $status['export attempts'] = t('This order has not yet been exported.');
  }
  else {
    reset($batches);
    $last_batch_id = key($batches);

    $status['export attempts'] = $attempts;
    $status['last export attempt'] = date('m/d/Y g:i:s a', $batches[$last_batch_id]['date_processed']);
    $status['last export status'] = $batches[$last_batch_id]['success'] ? t('Success') : t('Failed') . ' (' . $batches[$last_batch_id]['error_message'] . ')';
  }

  return $status;
}

function _dsr_create_batch_info($batch_id, $order_id, $batch) {
  $rows = array();
  $markup = '<div id="batch-' . $order_id . '-' . $batch_id . '">';
  $markup .= '<h4>Batch ' . $batch_id . '</h4>';
  $markup .= 'Date processed: ' . date('m/d/Y g:i:s a', $batch['date_processed']) . '<br/>';

  if (!empty($batch['object'])) {
    foreach ($batch['object'] as $field => $value) {
      $rows[] = array(
        'data' => array($field, $value),
        'class' => $batch['error_fields'] == $field ? 'error' : 'valid',
      );
    }

    $markup .= '<div class="object-details">';
    $markup .= theme_table(array('Field', 'Value'), $rows);
    $markup .= '</div>';
  }
  else {
    $markup .= t('There are no batch details available. This likely means that an exception was thrown during batch processing.');
  }

  $markup .= '</div>';
  return $markup;
}

function _dsr_format_date_heading($start_date, $end_date) {
  if ($start_date == $end_date) {
    return '<h3>' . date('l F j, Y', strtotime($start_date)) . '</h3>';
  }

  return '<h3>' . date('F j, Y', strtotime($start_date)) . ' -- ' . date('F j, Y', strtotime($end_date)) . '</h3>';
}

function theme_dsr_report_table($label, $sf_data, $drupal_data) {
  $headers = array(
    ' ',
    'Count',
    'Amount',
  );
  $rows[] = array(
    '<strong>Drupal</strong>',
    $drupal_data['total_count'],
    number_format((float) $drupal_data['total_amount'], 2, '.', ','),
  );
  $rows[] = array(
    '<strong>Salesforce</strong>',
    $sf_data['total_count'],
    $sf_data['total_amount'],
  );
  if (isset($sf_data['types'])) {
    foreach ($sf_data['types'] as $type => $data) {
      $rows[] = array(
        "<em>$type</em>",
        $data['total_count'],
        number_format((float) $data['total_amount'], 2, '.', ','),
      );
    }
  }
  $content = theme('table', $headers, $rows, array('class' => 'dsr-report-table'));
  $output = theme_box($label, $content);
  return $output;
}
