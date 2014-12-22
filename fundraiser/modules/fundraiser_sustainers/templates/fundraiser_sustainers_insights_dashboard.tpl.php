<h2>Today's Snapshot <?php print $today ?></h2>

<div class="row">
  <div class="insights-today-box span3">
    <h3>Scheduled Charges</h3>
    <div class="big-metric"><?php print $scheduled_charges ?></div>
    <div class="foot">View donations</div>
  </div>

  <div class="insights-today-box span3">
    <h3>Total Value</h3>
    <div class="big-metric"><?php print $total_value ?></div>
    <div class="foot"></div>
  </div>

  <div class="insights-today-box span3">
    <h3>Successes</h3>
    <div class="big-metric"><?php print $successes ?></div>
    <div class="foot">View log</div>
  </div>

  <div class="insights-today-box span3">
    <h3>Failures</h3>
    <div class="big-metric"><?php print $failures ?></div>
    <div class="foot">View failures</div>
  </div>
</div>
<hr>

<div class="insights-historical-report">
  <h3>Last <?php print $range ?></h3>
  <?php print $historical_report_table ?>
</div>

<div class="insights-forecast-report">
  <h3>Next <?php print $range ?></h3>
  <?php print $forecast_report_table ?>
</div>

<div class="insights-processing-stats-report">
  <h3>Processing Stats</h3>
  <?php print $processing_stats_table ?>
</div>
