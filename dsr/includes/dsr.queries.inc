<?php
/**
 * @file
 * Report queries.
 */

/**
 * Ecommerce agnostic report select query.
 *
 * @param string $status
 *   Accepted values: 'payment_received', 'refunded', or 'partially_refunded'.
 *
 * @param string $start
 *   The report start date formatted for MySQL.
 *
 * @param string $end
 *   The report end date formatted for MySQL.
 *
 * @return array
 *   An array with 'total_count' and 'total_amount' keys.
 */
function dsr_select($status, $start, $end) {
  $results = array(
    'total_count' => 0,
    'total_amount' => 0,
  );

  $query = "SELECT MAX(t.transaction_id), t.order_id, t.amount FROM {commerce_payment_transaction} t JOIN {commerce_order} o ON o.order_id = t.order_id WHERE t.status = 'success' AND o.status = :status AND from_unixtime(t.changed) BETWEEN :start AND :end GROUP BY t.order_id";
  $replacements = array(
    ':status' => $status,
    ':start' => $start,
    ':end' => $end,
  );
  $transactions = db_query($query, $replacements);
  foreach ($transactions as $transaction) {
    $results['total_count']++;
    $results['total_amount'] += $transaction->amount;
  }

  $results['total_amount'] = $results['total_amount'] / 100;

  return $results;
}

/**
 * Get a list of order data by status and date range.
 *
 * @param string $status
 *   Order status (payment_received, refunded, partially_refunded).
 *
 * @param string $start
 *   The start date formatted for MySQL.
 *
 * @param string $end
 *   The end date formatted for MySQL.
 *
 * @return array
 *   An array of orders matching the select criteria.
 */
function dsr_get_orders($status, $start, $end) {
  $orders = array();

  $query = "SELECT MAX(t.transaction_id), t.order_id, t.amount FROM {commerce_payment_transaction} t JOIN {commerce_order} o ON o.order_id = t.order_id WHERE t.status = 'success' AND o.status = :status AND from_unixtime(t.changed) BETWEEN :start AND :end GROUP BY t.order_id";
  $replacements = array(
    ':status' => $status,
    ':start' => $start,
    ':end' => $end,
  );
  $transactions = db_query($query, $replacements);
  foreach ($transactions as $transaction) {
    $orders[$transaction->order_id] = $transaction->amount / 100;
  }

  return $orders;
}

/**
 * Perform aggregate query on Salesforce data by donation status and date range.
 *
 * @param string $status
 * @param string $start_date
 * @param string $end_date
 *
 * @return array
 */
function dsr_salesforce_aggregate($status, $start_date, $end_date) {
  $sf_status = array(
    'payment_received' => 'Posted',
    'refunded' => 'Refunded',
    'partially_refunded' => 'Partially Refunded',
  );
  if ($status == 'refunded' || $status == 'partially_refunded') {
    $and = sprintf("AND Parent_Donation__r.StageName = '%s'", $sf_status[$status]);
  }
  else {
    $and = '';
  }

  $soql = sprintf("SELECT SUM(AMOUNT) total_amount, COUNT(AMOUNT) total_count, RecordType.Name record_type FROM Opportunity WHERE StageName = '%s' AND Transaction_Date_Time__c > %s AND Transaction_Date_Time__c < %s"
          . $and .
     " Group By RecordType.Name", $sf_status[$status], $start_date, $end_date);

  $salesforce = salesforce_get_api();

  if ($salesforce->isAuthorized()) {
    $soql = rawurlencode($soql);
    $response = $salesforce->apiCall('query?q=' . $soql);
    return _dsr_parse_aggregate_result($response);
  }
  else {
    drupal_set_message('Unable to connect to Salesforce at this time. Please double check your credentials and try again.');
    return array('total_count' => 0, 'total_amount' => 0);
  }

}

/**
 * Runs a predefined SOQL statement and returns all the results as an array.
 *
 * @param $soql
 *   The SOQL statement to run.
 * @param $key_field
 *   The field to use as the key if you want an associative array returned.
 *
 * @deprecated $salesforce->client is not an object. Also this doesn't appear
 *             to be called anywhere.
 */
function dsr_soql_select($soql, $key_field = NULL) {
  $records = array();
  $continue = TRUE;

  $salesforce = salesforce_get_api();
  // Make sure we've got a good connection.
  if ($salesforce->isAuthorized()) {
    $results = $salesforce->client->query(rawurlencode($soql));
    return $results->records;
  }

  // Return false if we couldn't connect.
  return FALSE;
}
