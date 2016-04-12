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
      'feedback' => 'feedback',
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
    );
    $type = drush_choice($options, dt('What kind of record would you like to queue?'));
  }

  // Get options.
  $limit = drush_get_option('limit', 999999999);
  $feedback = drush_get_option('feedback', 10);

  // Populate list of ids to operate on.
  switch ($type) {
    case 'contact':
      $ids = db_query_range('SELECT uid FROM {users} ORDER BY created ASC', 0, $limit)->fetchCol();
      $batch_process_function = 'springboard_dw_queue_legacy_users';
      break;

    case 'advocacy action':
      $ids = db_query_range("select s.sid, s.nid, s.uid FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE type = 'sba_message_action'", 0, $limit)->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_advocacy_actions';
      break;

    case 'advocacy message':

      break;

    case 'petition':
      $ids = db_query_range("select s.sid, s.nid, s.uid FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE type = 'springboard_petition'", 0, $limit)->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_petition';
      break;

    case 'advocacy petition message':

      break;

    case 'form':
      $ids = db_query_range('SELECT n.nid FROM {node} n INNER JOIN {webform_user} w ON n.nid = w.nid ORDER BY created ASC', 0, $limit)->fetchCol();
      $batch_process_function = 'springboard_dw_queue_legacy_forms';
      break;

    default:
      return drush_set_error(dt('"@type" is not a valid record type. Nothing was processed.', array('@type' => $type)));
  }

  $record_count = count($ids);

  // Break up all of our data so each process does not time out.
  $chunks = array_chunk($ids, $feedback);
  $operations = array();

  // For every chunk, assign some method to run on that chunk of data.
  $i = 0;
  foreach ($chunks as $chunk) {
    $i++;
    $operations[] = array($batch_process_function,
                      array($chunk, 'details' => t('(Importing records @chunk of @count)',
                        array('@chunk' => $i * $feedback, '@count' => $record_count)
                        ),
                      ),
    );
  }

  // Put all that information into our batch array.
  $batch = array(
    'operations' => $operations,
    'title' => t('Import batch'),
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
}

/**
 * Callback for finished batch.
 */
function springboard_dw_queue_legacy_finished_batch($success, $results, $operations) {
  print t('Finished queueing!');
}

/**
 *
 */
function springboard_dw_queue_legacy_chunk($chunk, $operation_details, &$context) {
  // Do something to $chunk, maybe create a node?
  // var_dump($chunk);
  springboard_dw_queue_legacy_users($chunk);
  $context['message'] = $operation_details;
}


/**
 * Batch process callbackfor processing users.
 */
function springboard_dw_queue_legacy_users($uids, $operation_details, &$context) {
  foreach ($uids as $uid) {
    // Create the user item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('contact', $uid)) {

      // Take the account object and prep for contact mapping.
      $user_obj = entity_metadata_wrapper('user', $uid);
      $user_data = springboard_dw_get_all_entity_properties($user_obj);

      // Map the user contact.
      $contact_item = springboard_dw_map_contact_item($uid, $user_data);

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
function springboard_dw_queue_legacy_forms($ids, $operation_details, &$context) {
  $nodes = node_load_multiple($ids);
  foreach ($nodes as $node) {
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
    // Create the Advocacy action item in the queue if not warehoused
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
// var_dump($submissions);
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($submissions as $submission) {
    $action = springboard_dw_queue_legacy_build_action($submission);
    // Create the Advocacy action item in the queue if not warehoused.
    // var_dump($action->node->nid);
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
