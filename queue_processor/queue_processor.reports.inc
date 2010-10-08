<?php
// $Id$

/**
 * @file
 * Report callbacks for the queue processor module.
 */

/**
 * Current queue report
 */
function current_queue_report() {
	$pager_num = 0;
	
	$result = pager_query(db_rewrite_sql("SELECT oid, 'Unknown' as title, sfid, type, action, from_unixtime(created, '%m/%e/%Y %h:%i:%S %p') FROM {sf_queue} ORDER BY created desc"), 20, $pager_num);
	$rows = array();
	
	// loop over each item in queue and allow individual modules a chance to add the title
	while ($row = db_fetch_array($result)) {
		$row['title'] = array_shift(module_invoke_all('queue_report_item_title', $row));
		$rows[] = $row;
	}
	$header = array('Drupal Id', 'Title', 'SFID', 'Type', 'Action', 'Date Added');
	
	$output = 'These items are currently queued up and will be exported to Salesforce during the next cron run.';
 	$output .= theme('table', $header, $rows, array('id' => 'current-queue'));
 	$output .= theme('pager', NULL, 20, $pager_num);
	return $output;
}

/**
 * List of batches that have been processed by the queue.
 */
function batch_history_report() {
	$pager_num = 0;
	
	$result = pager_query(db_rewrite_sql("SELECT id, type, from_unixtime(created, '%m/%e/%Y %h:%i:%S %p'), record_count, success_count, failure_count,
	 	from_unixtime(start_time, '%m/%e/%Y %h:%i:%S %p'), from_unixtime(end_time, '%m/%e/%Y %h:%i:%S %p') FROM {sf_batch} ORDER BY created desc"), 20, $pager_num);
	$rows = array();
	
	while($row = db_fetch_array($result)) {
		$row['id'] = l($row['id'], 'admin/reports/salesforce/batch/' . $row['id']);
		$rows[] = $row;
	}

	$header = array('Id', 'Type', 'Created', 'Items', 'Successes', 'Failures', 'Start Time', 'End Time');
	$output = 'Click on the id of the batch to drill down into more detail.';
	$output .= theme('table', $header, $rows, array('id' => 'batch-report'));
	$output .= theme('pager', NULL, 20, $pager_num);
	return $output;
}

/**
 * Batch detail page
 */
function batch_detail($id) {

	$base = drupal_get_path('module', 'queue_processor');
	drupal_add_js($base .'/queue_processor.js');
	drupal_add_css($base .'/queue_processor.css');

	//$output = 'Batch detail ' . $id . '<pre>';
	$batch = load_batch($id);
	
	//$output .= '</pre>';
	$output = '<div id="batch-details">';
	$output .= '<h2>Batch ' . $batch['id'] . '</h2>';
	$output .= 'Action: ' . $batch['action'] . '<br/>';
	$output .= 'Drupal Type: <span class="object-type">' . ucfirst($batch['type']) . '</span><br/>';
	$output .= 'Salesforce Type: <span class="object-type">' . ucfirst($batch['salesforce_type']) . '</span><br/>';
	$output .= '<h4>Items</h4>';
	$output .= '</div>';

	//$output .= '<pre>' . print_r($batch, true) . '</pre>';
	
	
	foreach($batch['records'] as $item) {
		$rows = array();
		$object = unserialize($item->data);
		if ($batch['action'] != 'delete') {
  		foreach($object as $field => $value) {
  			$rows[] = array(
  				'field' => $field,
  				'value' => $value,
  			);
  		}	
		}	
		
		asort($rows);
		$retry = ($item->is_retry == 1) ? 'Yes' : 'No';
		$success = ($item->success == 1) ? 'Yes' : 'No';
		$output .= '<div id="batch-item-container">';
		$output .= '<h5>' . ucfirst($batch['type']) . ' ' . $item->oid . '</h5>';
		$output .= '<div class="batch-item">';
		$output .= '<span>Success:</span>' . $success . '<br/>';
		
		if (!$item->success) {
			$error = load_batch_error($batch['id'], $item->oid, $batch['type']);
			$output .= '<div class="item-error">';
			$output .= '<span>Error message:</span>' . $error->message . '<br/>';
			$output .= '<span>Status code:</span>' . $error->status_code . '<br/>';
			$output .= '<span>Fields:</span>' . $error->fields . '<br/>';
			$output .= '</div>';
		}
		
		$output .= '<span>Processed:</span>' . date('n/j/Y g:i:s a', $item->date_processed) . '<br/>';
		$output .= '<span>Was retry:</span>' . $retry . '<br/>';
		$output .= '<a href="#">Click for object details</a>';
		$output .= '<div class="batch-item-fields">';
		$output .= theme('table', array('Field', 'Value'), $rows);
		$output .= '</div>';
		$output .= '</div>';
		$output .= '</div>';
		//$output .= '</div>';
	}
	
	return $output;
}

/**
 * Salesforce reports landing page.
 */
function saleforce_integration_reports() {
	$output = 'future home of salesforce integration';
	return $output;
}

/**
 * Lists all items in the retry queue
 */
function retry_queue_report() {
	$output = 'The following items are currently in the retry queue. If not edited beforehand, the next export attempt will be performed on the items\'s retry date.';
	$pager_num = 0;
	
	$rows = array();
	$result = pager_query(db_rewrite_sql("SELECT oid, sfid, type, fieldmap, action, from_unixtime(retry_date, '%m/%e/%Y %h:%i:%S %p'), attempts FROM {sf_retry_queue} ORDER BY retry_date ASC"), 20, $pager_num);
	
	while ($row = db_fetch_array($result)) {
		$rows[] = $row;
	}
	$header = array('Drupal Id', 'SFID', 'Type', 'Fieldmap', 'Action', 'Retry Date', 'Attempts');
	$output .= theme('table', $header, $rows, array('id' => 'retry-queue'));
	$output .= theme('pager', NULL, 20, $pager_num);
	return $output;
}

/**
 * Lists all items that have been flagged as a permanent failure
 */
function permanent_failures_report() {
	$output = 'The following items have been marked as permanent failures. No more attempts to export them will be made until they have been edited.';
	$pager_num = 0;
	
	$rows = array();
	$result = pager_query(db_rewrite_sql("SELECT oid, type, fieldmap, action, from_unixtime(date_added, '%m/%e/%Y %h:%i:%S %p') FROM {sf_permanent_failure} ORDER BY date_added DESC"), 20, $pager_num);
	while ($row = db_fetch_array($result)) {
		$rows[] = $row;
	}
	$header = array('Drupal Id', 'Type', 'Fieldmap', 'Action', 'Date Added');
	$output .= theme('table', $header, $rows, array('id' => 'permanent-failures'));
	$output .= theme('pager', NULL, 20, $pager_num);
	return $output;
}