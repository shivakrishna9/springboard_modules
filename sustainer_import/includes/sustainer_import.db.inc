<?php

/**
 * @file database helper functions.
 */

/**
 *  List master order ids in the import queue.
 */
function sustainer_import_list_master_order_ids() {
  $query = db_select('sustainer_import_queue', 's');
  $query->fields('s', array('master_order_id'));
  $results = $query->execute();

  return $results;
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

