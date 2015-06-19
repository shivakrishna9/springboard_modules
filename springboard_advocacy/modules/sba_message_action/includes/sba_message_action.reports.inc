<?php

/**
 * Callback handler for 'node/%/performance' menu item.
 *
 * Generates and outputs deliverability reports for a particular message action.
 */
function sba_message_action_performance_page($nid) {
  // Load up the message action node.
  $node = node_load($nid);

  // Get an instance of the advocacy API client.
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();

  // TODO: Probably need to wrap this call in sort of error handling.
  $report = $client->getTargetDeliverability($node->advocacy_id);

  // Use the devel module if you want to use dsm(). Calling that will output
  // the raw API response data to the screen so you can examine it.
  dsm($report);

  // Check the status of the API call return.
  if (isset($report->error)) {
    // TODO: Ensure the message displayed to the user makes sense.
    return $report->error->message;
  }
  else if (isset($report->data)) {
    // TODO: Bad variable name.
    $target_deliverablity = sba_message_action_performance_format_targets($report->data->targets);

    // This will be the header row for the HTML table.
    $header = array('Name', 'Party', 'State', 'District', 'Total Messages Sent', 'Delivered Messages', 'Failed Messages', 'Deliverablity Index');

    // TODO: This is crap. Generate the output using a theme function and proper Drupal conventions. The classes are bootstap 2.x
    // which ships with the Springboard admin theme so they're ok to use. The style attributes should be created via drupal_add_css.
    // Could potentially move this to it's own function that accepts $report->data.
    $output = '<h2>Overall Deliverability</h2><div class="container"><div class="row">
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->totalMessages.'</h2>Total Messages</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->delivered.'</h2>Delivered</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->failed.'</h2>Failed</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->deliverabilityIndex.'</h2>Deliverablity Index</div></div>
</div></div>';

    $output .= '<h2>Individual Target Deliverability</h2>';

    // This runs the data through Drupal's built-in theme_table function to create an HTML table.
    $output .= theme('table', array('header' => $header, 'rows' => $target_deliverablity));
    return $output;

  }
  else {
    // TODO: Probably don't need this else if API call is wrapped in error handling.
    return 'uh oh';
  }



}

/**
 * Helper function to convert target deliverablity report data into
 * a data structure that can be rendered in an HTML table.
 */
function sba_message_action_performance_format_targets($targets) {
  $rows = array();
  foreach ($targets as $target) {
    $rows[] = array(
      $target->salutation . ' ' . $target->first_name . ' ' . $target->last_name,
      $target->party,
      $target->state,
      $target->district_code,
      $target->totalMessages,
      $target->delivered,
      $target->failed,
      $target->deliverabilityIndex // TODO: Format the deliverablity index as a percentage.
      //sprintf("%.f%%", $target->deliverabilityIndex)
    );
  }

  return $rows;
}