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
      $orders = dsr_get_orders('payment_received', $drupal_dates['start'], $drupal_dates['end']);
      break;

    case 'refunded':
      $donations = dsr_get_refunded_donations($salesforce_dates['start'], $salesforce_dates['end']);
      $orders = dsr_get_orders('refunded', $drupal_dates['start'], $drupal_dates['end']);
      break;

    case 'partially_refunded':
      $donations = dsr_get_partially_refunded_donations($salesforce_dates['start'], $salesforce_dates['end']);
      $orders = dsr_get_orders('partially_refunded', $drupal_dates['start'], $drupal_dates['end']);
      break;
  }



  $missing_orders = array_diff_key($orders, $donations);
  $variables = array(
    'orders' => $missing_orders,
    'status' => $status,
    'start_date' => $start_date,
    'end_date' => $end_date,
  );
  return theme('dsr_discrepancy_report', $variables);
}

/**
 * Theme function for dsr discrepancy report.
 */
function theme_dsr_discrepancy_report($variables) { //$orders, $status, $start_date, $end_date) {
  $orders = isset($variables['orders']) ? $variables['orders'] : array();
  $status = isset($variables['status']) ? $variables['status'] : '';
  $start_date = isset($variables['start_date']) ? $variables['start_date'] : '';
  $end_date = isset($variables['end_date']) ? $variables['end_date'] : '';

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
    $variables = array(
      'title' => '',
      'items' => $statues,
      'attributes' => array(),
      'type' => 'ul',
    );
    $output .= theme_item_list($variables);
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


  if (module_exists('fundraiser_commerce')) {
    $links[] = l('view order', 'admin/commerce/orders/' . $order_id);
  }
  elseif (module_exists('fundraiser_ubercart')) {
    $links[] = l('view order', 'admin/store/orders/' . $order_id);
  }
  $links[] = l('view form submission', sprintf('node/%s/submission/%s', $submission['webform_nid'], $submission['sid']));
  // TODO: replace batch viewer
  //$links[] = l('view batches', '', array('fragment' => ' ', 'external' => TRUE, 'attributes' => array('class' => 'batch-viewer')));

  $variables = array(
    'items' => $links,
    'title' => '',
    'attributes' => array(),
    'type' => 'ul',
  );
  return theme_item_list($variables);
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

  // missing "data", "success", "fields",
  $result = db_query('
    SELECT
      i.bid AS batch_id,
      FROM_UNIXTIME(b.end) AS date_processed,
      i.retries,
      i.status,
      i.message
    FROM {salesforce_log_item} i
    LEFT JOIN {salesforce_log_batch} b
      ON i.bid = b.bid
    WHERE i.drupal_id = :oid AND i.object_type = :type ORDER BY b.end DESC
  ', array(':oid' => $order_id, ':type' => $type));

  while ($data = $result->fetchAssoc()) {
    $batches[$data['batch_id']] = array(
      //'object' => unserialize($data['data']),
      //'success' => $data['success'],
      'date_processed' => $data['date_processed'],
      'retry' => $data['retries'] ? 1 : 0, //$data['is_retry'],
      'error_code' => $data['status'],
      'error_message' => $data['message'],
      //'error_fields' => $data['fields'],
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
  $result = db_query('
    SELECT
      sid,
      nid AS webform_nid
    FROM {fundraiser_donation} WHERE did = :did
  ', array(':did' => $order_id));
/*    UNION
    SELECT f.sid, f.webform_nid FROM {fundraiser_donation} f
    INNER JOIN fundraiser_recurring r on r.master_order_id = f.order_id
    WHERE r.order_id = %d",
  $order_id);*/

  return $result->fetchAssoc();
}

function dsr_get_order_sfid($order_id) {
  return 0; //salesforce_management_api_id_load('donation', $order_id);
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

function theme_dsr_report_table($variables) {
  $drupal_data = $variables['drupal_data'];
  $sf_data = $variables['salesforce_data'];
  $label = $variables['label'];
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
  // TODO: check this
  if (isset($sf_data['types'])) {
    foreach ($sf_data['types'] as $type => $data) {
      $rows[] = array(
        "<em>$type</em>",
        $data['total_count'],
        number_format((float) $data['total_amount'], 2, '.', ','),
      );
    }
  }
  $vars = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'colgroups' => array(),
    'caption' => '',
    'sticky' => FALSE,
    'empty' => '',
  );

  $content = theme('table', $vars);
  $output = theme('dsr_box', array('content' => $content, 'label' => $label));
  return $output;
}

function theme_dsr_box($variables) {
  $output = '<h2>' . $variables['label'] . '</h2>';
  $output .= '<div class="dsr-box">' . $variables['content'] . '</div>';
  return $output;
}
