<?php
/**
 * @file
 * Provideas methods to work with sustaienr log snapshot data.
 */

/**
 * Class FundraiserSustainersInsights
 */
class FundraiserSustainersInsights {

  /**
   * Get the snapshot data for today.
   *
   * @return FundraiserSustainersDailySnapshot
   *   The snapshot instance for the today.
   */
  public function getTodaysSnapshot() {
    return $this->getSnapshot(new DateTime());
  }

  /**
   * Get the snapshot for a given day.
   *
   * @param DateTime $time
   *   The day for the desired snapshot.
   *
   * @return FundraiserSustainersDailySnapshot
   *   The snapshot instance for the date given.
   */
  public function getSnapshot(DateTime $time) {
    return new FundraiserSustainersDailySnapshot($time);
  }

  /**
   * Get a forecast report based on a relative time string.
   *
   * Examples include '7 days', '1 month' and most other formats that DateTime
   * will accept. This method prepends the '+' so that it will only forecast
   * future dates. Defaults to the next 7 days if the string is not valid.
   *
   * @param string $string
   *   The string to construct a DateTime instance. Gets prepended with '+'.
   *
   * @return FundraiserSustainersForecast
   *   A forecast report instance for the time range.
   */
  public function getForecastPreset($string) {
    $string = '+' . $string;
    $begin = new DateTime();

    try {
      $end = new DateTime($string);
    }
    catch (Exception $e) {
      // Default to future 7 days if we can't figure out what the string is.
      $end = new DateTime('+7 days');
    }

    return $this->getForecast($begin, $end);
  }

  /**
   * Get a historical report for a given relative time range.
   *
   * For example, '7 days', '2 weeks', '1 month'. Defaults to the past 7 days
   * if the string is not valid.
   *
   * @param string $string
   *   A string for DateTime to convert to a date object.
   *
   * @return FundraiserSustainersSnapshotReport
   *   A historical report instance for the time range.
   */
  public function getHistoricalReportPreset($string, $range) {

    try {
      $date1 = new DateTime($string);
    }
    catch (Exception $e) {
      // Default to past 7 days if we can't figure out what the string is.
      $date1 = new DateTime();
    }

    // Clone the date2 datetime and then attempt to modify it by the range.
    $date2 = clone $date1;
    try {
      $date2->modify($range);
    }
    catch (Exception $e) {
      // Default to past 7 days if we can't figure out what the string is.
      $date2->modify('-7 days');
    }

    return $this->getHistoricalReport($date2, $date1);
  }

  /**
   * Get a historical report that spans a specific date range.
   *
   * @param DateTime $begin
   *   The beginning date.
   * @param DateTime $end
   *   The end date.
   *
   * @return FundraiserSustainersHistoricalReport
   *   The historical report instance.
   */
  public function getHistoricalReport(DateTime $begin, DateTime $end) {
    return new FundraiserSustainersHistoricalReport($begin, $end);
  }

  /**
   * Get a forecast report that spans a specific date range.
   *
   * @param DateTime $begin
   *   The beginning date.
   * @param DateTime $end
   *   The end date.
   *
   * @return FundraiserSustainersForecast
   *   The forecast report instance.
   */
  public function getForecast(DateTime $begin, DateTime $end) {
    return new FundraiserSustainersForecast($begin, $end);
  }
}
