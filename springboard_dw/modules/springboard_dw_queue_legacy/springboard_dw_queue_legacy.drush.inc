<?php
/**
 * @file
 * Drush commands for the Springboard data warehouse module.
 */

/**
 * Implements hook_drush_help().
 */
function springboard_dw_queue_legacy_drush_help($command) {
  switch ($command) {
    case 'drush:springboard-dw-queue-legacy-data':
      return dt('Add legacy data to Springboard data warehouse export queue.');
  }
}

/**
 * Implements hook_drush_command().
 */
function springboard_dw_queue_legacy_drush_command() {
  $items = array();
  $items['springboard-dw-queue-legacy-data'] = array(
    'callback' => 'springboard_dw_queue_legacy_setup_batch',
    'description' => dt('Add legacy data to Springboard data warehouse export queue.'),
    'arguments' => array(
      'type' => 'The type of record to be queued',
    ),
    'options' => array(
      'limit' => 'limit',
      'batch-size' => 'batch-size',
      'reset' => 'reset',
    ),
    'examples' => array(
      'Standard example' => 'springboard-dw-queue-legacy-data',
    ),
    'aliases' => array('sb-dw-queue'),
  );

  return $items;
}

/**
 * Callback function for drush springboard-dw-queue-legacy-data.
 *
 * Processes the Springboard data warehouse export queue.
 */
