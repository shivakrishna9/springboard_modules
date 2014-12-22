<?php

/**
 * Class FundraiserSustainersSnapshotReport
 */
class FundraiserSustainersSnapshotReport {

  /**
   * @var DateTime
   */
  protected $begin;

  /**
   * @var DateTime
   */
  protected $end;

  /**
   * @var FundraiserSustainersDailySnapshot[]
   */
  protected $snapshots;

  /**
   *
   * @param DateTime $begin
   * @param DateTime $end
   */
  public function __construct(DateTime $begin, DateTime $end) {
    // @todo Throw an Exception if begin is after end.
    // @todo Throw an Exception if the range is too big, beyond some threshold.

    $this->begin = $begin;
    $this->end = $end;

    $this->loadSnapshots();
  }

  /**
   * @return DateTime
   */
  public function getBegin() {
    return $this->begin;
  }

  /**
   * @return DateTime
   */
  public function getEnd() {
    return $this->end;
  }

  /**
   * @return FundraiserSustainersDailySnapshot[]
   */
  public function getSnapshots() {
    return $this->snapshots;
  }

  /**
   *
   */
  protected function loadSnapshots() {
    $interval = DateInterval::createFromDateString('1 day');
    $period = new DatePeriod($this->begin, $interval, $this->end);

    foreach ($period as $date) {
      $this->snapshots[$date->format("Y-m-d")] = new FundraiserSustainersDailySnapshot($date);
    }
  }

}
