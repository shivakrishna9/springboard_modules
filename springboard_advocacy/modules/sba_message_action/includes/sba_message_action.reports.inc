<?php

function sba_message_action_performance_page($nid) {
  $node = node_load($nid);

  //dsm($node);

  // Send submission to API Client
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();

  $report = $client->getTargetDeliverability($node->advocacy_id);

  //dsm($report);

  if (isset($report->error)) {
    return $report->error->message;
  }
  else if (isset($report->data)) {
    $target_deliverablity = sba_message_action_performance_format_targets($report->data->targets);
    $header = array('Name', 'Party', 'State', 'District', 'Total Messages Sent', 'Delivered Messages', 'Failed Messages', 'Deliverablity Index');

    $output = '<h2>Overall Deliverability</h2><div class="container"><div class="row">
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->totalMessages.'</h2>Total Messages</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->delivered.'</h2>Delivered</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->failed.'</h2>Failed</div></div>
  <div class="span3" style="text-align:center"><div class="well"><h2 class="text-info" style="font-size:35px;">'.$report->data->deliverabilityIndex.'</h2>Deliverablity Index</div></div>
</div></div>';

    $output .= '<h2>Individual Target Deliverability</h2>';
    $output .= theme('table', array('header' => $header, 'rows' => $target_deliverablity));
    return $output;

  }
  else {
    return 'uh oh';
  }



}

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
      $target->deliverabilityIndex
      //sprintf("%.f%%", $target->deliverabilityIndex)
    );
  }

  return $rows;
}