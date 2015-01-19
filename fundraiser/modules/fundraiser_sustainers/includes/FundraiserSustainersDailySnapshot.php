<?php
/**
 * @file
 * A class to represent a single day's worth of sustainer log data.
 */

/**
 * Class FundraiserSustainersDailySnapshot
 */
class FundraiserSustainersDailySnapshot {

  /**
   * @var DateTime
   *   Today. Used for some calculations and display.
   */
  protected $today;

  /**
   * @var DateTime
   *   The day of this Snapshot.
   */
  protected $date;

  /**
   * @var int
   *   The timestamp of the day start.
   *
   * Used for database queries.
   */
  protected $beginTimestamp;

  /**
   * @var int
   *   The timestamp of the day start of the following day.
   *
   * Used for database queries.
   */
  protected $endTimestamp;

  /**
   * @var int
   *   Number of charges scheduled, including processed.
   */
  protected $scheduledCharges;

  /**
   * @var int
   *   Value (in cents) of charges scheduled, including processed.
   */
  protected $scheduledValue;

  /**
   * @var int
   *   Number of charges that were retried on this day.
   */
  protected $retriedCharges;

  /**
   * @var int
   *   Value (in cents) retried on this day.
   */
  protected $retriedValue;

  /**
   * @var int
   *   Number of charges processed.
   */
  protected $processedCharges;

  /**
   * @var int
   *   Value (in cents) processed.
   */
  protected $processedValue;

  /**
   * @var int
   *   Number of charges that are rescheduled after being processed on this day.
   */
  protected $rescheduledCharges;

  /**
   * @var int
   *   Value (in cents) rescheduled after being processed.
   */
  protected $rescheduledValue;

  /**
   * @var int
   *   Number of charges that permanently failed after being processed.
   */
  protected $abandonedCharges;

  /**
   * @var int
   *   Value (in cents) that permanently failed.
   */
  protected $abandonedValue;

  /**
   * @var int
   *   Number of successful charges completed so far in this day.
   */
  protected $successes;

  /**
   * @var int
   *   Value (in cents) of successful charges completed.
   */
  protected $successValue;

  /**
   * @var int
   *   Number of failed charges attempted so far in this day.
   */
  protected $failures;

  /**
   * @var int
   *   Value (in cents) of failed charges attempted.
   */
  protected $failureValue;

  /**
   * Constructs a snapshot sets up some values, and loads data.
   *
   * @param DateTime $date
   *   The date of the desired snapshot.
   */
  public function __construct(DateTime $date) {
    $this->date = $date;
    $this->date->setTime(0, 0, 0);

    $this->beginTimestamp = $this->date->getTimestamp();

    $date_end = clone $this->date;
    $date_end->add(new DateInterval('P1D'));
    $date_end->setTime(0, 0, 0);
    $this->endTimestamp = $date_end->getTimestamp();

    $this->today = new DateTime();
    $this->today->setTime(0, 0, 0);

    $this->load();
  }

  /**
   * Get the DateTime instance for this snapshot.
   *
   * @return DateTime
   *   The DateTime for this snapshot data.
   */
  public function getDate() {
    return $this->date;
  }

  /**
   * Get the scheduled charges.
   *
   * @return int
   *   Number of charges scheduled.
   */
  public function getScheduledCharges() {
    return $this->scheduledCharges;
  }

  /**
   * Get the scheduled value.
   *
   * @return int
   *   Value in cents.
   */
  public function getScheduledValue() {
    return $this->scheduledValue;
  }

  /**
   * Total value in the UI is the same thing as scheduled value.
   *
   * @return int
   *   Value in cents.
   */
  public function getTotalValue() {
    return $this->getScheduledValue();
  }

  /**
   * Get the number of successfully processed sustainers for this report date.
   *
   * @return int
   *   Count of successes.
   */
  public function getSuccesses() {
    return $this->successes;
  }

  /**
   * Get the number of sustainers that failed to process for this report date.
   *
   * Includes temporary and permanent failures.
   *
   * @return int
   *   Count of failures.
   */
  public function getFailures() {
    return $this->failures;
  }

  /**
   * Get the count of charges that permanently failed while processing.
   *
   * @return int
   *   Count of sustainers.
   */
  public function getAbandonedCharges() {
    return $this->abandonedCharges;
  }

  /**
   * Get the value of the abandoned sustainers.
   *
   * @return int
   *   Value in cents.
   */
  public function getAbandonedValue() {
    return $this->abandonedValue;
  }

  /**
   * Get the total number of sustainers processed on this report date.
   *
   * @return int
   *   Count of charges processed.
   */
  public function getProcessedCharges() {
    return $this->getSuccesses() + $this->getFailures();
  }

  /**
   * Get the value of of the sustainers processed on this report date.
   *
   * @return int
   *   Value in cents.
   */
  public function getProcessedValue() {
    return $this->getSuccessValue() + $this->getFailureValue();
  }

  /**
   * Get the value of the successfully processed sustainer on this report date.
   *
   * @return int
   *   Value in cents.
   */
  public function getSuccessValue() {
    return $this->successValue;
  }

  /**
   * Get the value of the sustainers that failed processing on this report date.
   *
   * Includes both temporary and permanent failures.
   *
   * @return int
   *   Value in cents.
   */
  public function getFailureValue() {
    return $this->failureValue;
  }

