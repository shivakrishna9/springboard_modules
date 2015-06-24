<h2>Overall Deliverability</h2>

<div class="container deliverability-performance-container">
  <div class="row">
    <div class="span3">
      <div class="well">
        <h2 class="text-info"><?php print $report->data->totalMessages; ?></h2>
        Total Messages
      </div>
    </div>
    <div class="span3">
      <div class="well">
        <h2 class="text-info"><?php print $report->data->delivered; ?></h2>
        Delivered
      </div>
    </div>
    <div class="span3">
      <div class="well">
        <h2 class="text-info"><?php print $report->data->failed; ?></h2>
        Failed
      </div>
    </div>
    <div class="span3">
      <div class="well">
        <h2 class="text-info"><?php print $report->data->deliverabilityIndexPercentage; ?></h2>
        Deliverablity
      </div>
    </div>
  </div>
</div>
<h2>Individual Target Deliverability</h2>

<?php print $targets_table; ?>