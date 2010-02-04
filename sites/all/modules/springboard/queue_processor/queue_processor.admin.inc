<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the queue processor module.
 */

/**
 * Batch detail page
 */
function batch_detail($id) {
	//$output = 'Batch detail ' . $id . '<pre>';
	$batch = load_batch($id);
	//$output .= print_r($batch, true);
	//$output .= '</pre>';
	
	$output = '<h2>Batch #' . $batch['id'] . '</h2>';
	$output .= '<h4>Type: ' . ucfirst($batch['type']) . '</h4>';
	
	$header = array('Id', 'Type', 'Success', 'Date Processed');
	$items = array();
	foreach($batch['items'] as $item) {
		$items[] = array(
			'oid' => $item->oid,
			'type' => $item->type,
			'success' => $item->success,
			'date_processed' => $item->date_processed,
		);
	}
	
	$output .= '<h4>Items</h4>';
	$output .= theme('table', $header, $items);
	return $output;
}

/**
 * Current queue report
 */
function current_queue_report() {
	$result = db_query("SELECT oid, 'Unknown' as title, sfid, type, action, created FROM {sf_queue} ORDER BY created desc");
	$rows = array();
	
	// loop over each item in queue and allow individual modules a chance to add the title
	while ($row = db_fetch_array($result)) {
		$row['title'] = array_shift(module_invoke_all('queue_report_item_title', $row));
		$rows[] = $row;
	}
	$header = array('Id', 'Title', 'SFID', 'Type', 'Action', 'Date Added');
	
	$output = 'These items are currently queued up and will be exported to Salesforce during the next cron run.';
 	$output .= theme('table', $header, $rows, array('id' => 'current-queue'));
	return $output;
}

/**
 * List of batches that have been processed by the queue.
 */
function batch_history_report() {
	$result = db_query("SELECT id, type, from_unixtime(created, '%m/%e/%Y %h:%i:%S %p'), record_count, success_count, failure_count,
	 	from_unixtime(start_time, '%m/%e/%Y %h:%i:%S %p'), from_unixtime(end_time, '%m/%e/%Y %h:%i:%S %p') FROM {sf_batch} ORDER BY created desc");
	$rows = array();
	
	while($row = db_fetch_array($result)) {
		$row['id'] = l($row['id'], 'admin/reports/salesforce/batch/' . $row['id']);
		$rows[] = $row;
	}

	$header = array('Id', 'Type', 'Created', 'Items', 'Successes', 'Failures', 'Start Time', 'End Time');
	$output = 'Click on the id of the batch to drill down into more detail.';
	$output .= theme('table', $header, $rows, array('id' => 'batch-report'));
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
 * Form builder. Configure queue processor.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function queue_processor_admin_settings() {
	$form['queue_batch_size'] = array(
		'#type' => 'textfield',
		'#title' => t('Batch size'),
		'#default_value' => variable_get('queue_batch_size', 200),
		'#description' => t('The number of records to include in each batch.'),
		'#size' => 4,
	);
	
	$form['queue_retry_max'] = array(
		'#type' => 'textfield',
		'#title' => t('Maximum retry attempts'),
		'#default_value' => variable_get('queue_retry_max', 3),
		'#description' => t('The maximum number of times to retry a failed object before removing it from the queue.'),
		'#size' => 2,
	);
	
	$form['queue_summary_email'] = array(
		'#type' => 'textfield',
		'#title' => t('Send email summary to'),
		'#description' => t('Send a detailed summary after each cron run to this email address.'),
		'#default_value' => variable_get('queue_summary_email', NULL),
	);
	
	$processing_order = variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node'));
	
	$form['type_processing_order']['#tree'] = TRUE;
	$tree = &$form['type_processing_order'];

	foreach($processing_order as $key => $type) {
		$tree[$type]['title'] = array(
			'#value' => ucfirst($type),
		);
		$tree[$type]['weight'] = array(
			'#type' => 'weight',
			'#delta' => count($processing_order),
			'#default_value' => $key,
		);
	}
	
	// Actions
  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => 'Save configuration',
		'#submit' => array('queue_processor_admin_settings_submit'),
  );

	$form['rest'] = array(
    '#type'   => 'submit',
    '#value'  => 'Reset to defaults',
    '#submit' => array('queue_processor_admin_settings_reset'),
  );
	
	return $form;
}

/**
 * Validates queue processor settings.
 */
function queue_processor_admin_settings_validate($form, &$form_state) {
	if (!is_numeric($form_state['values']['queue_batch_size'])) {
		form_set_error('queue_batch_size', t('Queue batch size must be numeric.'));
	}
	
	if (!is_numeric($form_state['values']['queue_retry_max'])) {
		form_set_error('queue_retry_max', t('Queue retry max must be numeric.'));
	}
}

/**
 * Submit action for settings page
 */
function queue_processor_admin_settings_submit($form, &$form_state) {
	$ordered_types = $form_state['values']['type_processing_order'];
	
	// sort the types based on weights
	uasort($ordered_types, "sort_weights");
  
	variable_set('queue_processing_order', array_keys($ordered_types));
	variable_set('queue_batch_size', $form_state['values']['queue_batch_size']);
	variable_set('queue_retry_max', $form_state['values']['queue_retry_max']);
	variable_set('queue_summary_email', $form_state['values']['queue_summary_email']);
  
	drupal_set_message(t('Settings have been saved.'));
}

/**
 * Resets queue processing settings to their default values
 */
function queue_processor_admin_settings_reset($form, &$form_state) {
	variable_set('queue_processing_order', array('user', 'donation', 'webform', 'node'));
	variable_set('queue_batch_size', 200);
	variable_set('queue_retry_max', 3);
	variable_set('queue_summary_email', NULL);
	
	drupal_set_message(t('Settings have been reset.'));
}

function sort_weights($a, $b) {
	if ($a['weight'] == $b['weight']) {
		return 0;
	}
	return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Theme admin settings form
 */
function theme_queue_processor_admin_settings($form) {
  drupal_add_tabledrag('processing-order', 'order', 'sibling', 'processing-element');
  
  // Header
  $header = array(
    t('Processing Order'),
    t('Weight'),
  );
    
  // Rows for the tabledrag
  $rows = array();
  
  $tree = &$form['type_processing_order'];
  foreach (element_children($tree) as $key) {
    $tree[$key]['weight']['#attributes']['class'] = 'processing-element';
    $row = array();
    $row[] = drupal_render($tree[$key]['title']);
    $row[] = drupal_render($tree[$key]['weight']);
    
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }

  // Build the output
	$output = '<div>' . t('Drag the types to rearrange the order in which they process.') . '</div>';
  $output .= theme('table', $header, $rows, array('id' => 'processing-order'));
  $output .= drupal_render($form);

  return $output;
}
