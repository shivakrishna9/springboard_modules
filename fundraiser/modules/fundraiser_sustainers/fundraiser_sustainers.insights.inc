<?php
/**
 * @file
 * Page callbacks for Fundraiser Sustainer Insights.
 */

/**
 * Page callback for the Sustainer Insights dashboard page.
 *
 * @param string $date
 *   A string that can be translated into a date.
 *  @param string $path_range
 *   An optional path argument to specify a date range.
 *
 * @return string
 *   Themed HTML.
 */
function fundraiser_sustainers_insights_dashboard($date = '', $path_range = 'last-7-days') {

  $insights = new FundraiserSustainersInsights();
  $today = $insights->getTodaysSnapshot();
  $date_format = 'n/j/Y';
  $url_date_format = 'Y-m-d';
  $today_date = new DateTime();

  // When no date is supplied use today.
  if (empty($date)) {
    $date = date($url_date_format);
  }

  switch ($path_range) {
    case 'last-7-days':
      $range = '-7 days';
      $historical_report = $insights->getHistoricalReportPreset($date, $range);
      $historical_report_heading = 'Last 7 days';
      $future_range = '7 days';
      break;

    default:
      return MENU_NOT_FOUND;
  }

  $start_date = $historical_report->getBegin();
  $end_date = $historical_report->getEnd();

  // If the end day is not today change the heading.
  if ($today_date->format($date_format) != $end_date->format($date_format)) {
    $historical_report_heading = '7 days from ' . $start_date->format($date_format);
  }

  $report_snapshots = $historical_report->getSnapshots();

  $headers = array(
    array('data' => ''),
    array('data' => 'Scheduled', 'colspan' => 2),
    array('data' => 'Retried', 'colspan' => 2),
    array('data' => 'Successful', 'colspan' => 2),
    array('data' => 'Failed/Rescheduled', 'colspan' => 2),
    array('data' => 'Failed/Abandoned', 'colspan' => 2),
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
    'class' => array('subhead_row'),
  );

  $rows[] = $first_row;

  $label_data = array();
  $successes_data = array();
  $failures_data = array();
  $date_report_path = 'springboard/reports/sustainers/activity/';

  foreach ($report_snapshots as $date => $snapshot) {
    $friendly_date = $snapshot->getDate()->format($date_format);

    $currency_sign = '$';

    $row = array(
      'data' => array(
        l($friendly_date, $date_report_path . $date),
        $snapshot->getScheduledCharges(),
        $currency_sign . number_format($snapshot->getScheduledValue() / 100, 2),
        $snapshot->getRetriedCharges(),
        $currency_sign . number_format($snapshot->getRetriedValue() / 100, 2),
        $snapshot->getSuccessfulCharges(),
        $currency_sign . number_format($snapshot->getSuccessfulValue() / 100, 2),
        $snapshot->getFailedRescheduledCharges(),
        $currency_sign . number_format($snapshot->getFailedRescheduledValue() / 100, 2),
        $snapshot->getFailedAbandonedCharges(),
        $currency_sign . number_format($snapshot->getFailedAbandonedValue() / 100, 2),
      ),
    );

    if ($snapshot->shouldWarn()) {
      $row['class'] = array('warning');
    }

    $label_data[] = $friendly_date;
    $successes_data[] = $snapshot->getSuccessfulCharges();
    $failures_data[] = $snapshot->getFailedCharges();

    $rows[] = $row;
  }

  $table_variables = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'class' => array('table'),
    ),
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

  $url_date = $today->getDate()->format($url_date_format);

  // The previous link should be linked to the day before this report starts.
  $previous_date = clone $start_date;
  $previous_date->modify('-1 day');
  $previous_link = l('<< previous', 'admin/springboard/reports/sustainers/insights/' . $previous_date->format($url_date_format));

  // The next link should be linked to seven days after this report ends.
  $next_date = clone $end_date;
  $next_date->modify('+7 days');
  $next_link = l('next >>', 'admin/springboard/reports/sustainers/insights/' . $next_date->format($url_date_format));

  $variables = array(
    'today' => $today->getDate()->format('d M Y'),
    'scheduled_charges' => $today->getScheduledCharges(),
    'today_scheduled_path' => 'springboard/reports/sustainers/scheduled/' . $url_date,
    'total_value' => '$ ' . number_format($today->getScheduledValue() / 100, 2),
    'successes' => $today->getSuccessfulCharges(),
    'today_success_path' => 'springboard/reports/sustainers/success/' . $url_date,
    'failures' => $today->getFailedCharges(),
    'today_failure_path' => 'springboard/reports/sustainers/failure/' . $url_date,
    'historical_report_heading' => $historical_report_heading,
    'historical_report_table' => theme('table', $table_variables),
    'forecast_report_heading' => $forecast_report_heading,
    'forecast_labels' => json_encode($forecast_labels),
    'forecast_data' => json_encode($forecast_data),
    'chart_labels' => json_encode(array_reverse($label_data)),
    'successes_data' => json_encode(array_reverse($successes_data)),
    'failure_data' => json_encode(array_reverse($failures_data)),
    'previous_link' => $previous_link,
    'next_link' => $next_link,
  );

  $path = drupal_get_path('module', 'fundraiser_sustainers');
  drupal_add_css($path . '/css/fundraiser_sustainers.insights.css');
  drupal_add_js($path . '/js/Chart.js');
  drupal_add_js($path . '/js/jquery.equalheights.js');
  drupal_add_js($path . '/js/fundraiser_sustainers.insights.js');

  return theme('fundraiser_sustainers_insights_dashboard', $variables);
}
