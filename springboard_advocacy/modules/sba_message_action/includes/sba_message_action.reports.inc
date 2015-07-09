<?php

/**
 * Get an instance of the API client to use in API calls.
 * @return \SpringboardAdvocacyAPIClient
 */
function springboard_advocacy_message_action_api_client()
{

  $apiClient = &drupal_static(__FUNCTION__);

  if (!isset($apiClient['client'])) {

    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();

    $apiClient['client'] = $client;
  }

  return $apiClient['client'];
}

/**
 * Callback handler for 'node/%/performance' menu item.
 *
 * Generates and outputs deliverability reports for a particular message action.
 */
function sba_message_action_performance_page($nid)
{
  // CSS and JS assets
  drupal_add_css(drupal_get_path('module', 'sba_message_action') . '/css/message-performance.css');
  drupal_add_js(drupal_get_path('module', 'sba_message_action') . '/js/jsfile.js');

  // Load up the message action node.
  $node = node_load($nid);

  // API Client instance
  $client = springboard_advocacy_message_action_api_client();

  // Try to make the API call and if it fails return a response.
  try {
    $report = $client->getTargetDeliverability($node->advocacy_id);

  } catch (Exception $e) {
    watchdog('advocacy', 'An error occurred when attempting to get target deliverability metrics.', array('@message' => $e), WATCHDOG_ERROR);

    return 'Oops! An invalid API response was encountered.';
  }

  // Check the status of the API call return and if it's an error return that to the browser.
  if (isset($report->error)):
    return 'Sorry, no performance metrics are available for this message.';
  endif;

  $deliverablity_targets = sba_message_action_performance_targets_table($report, $node);

  // dsm($node->advocacy_id);

  return $deliverablity_targets;
}

/**
 * Helper function to convert target deliverability report data into
 * a data structure that can be rendered in an HTML table.
 */
function sba_message_action_performance_targets_table($report, $node)
{

  $targets = $report->data->targets;

  $rows = array();

  $nodeId = $node->advocacy_id;

  // Get an instance of the advocacy API client.
  $client = springboard_advocacy_message_action_api_client();

  foreach ($targets as $target) {

    // Get the target deliverability metrics for this target
    $targetDeliverability = $client->getSingleTargetDeliverability($nodeId, $target->id)->data;

    $rows[] = array(
      $target->salutation . ' ' . $target->first_name . ' ' . $target->last_name,
      $target->party,
      $target->state,
      $target->district_code,
      $targetDeliverability->totalMessages,
      $targetDeliverability->delivered,
      $targetDeliverability->failed,
      $targetDeliverability->deliverabilityIndexPercentage
    );
  }

  // Define the table header
  $header = array('Name', 'Party', 'State', 'District', 'Total Messages Sent', 'Delivered Messages', 'Failed Messages', 'Deliverability Index');

  $queueReadyItems = $client->getQueueMessagesCountByStatus($nodeId, 'ready')->data->count;
  $queuePausedItems = $client->getQueueMessagesCountByStatus($nodeId, 'paused')->data->count;
  $queueCanceledItems = $client->getQueueMessagesCountByStatus($nodeId, 'canceled')->data->count;

  $tabs = theme('messsage_deliverability_tabs', array(
    'nodeId' => $node->nid
  ));

  $output = theme('messsage_deliverability_dashboard', array(
    'targets_table_header' => $header,
    'targets'              => $rows,
    'report'               => $report,
    'queueReadyItems'      => $queueReadyItems,
    'queuePausedItems'     => $queuePausedItems,
    'queueCanceledItems'   => $queueCanceledItems,
    'nodeId'               => $node->nid,
    'tabs'                 => $tabs
  ));

  return $output;
}

/**
 * Implements hook_preprocess()
 * @param $variables
 * @throws \Exception
 */
function sba_message_action_preprocess_messsage_deliverability_dashboard(&$variables)
{
  // Sanitize the targets data
  foreach ($variables['targets'] as $key => $data) {
    foreach ($data as $itemKey => $value) {
      $variables['targets'][$key][$itemKey] = check_plain($value);
    }
  }

  // Sanitize report data
  $variables['report']->data->totalMessages = check_plain($variables['report']->data->totalMessages);
  $variables['report']->data->delivered = check_plain($variables['report']->data->delivered);
  $variables['report']->data->failed = check_plain($variables['report']->data->failed);
  $variables['report']->data->deliverabilityIndexPercentage = check_plain($variables['report']->data->deliverabilityIndexPercentage);

  // Render targets table
  $targetsTable = theme('table', array('header' => $variables['targets_table_header'], 'rows' => $variables['targets']));

  // dsm($targetsTable);

  $variables['targets_table'] = $targetsTable;
}

