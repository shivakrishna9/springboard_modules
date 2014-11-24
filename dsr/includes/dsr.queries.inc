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

  $results = db_query('
    SELECT
      COALESCE(SUM(f.amount), 0) AS total_amount,
      COUNT(f.amount) AS total_count
    FROM {fundraiser_donation} f
    WHERE
      FROM_UNIXTIME(f.changed)
      BETWEEN :start AND :end
      AND f.status = :status
  ', array(':start' => $start, ':end' => $end, ':status' => $status))->fetchAssoc();

  return !empty($results) ? $results : array(
    'total_count' => 0,
    'total_amount' => 0,
  );
}


/**
 * Get a list of order data by status and date range.
 *
 * @param string $status
 *   Order status (payment_received, refunded, partially_refunded).
 *
 * @param string $start_date
 *   The start date formatted for MySQL.
 *
 * @param string $end_date
 *   The end date formatted for MySQL.
 *
 * @return array
 *   An array of orders matching the select criteria.
 */
function dsr_get_orders($status, $start_date, $end_date) {
  $orders = array();

  $results = db_query('
    SELECT
      f.did,
      f.amount
    FROM {fundraiser_donation} f
    WHERE
      FROM_UNIXTIME(f.changed)
      BETWEEN :start AND :end
      AND f.status = :status
  ', array(':start' => $start_date, ':end' => $end_date, ':status' => $status));
  while ($data = $results->fetchAssoc()) {
    $orders[$data['did']] = $data['amount'];
  }

  return $orders;
}

/**
 * Perform aggregate query on Salesforce data by donation status and date range
 *
 * @param string $status
 * @param string $start_date
 * @param string $end_date
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

  $soql =  sprintf("SELECT SUM(AMOUNT) total_amount, COUNT(AMOUNT) total_count, RecordType.Name record_type FROM Opportunity WHERE StageName = '%s' AND Transaction_Date_Time__c > %s AND Transaction_Date_Time__c < %s"
          . $and .
     " Group By RecordType.Name", $sf_status[$status], $start_date, $end_date);

  $salesforce = salesforce_get_api();

  // make sure we've got a good connection
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
  // make sure we've got a good connection
  if ($salesforce->isAuthorized()) {
    $results = $salesforce->client->query(rawurlencode($soql));
    return $results->records;
  }

  // return false if we couldn't connect
  return FALSE;
}
