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
  // CSS assets
  drupal_add_css(drupal_get_path('module', 'sba_message_action') . '/css/message-performance.css');

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

  $deliverablity_targets = sba_message_action_performance_format_targets($report->data->targets, $node->advocacy_id);

  return deliverability_report($report, $deliverablity_targets);
}

/**
 * Helper function to convert target deliverability report data into
 * a data structure that can be rendered in an HTML table.
 */
function sba_message_action_performance_format_targets($targets, $nodeId)
{

  $rows = array();

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

  return $rows;
}

/**
 * Render the deliverability report to a template.
 * @param $report
 * @param $deliverablity_targets
 * @return string
 * @throws \Exception
 */
function deliverability_report($report, $deliverablity_targets)
{

  // Define the table header
  $header = array('Name', 'Party', 'State', 'District', 'Total Messages Sent', 'Delivered Messages', 'Failed Messages', 'Deliverablity Index');

  // Uses sba_message_action_theme()
  $output = theme('messsage_deliverability_dashboard', array(
    'header'  => $header,
    'targets' => $deliverablity_targets,
    'report'  => $report
  ));

  return $output;

}