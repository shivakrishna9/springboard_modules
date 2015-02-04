<?php
/**
 * @file
 * Page callbacks for Fundraiser Sustainer Insights.
 */

/**
 * Page callback for the Sustainer Insights dashboard page.
 *
 * @param string $path_range
 *   An optional path argument to specify a date range.
 *
 * @return string
 *   Themed HTML.
 * @throws Exception
 */
function fundraiser_sustainers_insights_dashboard($path_range = 'last-7-days') {

  $insights = new FundraiserSustainersInsights();
  $today = $insights->getTodaysSnapshot();
  $date_format = 'n/j/Y';

  switch ($path_range) {
    case 'last-7-days':
      $range = '7 days';
      $historical_report = $insights->getHistoricalReportPreset($range);
      $historical_report_heading = 'Last 7 days';
      $future_range = '7 days';
      break;

    case 'last-30-days':
      $range = '30 days';
      $historical_report = $insights->getHistoricalReportPreset($range);
      $historical_report_heading = 'Last 30 days';
      $future_range = '30 days';
      break;

    case 'last-month':
      $begin = new DateTime('first day of last month midnight');
      $end = new DateTime('first day of this month midnight');
      $historical_report = $insights->getHistoricalReport($begin, $end);
      $historical_report_heading = 'Last month';

      $future_range = '30 days';
      break;

    case 'this-month':
      $begin = new DateTime('first day of this month midnight');
      $end = new DateTime('first day of next month midnight');
      $historical_report = $insights->getHistoricalReport($begin, $end);
      $historical_report_heading = 'This month';
      $future_range = '30 days';
      break;

    default:
      return MENU_NOT_FOUND;
  }

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
  $date_report_path = 'springboard/reports/sustainers/activity/';
  foreach ($report_snapshots as $date => $snapshot) {
    $friendly_date = $snapshot->getDate()->format($date_format);

    $row = array(
      l($friendly_date, $date_report_path . $date),
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
  $forecast_report = $insights->getForecastPreset($future_range);
  $forecast_report_heading = 'Next ' . $future_range;
  $future_snapshots = $forecast_report->getSnapshots();
  foreach ($future_snapshots as $date => $snapshot) {
    $forecast_labels[] = $snapshot->getDate()->format($date_format);
    $forecast_data[] = $snapshot->getScheduledCharges();
  }

  $url_date = $today->getDate()->format('Y-m-d');

  $variables = array(
    'time_range_links' => theme('links', fundraiser_sustainers_insights_dashboard_links()),
    'today' => $today->getDate()->format('d M Y'),
    'scheduled_charges' => $today->getScheduledCharges(),
    'today_scheduled_path' => 'springboard/reports/sustainers/scheduled/' . $url_date,
    'total_value' => '$ ' . number_format($today->getTotalValue() / 100, 2),
    'successes' => $today->getSuccesses(),
    'today_success_path' => 'springboard/reports/sustainers/success/' . $url_date,
    'failures' => $today->getFailures(),
    'today_failure_path' => 'springboard/reports/sustainers/failure/' . $url_date,
    'historical_report_heading' => $historical_report_heading,
    'historical_report_table' => theme('table', $table_variables),
    'forecast_report_heading' => $forecast_report_heading,
    'forecast_labels' => json_encode($forecast_labels),
    'forecast_data' => json_encode($forecast_data),
    'chart_labels' => json_encode(array_reverse($label_data)),
    'successes_data' => json_encode(array_reverse($successes_data)),
    'failure_data' => json_encode(array_reverse($failures_data)),
  );

  $path = drupal_get_path('module', 'fundraiser_sustainers');
  drupal_add_css($path . '/css/fundraiser_sustainers.insights.css');
  drupal_add_js($path . '/js/Chart.js');
  drupal_add_js($path . '/js/jquery.equalheights.js');
  drupal_add_js($path . '/js/fundraiser_sustainers.insights.js');

  return theme('fundraiser_sustainers_insights_dashboard', $variables);
}

/**
 * Creates the links for time range.
 *
 * @return array
 *   The links render array for theming.
 */
function fundraiser_sustainers_insights_dashboard_links() {
  $base = 'springboard/reports/sustainers/insights';

  return array(
    'links' => array(
      'week' => array(
        'title' => 'Last week',
        'href' => $base,
      ),
      'last-month' => array(
        'title' => 'Last month',
        'href' => $base . '/last-month',
      ),
      'this-month' => array(
        'title' => 'This month',
        'href' => $base . '/this-month',
      ),
    ),
    'heading' => 'Time Range',
  );
}