  /**
   * Get the charges that failed on this report date but will be retried.
   *
   * @return int
   *   Count of sustainers.
   */
  public function getRescheduledCharges() {
    return $this->rescheduledCharges;
  }

  /**
   * Get the value of charges failed on this report date but will be retried.
   *
   * @return int
   *   Value in cents.
   */
  public function getRescheduledValue() {
    return $this->rescheduledValue;
  }

  /**
   * Get the count of charges that previously failed processing.
   *
   * @return int
   *   Count of sustainers.
   */
  public function getRetriedCharges() {
    return $this->retriedCharges;
  }

  /**
   * Get the value of retried charges.
   *
   * @return int
   *   Value in cents.
   */
  public function getRetriedValue() {
    return $this->retriedValue;
  }

  /**
   * Populates the snapshot instance with data.
   */
  protected function load() {
    $this->initializeValues();
    $this->calculateValues();
  }

  /**
   * Calculates and sets the properties for this snapshot instance.
   */
  protected function calculateValues() {
    $this->calculateScheduledProperties();
    $this->calculateOtherProperties();
  }

  /**
   * Gets the order amount from a donation/order.
   *
   * Does not handle currency code. Assumes everything is in cents.
   *
   * @param int $did
   *   Donation/order ID.
   *
   * @return int
   *   The value in cents.
   */
  protected function getValueFromOrder($did) {
    $order = commerce_order_load($did);
    $wrapper = entity_metadata_wrapper('commerce_order', $order);

    return $wrapper->commerce_order_total->amount->value();
  }

  /**
   * Calculates the scheduled charges and value.
   */
  protected function calculateScheduledProperties() {
    // success, canceled, skipped, failed, retry, processing, NULL
    $replacements = array(
      ':begin' => $this->beginTimestamp,
      ':end' => $this->endTimestamp,
    );

    $query = "SELECT did FROM {fundraiser_sustainers_log} WHERE (new_state = 'scheduled' OR (new_state = 'retry' AND old_state = 'processing')) AND next_charge >= :begin AND next_charge < :end";

    $result = db_query($query, $replacements);

    $count = 0;
    $total = 0;
    foreach ($result as $row) {
      $count++;
      $total += $this->getValueFromOrder($row->did);
    }

    $this->scheduledCharges = $count;
    $this->scheduledValue = $total;
  }

  /**
   * Calculates all other properties for the snapshot.
   */
  protected function calculateOtherProperties() {
    $successes = 0;
    $failures = 0;
    $successValue = 0;
    $failureValue = 0;
    $retriedCharges = 0;
    $retriedValue = 0;
    $rescheduledCharges = 0;
    $rescheduledValue = 0;
    $abandonedCharges = 0;
    $abandonedValue = 0;

    $replacements = array(
      ':begin' => $this->beginTimestamp,
      ':end' => $this->endTimestamp,
    );
    // Get all log events that happened on this day.
    // Ordering these so the new transitions get checked first, and earlier
    // ones get ignored if there are multiple transitions per day.
    // @todo Is that a good idea?
    $results = db_query("SELECT did, old_state, new_state FROM {fundraiser_sustainers_log} WHERE timestamp >= :begin AND timestamp < :end ORDER BY lid DESC", $replacements);

    foreach ($results as $row) {

      // Sustainer was born and at best is going into locking and processing.
      // Otherwise the charge might be getting advanced or changed.
      if (is_null($row->old_state)) {
        continue;
      }

      // Special case, master donation.
      if ($row->old_state == 'success' && $row->new_state == 'success') {
        continue;
      }

      // Get the amount value from the related commerce order.
      $value = $this->getValueFromOrder($row->did);
      if ($row->old_state == 'processing') {

        if ($row->new_state == 'success') {
          $successes++;
          $successValue += $value;
        }
        else {
          $failures++;
          $failureValue += $value;

          if ($row->new_state == 'retry') {
            $rescheduledCharges++;
            $rescheduledValue += $value;
          }
          elseif ($row->new_state == 'failed') {
            $abandonedCharges++;
            $abandonedValue += $value;
          }
        }
      }
      elseif ($row->old_state == 'retry' && $row->new_state == 'processing') {
        $retriedCharges++;
        $retriedValue += $value;
      }
    }

    $this->successes = $successes;
    $this->failures = $failures;
    $this->successValue = $successValue;
    $this->failureValue = $failureValue;

    $this->retriedCharges = $retriedCharges;
    $this->retriedValue = $retriedValue;
    $this->rescheduledCharges = $rescheduledCharges;
    $this->rescheduledValue = $rescheduledValue;
    $this->abandonedCharges = $abandonedCharges;
    $this->abandonedValue = $abandonedValue;
  }

  /**
   * Sets initial values to zero.
   */
  protected function initializeValues() {
    $this->scheduledCharges = 0;
    $this->scheduledCharges = 0;
    $this->successes = 0;
    $this->failures = 0;
    $this->successValue = 0;
    $this->failureValue = 0;

    $this->abandonedCharges = 0;
    $this->abandonedValue = 0;
    $this->rescheduledCharges = 0;
    $this->rescheduledValue = 0;
    $this->processedCharges = 0;
    $this->processedValue = 0;
    $this->retriedCharges = 0;
    $this->retriedValue = 0;
  }
}
