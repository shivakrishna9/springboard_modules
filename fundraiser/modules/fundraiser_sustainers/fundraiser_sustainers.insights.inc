<?php
/**
 * @file
 * Page callbacks for Fundraiser Sustainer Insights.
 */

function fundraiser_sustainers_insights_dashboard() {
  $insights = new FundraiserSustainersInsights();
  $today = $insights->getTodaysSnapshot();
  $range = '7 days';

  $historical_report = $insights->getHistoricalReportPreset($range);
  $report_snapshots = $historical_report->getSnapshots();

  $headers = array(
    array('data' => ''),
    array('data' => 'Scheduled', 'colspan' => 2),
    array('data' => 'Retried', 'colspan' => 2),
    array('data' => 'Processed', 'colspan' => 2),
    array('data' => 'Rescheduled', 'colspan' => 2),
    array('data' => 'Abandoned', 'colspan' => 2),
  );

  $rows = array();

  $first_row = array(
    'data' => array(
      array('data' => 'Date', 'header' => TRUE),
      array('data' => '#', 'header' => TRUE),
      array('data' => '$', 'header' => TRUE),
      array('data' => '#', 'header' => TRUE),
      array('data' => '$', 'header' => TRUE),
      array('data' => '#', 'header' => TRUE),
      array('data' => '$', 'header' => TRUE),
      array('data' => '#', 'header' => TRUE),
      array('data' => '$', 'header' => TRUE),
      array('data' => '#', 'header' => TRUE),
      array('data' => '$', 'header' => TRUE),
    ),
    'no_striping' => TRUE,
    'class' => 'subhead_row',
  );

  $rows[] = $first_row;

  $processing_stats_rows = array();
  $label_data = array();
  $successes_data = array();
  $failures_data = array();
  foreach ($report_snapshots as $date => $snapshot) {
    $row = array(
      $date,
      $snapshot->getScheduledCharges(),
      $snapshot->getScheduledValue() / 100,
      $snapshot->getRetriedCharges(),
      $snapshot->getRetriedValue() / 100,
      $snapshot->getProcessedCharges(),
      $snapshot->getProcessedValue() / 100,
      $snapshot->getRescheduledCharges(),
      $snapshot->getRescheduledValue() / 100,
      $snapshot->getAbandonedCharges(),
      $snapshot->getAbandonedValue() / 100,
    );

    $processing_stats_row = array(
      $date,
      $snapshot->getSuccesses(),
      $snapshot->getFailures(),
    );

    $label_data[] = $date;
    $successes_data[] = $snapshot->getSuccesses();
    $failures_data[] = $snapshot->getFailures();

    $processing_stats_rows[] = $processing_stats_row;

    $rows[] = $row;
  }

  $table_variables = array(
    'header' => $headers,
    'rows' => $rows,
  );

  $processing_stats_table_variables = array(
    'header' => array('Date', 'Successes', 'Failures'),
    'rows' => $processing_stats_rows,
  );

  $forecast_headers = array(
    'Date',
    'Scheduled #',
    'Scheduled $',
  );

  $forecast_rows = array();

  $forecast_report = $insights->getForecastPreset($range);

  $future_snapshots = $forecast_report->getSnapshots();

  foreach ($future_snapshots as $date => $snapshot) {
    $row = array(
      $date,
      $snapshot->getScheduledCharges(),
      $snapshot->getScheduledValue() / 100,
    );

    $forecast_rows[] = $row;
  }

  $forecast_table_variables = array(
    'header' => $forecast_headers,
    'rows' => $forecast_rows,
  );

  $chart_labels = json_encode(array_reverse($label_data));
  $chart_successes_data = json_encode(array_reverse($successes_data));
  $chart_failure_data = json_encode(array_reverse($failures_data));

  $variables = array(
    'today' => $today->getDate()->format('d M Y'),
    'range' => $range,
    'scheduled_charges' => $today->getScheduledCharges(),
    'total_value' => '$ ' . number_format($today->getTotalValue() / 100, 2),
    'successes' => $today->getSuccesses(),
    'failures' => $today->getFailures(),
    'historical_report_table' => theme('table', $table_variables),
    'forecast_report_table' => theme('table', $forecast_table_variables),
    'processing_stats_table' => theme('table', $processing_stats_table_variables),
    'chart_labels' => $chart_labels,
    'successes_data' => $chart_successes_data,
    'failure_data' => $chart_failure_data,
  );

  $path = drupal_get_path('module', 'fundraiser_sustainers');
  drupal_add_css($path . '/css/fundraiser_sustainers.insights.css');
  drupal_add_js($path . '/js/Chart.js');

  return theme('fundraiser_sustainers_insights_dashboard', $variables);
}
