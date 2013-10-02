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
  $data = array(
    'master_order_id' => $record['order_id'],
    'record' => serialize($record),
    'created' => time(),
    'status' => '',
  );
  $output = drupal_write_record('sustainer_import_queue', $data, $update);
  dsm($output, 'dwr output');
}

function sustainer_import_get_pending_queue_items() {
  $records = array();
  $results = db_query("SELECT record FROM {sustainer_import_queue} WHERE status = ''");
  while ($item = $results->fetchAssoc()) {
    $records[] = unserialize($item['record']);
  }
  return $records;
}

function sustainer_import_queue_status_counts() {
  $output = array();
  $results = db_query('SELECT status as status, COUNT(status) AS count FROM sustainer_import_queue GROUP BY status');
  while ($data =  $results->fetchAssoc()){
     $output[] = $data;
  }
  return $output;
}


