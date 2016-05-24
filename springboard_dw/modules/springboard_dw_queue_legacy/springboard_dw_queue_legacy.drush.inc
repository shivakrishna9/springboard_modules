<?php
/**
 * @file
 * Drush command and supporting functions.
 */

use GuzzleHttp\Client;

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
      'batch-size' => 'batch-size (Default 100)',
      'reset' => 'reset',
    ),
    'examples' => array(
      'drush sb-dw-queue contact' => 'Queues all users',
      'drush sb-dw-queue action --limit=500' => 'Queues next 500 advocacy actions.',
      'drush sb-dw-queue form --reset --limit=500' => 'Reset highwater mark to zero. Queues 500 forms.',
      'drush sb-dw-queue donation --batch-size=10' => 'Queue all donations, 10 at a time. Defaults to 100. Potentially useful if you run into memory issues.',
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
      'submission' => dt('submission'),
      'donation' => dt('donation'),
      'action' => dt('action'),
      'message' => dt('message'),
      'petition message' => dt('petition message'),
      'payment' => dt('payment'),
    );
    $type = drush_choice($options, dt('What kind of record would you like to queue?'));
  }

  // Special snowflake checks for 'message' type.
  if ($type == 'message') {
    // Exit early if the API endpoint and auth token have not been set.
    if (variable_get('springboard_dw_advocacy_authentication_token') == NULL
          || variable_get('springboard_dw_advocacy_api_endpoint') == NULL
      ) {
      watchdog('data_warehouse', 'API endpoint and/or authentication token
      have not been set.', NULL, WATCHDOG_ERROR, l(t('Fix this on the config page'),
        'admin/config/services/springboard-dw'));
      return drush_set_error(dt('API endpoint and/or authentication token have not been set.'));
    }
    // Exit early if trying to use the --reset option on the 'message' type.
    if (drush_get_option('reset')) {
      return drush_set_error(dt('Type "message" cannot be run with the --reset option, as the highwater mark is not supported via the Advocacy service.'));
    }
  }

  // Get options.
  $limit = drush_get_option('limit', 999999999);
  $batch_size = drush_get_option('batch-size', 100);

  // Reset highwater mark to 0, if option is set.
  if (drush_get_option('reset', FALSE)) {
    springboard_dw_queue_legacy_set_highwater_mark($type, 0);
    drush_log(dt('Resetting highwater mark to zero.'), 'ok');
  }

  // Get the highwater mark for this type so we can pick up where we left off.
  $highwater = springboard_dw_queue_legacy_get_highwater_mark($type);

  // Populate list of records to operate on.
  switch ($type) {
    case 'contact':
        $records = db_query_range('SELECT uid, created FROM {users} WHERE created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_users';
      break;

    case 'donation':
        $records = db_query_range('SELECT did, nid, created FROM {fundraiser_donation} WHERE created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_donations';
      break;

    case 'action':
        $records = db_query_range("SELECT n.nid, n.created, n.type FROM {node} n WHERE (type='springboard_petition' OR type='sba_social_action' OR type='sba_message_action') AND created > :highwater ORDER BY created ASC", 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_actions';
      break;

    case 'message':
        $records = springboard_dw_queue_legacy_get_advocacy_messages();
      $batch_process_function = 'springboard_dw_queue_legacy_messages';
      break;

    case 'petition message':
        $records = db_query_range("SELECT s.sid, s.nid, s.uid, s.submitted AS created FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE n.type = 'springboard_petition' AND s.submitted > :highwater ORDER BY s.submitted ASC", 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_petition_messages';
      break;

    case 'form':
        $records = db_query_range('SELECT n.nid, n.created FROM {node} n INNER JOIN {webform} w ON n.nid = w.nid WHERE n.created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_forms';
      break;

    case 'submission':
      $excluded_types = springboard_dw_get_donation_types();
      $excluded_types[] = 'springboard_petition';
      $excluded_types[] = 'sba_message_action';
      $excluded_types[] = 'sba_social_action';
      $records = db_query_range("SELECT s.sid, s.nid, s.uid, s.remote_addr, s.submitted AS created FROM {webform_submissions} s INNER JOIN {node} n ON s.nid = n.nid WHERE n.type NOT IN (:excluded_types) AND s.submitted > :highwater ORDER BY s.submitted ASC", 0, $limit, array(':highwater' => $highwater, ':excluded_types' => $excluded_types))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_submissions';
      break;

    case 'payment':
        $records = db_query_range('SELECT transaction_id, order_id AS donation_id, remote_id, uid, payment_method, instance_id, currency_code, status, message, remote_status, created, changed FROM {commerce_payment_transaction} WHERE created > :highwater ORDER BY created ASC', 0, $limit, array(':highwater' => $highwater))->fetchAll();
      $batch_process_function = 'springboard_dw_queue_legacy_payments';
      break;

    default:
      return drush_set_error(dt('"@type" is not a valid record type. Nothing was processed.', array('@type' => $type)));
  }

  $record_count = count($records);
  drush_log(dt(':count records found to process.', array(':count' => $record_count)), 'ok');
  if ($record_count < 1) {
    watchdog("data_warehouse", 'Attempted to process %type records when there are :record_count remaining.', array('%type' => $type, ':record_count' => $record_count), WATCHDOG_WARNING);
  }
  // Break up all of our data so each process does not time out.
  $chunks = array_chunk($records, $batch_size);
  $operations = array();

  // For every chunk, assign some method to run on that chunk of data.
  $i = 0;
  foreach ($chunks as $chunk) {
    $i++;
    $operations[] = array($batch_process_function,
                    array($chunk, 'details' => t(
                        '(Queued records @chunk of @count)',
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
  drush_log(dt('Setting highwater mark to %highwater', array('%highwater' => date(DATE_ATOM, $highwater))), 'ok');
  springboard_dw_queue_legacy_set_highwater_mark($type, $highwater);
}

/**
 * Callback for finished batch.
 */
function springboard_dw_queue_legacy_finished_batch($success, $results, $operations) {

  drush_log(dt('Finished queueing'));
}

/**
 * Batch process callback for processing users.
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
      // Log the queue item.
      springboard_dw_log_queue_item($contact_item);
      // Mark the item as queued.
      springboard_dw_track_item_warehouse_queued('contact', $record->uid);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing donations.
 */
function springboard_dw_queue_legacy_donations($records, $operation_details, &$context) {
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($records as $record) {
    $donation = fundraiser_donation_get_donation($record->did);
    // Massage values into format springboard_dw_map_donation_item() expects.
    $donation->donation['referrer'] = $donation->submission_data['referrer'];
    $donation->donation['initial_referrer'] = $donation->submission_data['initial_referrer'];
    $donation->donation['payment_method'] = $donation->submission_data['payment_method'];
    $donation->donation['recurs_monthly'] = $donation->submission_data['recurs_monthly'];
    $donation->donation['search_engine'] = $donation->submission_data['search_engine'];
    $donation->donation['search_string'] = $donation->submission_data['search_string'];
    $donation->donation['user_agent'] = $donation->submission_data['user_agent'];
    $donation->donation['device_type'] = $donation->submission_data['device_type'];
    $donation->donation['device_name'] = $donation->submission_data['device_name'];
    $donation->donation['device_os'] = $donation->submission_data['device_os'];
    $donation->donation['device_browser'] = $donation->submission_data['device_browser'];
    // If $donation->data could not decrypted, just set the expected values to
    // empty string.
    if (!is_array($donation->data)) {
      $donation->data = array();
      $donation->data['payment_fields']['credit']['card_type'] = '';
      $donation->data['payment_fields']['credit']['card_number'] = '';
      $donation->data['payment_fields']['credit']['card_expiration_month'] = '';
      $donation->data['payment_fields']['credit']['card_expiration_year'] = '';
    }
    if (!springboard_dw_item_is_warehoused('donation', $record->did)) {
      $donation_item = springboard_dw_map_donation_item($donation);
      $queue->createItem($donation_item);
      springboard_dw_log_queue_item($donation_item);
      springboard_dw_track_item_warehouse_queued('donation', $record->did);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing webforms.
 */
function springboard_dw_queue_legacy_forms($records, $operation_details, &$context) {
  foreach ($records as $record) {
    $node = node_load($record->nid);
    // Get the Drupal data warehouse queue.
    $queue = DrupalQueue::get("springboard_dw_export");

    // Create the form item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('form', $node->nid)) {
      $form_item = springboard_dw_map_form_item($node);
      $queue->createItem($form_item);
      springboard_dw_log_queue_item($form_item);
      springboard_dw_track_item_warehouse_queued('form', $node->nid);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing payments
 */
function springboard_dw_queue_legacy_payments($records, $operation_details, &$context) {
  // var_dump($records);
  foreach ($records as $record) {
    $queue = DrupalQueue::get("springboard_dw_export");

    // Create the form item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('payment', $record->transaction_id)) {
      $donation['donation_id'] = $record->donation_id;
      $payment['uid'] = $record->uid;
      $payment['payment_method'] = $record->payment_method;
      $payment['instance_id'] = $record->instance_id;
      $payment['remote_id'] = $record->remote_id; // the id returned from the gateway
      $payment['amount'] = $record->amount;
      $payment['currency_code'] = $record->currency_code;
      $payment['status'] = $record->status;
      $payment['message'] = $record->message;
      $payment['remote_status'] = $record->remote_status;
      $payment['created'] = $record->created;
      $payment['changed'] = $record->changed;
      $payment_item = springboard_dw_map_payment_item($payment, $donation);
      $queue->createItem($payment_item);
      springboard_dw_log_queue_item($payment_item);
      springboard_dw_track_item_warehouse_queued('payment', $payment['transaction_id']);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing webform submissions.
 *
 * Submissions for content types 'donation_form', 'springboard_petition',
 * sba_social_action', 'sba_message_action' should not be passed to this
 * function per the DW schema.
 */
function springboard_dw_queue_legacy_submissions($records, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  foreach ($records as $record) {
    $node = node_load($record->nid);
    $submission = webform_get_submission($record->nid, $record->sid);
    // If submission is false, the webform_submitted_data table was probably
    // cleared out at some point. So, let's populate a $submission object with
    // the data we have available from the webform_submissions table.
    if (!$submission) {
      $submission = new stdClass();
      $submission->sid = $record->sid;
      $submission->nid = $record->nid;
      $submission->uid = $record->uid;
      $submission->submitted = $record->created;
      $submission->remote_addr = $record->remote_addr;
    }
    if (is_array($submission->data)) {
      $submitted_data = array();
      // Put the submission data in key/value pair format.
      foreach ($submission->data as $cid => $submittted_value) {
        if (array_key_exists($cid, $node->webform['components'])) {
          $submitted_data[$node->webform['components'][$cid]['form_key']] = $submission->data[$cid]['value'][0];
        }
      }
    }
    if (!springboard_dw_item_is_warehoused('submission', $node->nid)) {
      $submission_item = springboard_dw_map_submission_item($submission, $submitted_data);
      // Create the queue item.
      $queue->createItem($submission_item);
      springboard_dw_log_queue_item($submission_item);
      springboard_dw_track_item_warehouse_queued('submission', $record->sid);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing actions.
 */
function springboard_dw_queue_legacy_actions($actions, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");

  foreach ($actions as $action) {
    $node = node_load($action->nid);

    $action = new stdClass();
    $action->node = $node;
    // None of these appear to be relvant for actions themselves, only their
    // associated messages, which we process separately.
    // $action->contact = $contact;
    // $action->data = $data;
    // $action->webform = $webform_submission_data;
    // $action->sid = $sid;.
    // Message action and social action.
    if ($node->type == 'sba_message_action' || $node->type == 'sba_social_action') {
      // Create the Advocacy action item in the queue if not warehoused.
      if (!springboard_dw_item_is_warehoused('advocacy action', $node->nid)) {
        $action_item = springboard_dw_map_advocacy_action_item($action);
        $queue->createItem($action_item);
        springboard_dw_log_queue_item($action_item);
        springboard_dw_track_item_warehouse_queued('advocacy action', $node->nid);
      }
    }

    // Petition.
    if ($node->type == 'springboard_petition') {
      if (!springboard_dw_item_is_warehoused('petition', $node->nid)) {
        $action_item = springboard_dw_map_petition_item($action);
        $queue->createItem($action_item);
        springboard_dw_log_queue_item($action_item);
        springboard_dw_track_item_warehouse_queued('petition', $node->nid);
      }
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing petitions.
 */
function springboard_dw_queue_legacy_petition_messages($submissions, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($submissions as $submission) {
    $action = springboard_dw_queue_legacy_build_action($submission);
    // Create the Advocacy action item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('advocacy petition message', $action->node->nid)) {
      $petition_item = springboard_dw_map_petition_submission_item($action);
      $queue->createItem($petition_item);
      springboard_dw_log_queue_item($petition_item);
      springboard_dw_track_item_warehouse_queued('advocacy petition message', $node->nid);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Batch process callback for processing advocacy messages.
 */
function springboard_dw_queue_legacy_messages($messages, $operation_details, &$context) {
  // Get the Drupal data warehouse queue.
  $queue = DrupalQueue::get("springboard_dw_export");
  foreach ($messages as $message) {
    // Create the Advocacy action item in the queue if not warehoused.
    if (!springboard_dw_item_is_warehoused('advocacy message', $message['id'])) {
      $message_item = array();
      $message_item['queue_type'] = 'advocacy message';
      $message_item['message_id'] = $message['id'];
      $message_item['action_id'] = $message['action_id'];
      $queue->createItem($message_item);
      springboard_dw_log_queue_item($message_item);
      springboard_dw_track_item_warehouse_queued('advocacy message', $message['id']);
    }
  }
  $context['message'] = $operation_details;
}

/**
 * Builds action object.
 *
 *  Build the action object by re-creating the data that is available during
 *  hook_advocacy_success().
 */
function springboard_dw_queue_legacy_build_action($submission) {
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
 * Get list of advocacy message node IDs.
 *
 * @return array
 *   Array of all message values.
 */
function springboard_dw_queue_legacy_get_advocacy_messages() {
  // Getnids of all sba_message_action nodes.
  $advocacy_action_records = db_query("SELECT nid FROM {node} WHERE type = 'sba_message_action' OR type = 'sba_social_action' ORDER BY created ASC")->fetchAll();
  $messages = array();
  foreach ($advocacy_action_records as $advocacy_action_record) {
    $node = node_load($advocacy_action_record->nid);
    $messages = array_merge($messages, springboard_dw_queue_legacy_get_advocacy_message_details($node->advocacy_id));

  }
  return $messages;
}

/**
 * Get advocacy messages related to a single action.
 *
 * Loops through all available pages to get entire dataset.
 *
 * @return array
 *   Return an array of messages and pagination details.
 */
function springboard_dw_queue_legacy_get_advocacy_message_details($action_id) {
  $details = array();
  $current_page = 1;
  $last_page = 2;
  while ($current_page <= $last_page) {
    drush_log('Getting page ' . $current_page . ' of data for ' . $action_id, 'ok');
    $page = springboard_dw_queue_legacy_get_advocacy_message_details_page($action_id, $current_page);
    $current_page = $page['current_page'] + 1;
    $last_page = $page['last_page'];
    foreach ($page['data'] as $message) {
      $details[] = array(
        'id' => $message['_id']['$id'],
        'action_id' => $action_id,
        'created' => $message['delivery']['createdAt']['sec'],
      );
    }
  }
  return $details;
}

/**
 * GET a single page of JSON data.
 *
 * @return array
 *   Return an array of message data for a specified pagination offset.
 */
function springboard_dw_queue_legacy_get_advocacy_message_details_page($action_id, $page = 1) {
  // Composer Vendor autoload
  if (function_exists('drush_autoload')) {
    drush_autoload("sites/all/libraries/springboard_composer/vendor/autoload.php");
  }
  else {
    require_once("sites/all/libraries/springboard_composer/vendor/autoload.php");
  }

  // Load Guzzle client.
  $guzzle = new Client(['http_errors' => FALSE]);
  $params = array(
    'query' => array(
      'access_token' => variable_get('springboard_dw_advocacy_authentication_token'),
      'page' => $page,
    ),
  );
  $endpoint = variable_get('springboard_dw_advocacy_api_endpoint') . '/api/v1/messages/get/' . $action_id;
  $response = $guzzle->request('GET', $endpoint, $params);
  $body = $response->getBody();
  return json_decode($body->getContents(), TRUE);
}