function springboard_dw_queue_legacy_setup_batch($type = '') {
  // Check for existence of argument.
  if (!$type) {
    $options = array(
      'contact' => dt('contact'),
      'form' => dt('form'),
      'advocacy action' => dt('advocacy action'),
      'advocacy message' => dt('advocacy message'),
      'petition' => dt('petition'),
      'advocacy petition message' => dt('advocacy petition message'),
    );
    $type = drush_choice($options, dt('What kind of record would you like to queue?'));
  }

  // Get options.
  $limit = drush_get_option('limit', 999999999);
  $batch_size = drush_get_option('batch-size', 10);

  // Get the highwater mark for this type so we can pick up where we left off.
  $highwater = springboard_dw_queue_legacy_get_highwater_mark($type);

  // Populate list of records to operate on.
  switch ($type) {
    case 'contact':
      $records = db_query_range('SELECT uid, created FROM {users} WHERE created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_users';
      break;

    case 'advocacy action':
      $records = db_query_range("SELECT s.sid, s.nid, s.uid, s.submitted AS created FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE type = 'sba_message_action' AND s.submitted > :highwater ORDER BY s.submitted ASC", 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_advocacy_actions';
      break;

    case 'advocacy message':
      $records = springboard_dw_queue_legacy_get_advocacy_messages();
      $batch_process_function = 'springboard_dw_queue_legacy_advocacy_message';
      break;

    case 'petition':
      $records = db_query_range("SELECT s.sid, s.nid, s.uid, s.submitted AS created FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE type = 'springboard_petition' AND s.submitted > :highwater ORDER BY s.submitted ASC", 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_petition';
      break;

    case 'advocacy petition message':

      break;

    case 'form':
      $records = db_query_range('SELECT n.nid, n.created FROM {node} n INNER JOIN {webform_user} w ON n.nid = w.nid WHERE n.created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_forms';
      break;

    default:
      return drush_set_error(dt('"@type" is not a valid record type. Nothing was processed.', array('@type' => $type)));
  }

  $record_count = count($records);
  drush_log(dt(':count records found to process.', array(':count' => $record_count)), 'ok');
  // Break up all of our data so each process does not time out.
  $chunks = array_chunk($records, $batch_size);
  $operations = array();

  // For every chunk, assign some method to run on that chunk of data.
  $i = 0;
  foreach ($chunks as $chunk) {
    $i++;
    $operations[] = array($batch_process_function,
                      array($chunk, 'details' => t('(Queued records @chunk of @count)',
                        array('@chunk' => $i * $batch_size, '@count' => $record_count)
                        ),
                      ),
    );
  }

  // Put all that information into our batch array.
  $batch = array(
    'operations' => $operations,
    'title' => t('Queue batch'),
    'init_message' => t('Initializing'),
    'error_message' => t('An error occurred'),
    'finished' => 'springboard_dw_queue_legacy_finished_batch',
  );

  // Get the batch process all ready!
  batch_set($batch);
  $batch =& batch_get();

  // Because we are doing this on the back-end, we set progressive to false.
  $batch['progressive'] = FALSE;

  // Start processing the batch operations.
  drush_backend_batch_process();

  // Set highwater mark for this type so we can pick up where we left off next
  // time this command is run.
  if ($record_count > 0) {
    $last_record = end(array_values($records));
    $highwater = $last_record->created;
  }
  // There were no records to process, so set the highwater mark to now.
  else {
    $highwater = time();
  }
  drush_log(dt('Setting highwater mark to :highwater', array(':highwater' => date(DATE_ATOM, $highwater))), 'ok');
  springboard_dw_queue_legacy_set_highwater_mark($type, $highwater);

}

/**
 * Callback for finished batch.
 */
function springboard_dw_queue_legacy_finished_batch($success, $results, $operations) {
  drush_log(dt('Finished queueing'));
}

/**
 * Batch process callbackfor processing users.
 */
function springboard_dw_queue_legacy_users($records, $operation_details, &$context) {
  foreach ($records as $record) {
    // Create the user item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('contact', $record->uid)) {

      // Take the account object and prep for contact mapping.
      $user_obj = entity_metadata_wrapper('user', $record->uid);
      $user_data = springboard_dw_get_all_entity_properties($user_obj);

      // Map the user contact.
      $contact_item = springboard_dw_map_contact_item($record->uid, $user_data);

      // Create the queue item to update the contact record.
      $queue = DrupalQueue::get("springboard_dw_export");
      $queue->createItem($contact_item);
      drush_log('Queued contact item.', 'ok');
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for pressing webforms.
 */
function springboard_dw_queue_legacy_forms($records, $operation_details, &$context) {
  foreach ($records as $record) {
    $node = node_load($record->nid);
    // Create the form item.
    if (isset($node->is_webform_user) && $node->is_webform_user) {

      // Get the Drupal data warehouse queue.
      $queue = DrupalQueue::get("springboard_dw_export");

      // Create the form item in the queue if not warehoused.
      if (!springboard_dw_item_is_warehoused('form', $node->nid)) {
        $form_item = springboard_dw_map_form_item($node);
        $queue->createItem($form_item);
      }
    }
  }
  $context['message'] = $operation_details;
}

/**
 *
 */
function springboard_dw_queue_legacy_advocacy_actions($submissions, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");

  foreach ($submissions as $submission) {
    $action = springboard_dw_queue_legacy_build_action($submission);
    // Create the Advocacy action item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('advocacy action', $action->node->nid)) {
      $message_action_item = springboard_dw_map_advocacy_action_item($action);
      $queue->createItem($message_action_item);
    }
  }
  $context['message'] = $operation_details;
}

/**
 *
 */
function springboard_dw_queue_legacy_petition($submissions, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($submissions as $submission) {
    $action = springboard_dw_queue_legacy_build_action($submission);
    // Create the Advocacy action item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('petition', $action->node->nid)) {
      $petition_item = springboard_dw_map_petition_item($action);
      $queue->createItem($petition_item);
    }
  }
  $context['message'] = $operation_details;
}
/**
 *
 */
function springboard_dw_queue_legacy_advocacy_message($submissions, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($submissions as $submission) {
    $action = springboard_dw_queue_legacy_build_action($submission);
    // Create the Advocacy action item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('advocacy message', $action->node->nid)) {
      $petition_item = springboard_dw_map_petition_item($action);
      $queue->createItem($petition_item);
    }
  }
  $context['message'] = $operation_details;
}

/**
 *
 */
function springboard_dw_queue_legacy_build_action($submission) {
  // Build the action object by re-creating the data that is available during
  // hook_advocacy_success().
  $node = node_load($submission->nid);
  $webform_submission_data = springboard_dw_webform_submission_data_keyed($node->nid, $submission->sid);
  $user = user_load($submission->uid);
  $action = new stdClass();
  $action->node = $node;
  $action->contact = $user;
  // @TODO how to populate $data? What even goes in there?
  $action->data = $data;
  $action->webform = $webform_submission_data;
  $action->sid = $sid;
  return $action;
}

/**
 * [springboard_dw_queue_legacy_get_highwater_mark description]
 * @param  [type] $type [description]
 * @return [type]       [description]
 */
function springboard_dw_queue_legacy_get_highwater_mark($type) {
  if (drush_get_option('reset', FALSE)) {
    drush_log(dt('Resetting highwater mark to zero.'), 'ok');
    return 0;
  }
  return db_query('SELECT highwater FROM {springboard_data_warehouse_queue_legacy} WHERE type = :type', array(':type' => $type))->fetchAssoc();
}

/**
 * [springboard_dw_queue_legacy_set_highwater_mark description]
 * @param  [type] $type      [description]
 * @param  [type] $timestamp [description]
 * @return [type]            [description]
 */
function springboard_dw_queue_legacy_set_highwater_mark($type, $timestamp) {
  $record = array('type' => $type, 'highwater' => $timestamp);
  $result = drupal_write_record('springboard_data_warehouse_queue_legacy', $record, 'type');
  return $result;
}

/**
 *
 */
function springboard_dw_queue_legacy_get_advocacy_messages() {
  $advocacy_action_records = db_query("SELECT nid FROM {node} WHERE type = 'sba_message_action' ORDER BY created ASC")->fetchAll();
  $advocacy_ids = array();
  foreach($advocacy_action_records as $advocacy_action_record) {
    $node = node_load($advocacy_action_record->nid);
    $messages = springboard_dw_queue_legacy_get_advocacy_message_details($node->advocacy_id);
  }
  return $messages;
}

/**
 *
 */
function springboard_dw_queue_legacy_get_advocacy_message_details($action_id) {
  $details = array();
  drush_log('Getting a page for ' . $action_id, 'ok');
  $page = springboard_dw_queue_legacy_get_advocacy_message_details_page($action_id, 1);
  $details[] = $page['data'];
  while ($page->current_page < $page->last_page) {
    drush_log('Getting a page for ' . $action_id, 'ok');

    $page = springboard_dw_queue_legacy_get_advocacy_message_details_page($action_id, $page->current_page + 1);
    $details[] = $page['data'];
  }

  return $details;
}

/**
 * Grabs a single page of JSON data
 *
 * @return array
 */
function springboard_dw_queue_legacy_get_advocacy_message_details_page($action_id, $page = 1) {
  // Composer Vendor autoload.
  drush_autoload(DRUPAL_ROOT."/sites/all/files/vendor/autoload.php");

  // Load Guzzle client.
  $guzzle = new GuzzleHttp\Client(['http_errors' => FALSE]);
  $params = array(
    'query' => array(
      'access_token' => 'ys9O22HXC9fdyS8JA5diqIzTtdTq5iIpPiYehoLf',
      'page' => $page,
    ),
  );
  $response = $guzzle->request('GET', 'https://advocacy-staging.gospringboard.io/api/v1/messages/get/' . $action_id, $params);
  $body = $response->getBody();
  $body_text = $body->getContents();
  return json_decode($body_text, TRUE);
}
