<?php

/**
 * Class FundraiserSustainersInsights
 */
class FundraiserSustainersInsights {

  /**
   *
   */
  public function __construct() {

  }

  public function timeframeOptions() {
    return array(
      '7 days' => t('Last 7 days'),
      '30 days' => t('Last 30 days'),
      '60 days' => t('Last 60 days'),
    );
  }

  /**
   * Record data for a snapshot on the given day.
   *
   * This might get removed.
   *
   * @param DateTime $time
   */
  public function takeSnapshot(DateTime $time) {
    return new FundraiserSustainersDailySnapshot($time);
  }

  /**
   * @return FundraiserSustainersDailySnapshot
   */
  public function getTodaysSnapshot() {
    return new FundraiserSustainersDailySnapshot(new DateTime());
  }

  /**
   * @param DateTime $time
   *
   * @return FundraiserSustainersDailySnapshot
   */
  public function getSnapshot(DateTime $time) {
    return new FundraiserSustainersDailySnapshot($time);
  }

  /**
   * @param $string
   *
   * @return FundraiserSustainersForecast
   */
  public function getForecastPreset($string) {
    $string = '+' . $string;
    $begin = new DateTime();

    try {
      $end = new DateTime($string);
    }
    catch (Exception $e) {
      // Default to past 7 days if we can't figure out what the string is.
      $end = new DateTime('+7 days');
    }

    return $this->getForecast($begin, $end);
  }

  /**
   * @param $string
   *
   * @return FundraiserSustainersSnapshotReport
   */
  public function getHistoricalReportPreset($string) {
    $string = '-' . $string;
    $end = new DateTime();

    try {
      $begin = new DateTime($string);
    }
    catch (Exception $e) {
      // Default to past 7 days if we can't figure out what the string is.
      $begin = new DateTime('-7 days');
    }

    return $this->getHistoricalReport($begin, $end);
  }

  /**
   * @param DateTime $begin
   * @param DateTime $end
   *
   * @return FundraiserSustainersHistoricalReport
   */
  public function getHistoricalReport(DateTime $begin, DateTime $end) {
    return new FundraiserSustainersHistoricalReport($begin, $end);
  }

  /**
   * @param DateTime $begin
   * @param DateTime $end
   *
   * @return FundraiserSustainersForecast
   */
  public function getForecast(DateTime $begin, DateTime $end) {
    return new FundraiserSustainersForecast($begin, $end);
  }
}
