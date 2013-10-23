<?php

/**
 * @file database helper functions.
 */

/**
 *  List master order ids in the import queue.
 */
function sustainer_import_list_master_order_ids() {
  $ids = array();
  $query = db_select('sustainer_import_queue', 's');
  $query->fields('s', array('master_order_id'));
  $results = $query->execute();
  while ($id = $results->fetchField()) {
    $ids[] = $id;
  }
  return $ids;
}

/**
 * Update record status in import queue.
 *
 * @param $master_order_id
 * Original master order id of the sustainer series record.
 *
 * @param $status
 * New status to assign to this record.
 */
function sustainer_import_flag_record($master_order_id, $status) {
  db_query('UPDATE {sustainer_import_queue} SET status = :status WHERE master_order_id = %d', array(':status' => $status, ':id' => $master_order_id));
}

/**
 * Save an entry to the import queue.
 *
 * @param $record
 * sustainer record
 *
 */
function sustainer_import_import_record_insert($record) {
  $master_ids = sustainer_import_list_master_order_ids();

  if (in_array($record['order_id'], $master_ids)) {
    return FALSE;
  }
  dsm($record, 'record during save');
  $data = array(
    'master_order_id' => $record['order_id'],
    'record' => serialize($record),
    'created' => time(),
    'status' => '',
    'gateway_type' => $record['payment_details']['gateway_type'],
  );

  $output = drupal_write_record('sustainer_import_queue', $data);
  dsm($output, 'dwr output');
}

// TODO: consolidate functions that fetch from the queue.
function sustainer_import_get_pending_queue_items() {
  $records = array();
  $results = db_query("SELECT rid, record FROM {sustainer_import_queue} WHERE status = ''");
  while ($item = $results->fetchAssoc()) {
    $records[$item['rid']] = unserialize($item['record']);
  }
  return $records;
}

function sustainer_import_get_failed_queue_items() {
  $records = array();
  $results = db_query("SELECT rid, record FROM {sustainer_import_queue} WHERE status = 'failed'");
  while ($item = $results->fetchAssoc()) {
    $records[$item['rid']] = unserialize($item['record']);
  }
  return $records;
}


function sustainer_import_get_log_entry($rid) {
  $result = db_query('SELECT rid, import_ids, timestamp, errors FROM {sustainer_import_log} WHERE rid = :rid', array(':rid' => $rid));
  $log_entry =  $result->fetchAssoc();
  $log_entry['import_ids'] = unserialize($log_entry['import_ids']);
  $log_entry['errors'] = unserialize($log_entry['errors']);
  return $log_entry;
}
/**
 * Provide a count of queue items by status.
 *
 * @return array
 */
function sustainer_import_queue_status_counts() {
  $output = array();
  $results = db_query('SELECT status as status, COUNT(status) AS count FROM {sustainer_import_queue} GROUP BY status');
  while ($data =  $results->fetchAssoc()){
     $output[] = $data;
  }
  return $output;
}

/**
 * List payment gateway types in queue.
 */
function sustainer_import_queue_gateway_types() {
  $output = array();
  $results = db_query('SELECT DISTINCT(gateway_type) FROM {sustainer_import_queue}');
  while ($data = $results->fetchAssoc()) {
    $output[] = $data['gateway_type'];
  }
  return $output;
}

/**
 * List available payment gateways.
 */
function sustainer_import_list_payment_gateways() {
  $gateway_info = _fundraiser_gateway_info();
  foreach ($gateway_info as $key => $gateway) {
    $gateways[$key] = $gateway['title'];
  }
  dsm($gateways);
  return $gateways;
}

/**
 * List available donation forms.
 */
function _sustainer_import_list_donation_forms() {
  $nodes = array();
  $enabled_node_types = array();
  $types = fundraiser_get_donation_types();
  foreach ($types as $type => $enabled) {
    if ($enabled) {
      $enabled_node_types[] = $type;
    }
  }

  $results = db_query('SELECT nid, title FROM {node} WHERE type IN (:types)', array(':types' => $enabled_node_types));

  while($data = $results->fetchAssoc()) {
    $nodes[$data['nid']] = $data['title'];
  }

  return $nodes;
}


/**
 * Update queue and log tables when a record has finished processing.
 */
function sustainer_import_log_record($rid, $record, $log, $errors) {
  if (count($errors)) {
    drupal_set_message('errors ecountered while processing record:' . $record['order_id'], 'error');
    $status = 'failed';
  }
  else {
    $status = 'complete';
  }
  // update queue record with status.
  db_query('
    UPDATE {sustainer_import_queue}
    SET status = :status
    WHERE rid = :rid',
    array(':status' => $status, ':rid' => $rid));

  // log progress
  $record = array(
    'rid' => $rid, //get row id from queue table
    'import_ids' => serialize($log),
    'timestamp' => time(),
    'errors' => serialize($errors),
  );
  dsm($record, 'log record');
  drupal_write_record('sustainer_import_log', $record);
}
