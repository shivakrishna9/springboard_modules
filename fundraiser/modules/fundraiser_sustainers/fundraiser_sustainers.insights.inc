<?php
/**
 * @file
 * Page callbacks for Fundraiser Sustainer Insights.
 */

function fundraiser_sustainers_insights_dashboard() {
  $insights = new FundraiserSustainersInsights();
  $today = $insights->getTodaysSnapshot();
  $range = '7 days';
  $date_format = 'n/j/Y';

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

  $label_data = array();
  $successes_data = array();
  $failures_data = array();
  foreach ($report_snapshots as $date => $snapshot) {
    $friendly_date = $snapshot->getDate()->format($date_format);

    $row = array(
      $friendly_date,
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

    $label_data[] = $friendly_date;
    $successes_data[] = $snapshot->getSuccesses();
    $failures_data[] = $snapshot->getFailures();

    $rows[] = $row;
  }

  $table_variables = array(
    'header' => $headers,
    'rows' => $rows,
  );

  $forecast_labels = array();
  $forecast_data = array();
  $forecast_report = $insights->getForecastPreset($range);
  $future_snapshots = $forecast_report->getSnapshots();
  foreach ($future_snapshots as $date => $snapshot) {
    $forecast_labels[] = $snapshot->getDate()->format($date_format);
    $forecast_data[] = $snapshot->getScheduledCharges();
  }

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
    'chart_labels' => $chart_labels,
    'successes_data' => $chart_successes_data,
    'failure_data' => $chart_failure_data,
    'forecast_labels' => json_encode($forecast_labels),
    'forecast_data' => json_encode($forecast_data),
  );

  $path = drupal_get_path('module', 'fundraiser_sustainers');
  drupal_add_css($path . '/css/fundraiser_sustainers.insights.css');
  drupal_add_js($path . '/js/Chart.js');

  return theme('fundraiser_sustainers_insights_dashboard', $variables);
}
