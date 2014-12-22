<?php

/**
 * Class FundraiserSustainersDailySnapshot
 */
class FundraiserSustainersDailySnapshot {

  /**
   * @var DateTime
   * Today.
   */
  protected $today;

  /**
   * @var DateTime
   * The day of this Snapshot.
   */
  protected $date;

  /**
   * @var int
   * The timestamp of the day start. Used for database queries.
   */
  protected $beginTimestamp;

  /**
   * @var int
   * The timestamp of the day start of the following day. Used for database
   * queries.
   */
  protected $endTimestamp;

  /**
   * @var bool
   */
  protected $isNew;

  /**
   * @var bool
   */
  protected $isComplete;

  /**
   * @var int
   * Timestamp when this was last saved.
   */
  protected $lastUpdated;

  /**
   * @var int
   * Number of charges scheduled, including processed.
   */
  protected $scheduledCharges;

  /**
   * @var int
   * Value (in cents) charges scheduled, including processed.
   */
  protected $scheduledValue;

  /**
   * @var int
   */
  protected $retriedCharges;

  /**
   * @var int
   */
  protected $retriedValue;

  /**
   * @var int
   */
  protected $processedCharges;

  /**
   * @var int
   */
  protected $processedValue;

  /**
   * @var int
   */
  protected $rescheduledCharges;

  /**
   * @var int
   */
  protected $rescheduledValue;

  /**
   * @var int
   */
  protected $abandonedCharges;

  /**
   * @var int
   */
  protected $abandonedValue;

  /**
   * @var int
   * Number of successful charges completed so far in this day.
   */
  protected $successes;

  /**
   * @var int
   * Number of failed charges attempted so far in this day.
   */
  protected $failures;

  protected $successValue;

  protected $failureValue;

  /**
   * @param DateTime $date
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

    $this->isNew = TRUE;
    $this->isComplete = FALSE;

    $this->load();
  }

  /**
   *
   */
  public function save() {
    $this->lastUpdated = REQUEST_TIME;

    // Save to the database.
    $this->saveRow();
  }

  /**
   * @return DateTime
   */
  public function getDate() {
    return $this->date;
  }

  /**
   * @return int
   */
  public function getScheduledCharges() {
    return $this->scheduledCharges;
  }

  /**
   * @return int
   */
  public function getScheduledValue() {
    return $this->scheduledValue;
  }

  /**
   *
   * @return int
   */
  public function getTotalValue() {
    return $this->getScheduledValue();
  }

  /**
   * @return int
   */
  public function getSuccesses() {
    return $this->successes;
  }

  /**
   * @return int
   */
  public function getFailures() {
    return $this->failures;
  }

  /**
   * @return int
   */
  public function getAbandonedCharges() {
    return $this->abandonedCharges;
  }

  /**
   * @return int
   */
  public function getAbandonedValue() {
    return $this->abandonedValue;
  }

  /**
   * @return int
   */
  public function getProcessedCharges() {
    return $this->getSuccesses() + $this->getFailures();
  }

  /**
   * @return int
   */
  public function getProcessedValue() {
    return $this->getSuccessValue() + $this->getFailureValue();
  }

  public function getSuccessValue() {
    return $this->successValue;
  }

  public function getFailureValue() {
    return $this->failureValue;
  }

  /**
   * @return int
   */
  public function getRescheduledCharges() {
    return $this->rescheduledCharges;
  }

  /**
   * @return int
   */
  public function getRescheduledValue() {
    return $this->rescheduledValue;
  }

  /**
   * @return int
   */
  public function getRetriedCharges() {
    return $this->retriedCharges;
  }

  /**
   * @return int
   */
  public function getRetriedValue() {
    return $this->retriedValue;
  }

  /**
   * @return bool
   */
  protected function shouldUseLiveData() {
    return $this->today->format('Y-m-d') == $this->date->format('Y-m-d');
  }

  /**
   *
   */
  protected function load() {
    $this->initializeValues();
    $save = FALSE;
    // Look for a record in the DB and load it.
    // If there's no existing record, calculate new values.
    $row = $this->findRow();

    if (is_object($row)) {
      $this->isNew = FALSE;
      $this->loadRow($row);
    }

    if (!$this->isComplete || $this->shouldUseLiveData()) {
      $this->calculateValues();
      $save = TRUE;

      if ($this->endTimestamp <= $this->today->getTimestamp()) {
        $this->isComplete = TRUE;
      }
    }

    if ($save) {
      $this->save();
    }
  }

  protected function loadRow($row) {
    $this->lastUpdated = $row->last_updated;
    $this->isComplete = $row->complete;
    $this->scheduledCharges = $row->scheduled_charges;
    $this->scheduledValue = $row->scheduled_value;
    $this->successes = $row->successes;
    $this->successValue = $row->success_value;
    $this->failures = $row->failures;
    $this->failureValue = $row->failure_value;
    $this->retriedCharges = $row->retried_charges;
    $this->retriedValue = $row->retried_value;
    $this->rescheduledCharges = $row->rescheduled_charges;
    $this->rescheduledValue = $row->rescheduled_value;
    $this->abandonedCharges = $row->abandoned_charges;
    $this->abandonedValue = $row->abandoned_value;
  }

  /**
   * Set up a record array for drupal_write_record.
   */
  protected function saveRow() {

    $record = array(
      'date' => $this->getDate()->format('Y-m-d'),
      'last_updated' => $this->lastUpdated,
      'complete' => $this->isComplete,
      'scheduled_charges' => $this->scheduledCharges,
      'scheduled_value' => $this->scheduledValue,
      'successes' => $this->successes,
      'success_value' => $this->successValue,
      'failures' => $this->failures,
      'failure_value' => $this->failureValue,
      'retried_charges' => $this->retriedCharges,
      'retried_value' => $this->retriedValue,
      'rescheduled_charges' => $this->rescheduledCharges,
      'rescheduled_value' => $this->rescheduledValue,
      'abandoned_charges' => $this->abandonedCharges,
      'abandoned_value' => $this->abandonedValue,
    );

    if ($this->isNew) {
      drupal_write_record('fundraiser_sustainers_insights_snapshot', $record);
    }
    else {
      drupal_write_record('fundraiser_sustainers_insights_snapshot', $record, 'date');
    }
  }

  /**
   * @return array
   */
  protected function findRow() {
    // Query for rows by the Y-m-d date string.
    return db_query("SELECT * FROM {fundraiser_sustainers_insights_snapshot} WHERE date = :date", array(':date' => $this->getDate()->format('Y-m-d')))->fetchObject();
  }

  /**
   *
   */
  protected function calculateValues() {
    // Do the DB queries and math stuff here.
    $this->calculateScheduledProperties();

    $this->calculateOtherProperties();
  }

  protected function getValueFromOrder($did) {
    $order = commerce_order_load($did);
    $wrapper = entity_metadata_wrapper('commerce_order', $order);
//      $currency_code = $wrapper->commerce_order_total->currency_code->value();

    return $wrapper->commerce_order_total->amount->value();
  }

  protected function calculateScheduledProperties() {
    // success, canceled, skipped, failed, retry, processing, NULL
    $replacements = array(
      ':end' => $this->endTimestamp,
    );

    if ($this->shouldUseLiveData()) {
      // When using live data, we're counting both unprocessed and processed.
      // Later on we remove processed where the next charge is before begin.
      $query = "SELECT DISTINCT * FROM {fundraiser_sustainers} WHERE next_charge < :end AND gateway_resp NOT IN ('canceled', 'skipped')";
    }
    else {
      // When doing past or future date lookups, we tie it to the date range.
      $query = "SELECT DISTINCT did FROM {fundraiser_sustainers_revision} WHERE next_charge >= :begin AND next_charge < :end AND gateway_resp NOT IN ('canceled', 'skipped')";
      $replacements[':begin'] = $this->beginTimestamp;
    }

    $result = db_query($query, $replacements);

    $count = 0;
    $total = 0;
    foreach ($result as $row) {

      if ($this->shouldUseLiveData()) {

        if (in_array($row->gateway_resp, array('success', 'failed')) && $row->next_charge < $this->beginTimestamp) {
          continue;
        }

      }

      $count++;
      $total += $this->getValueFromOrder($row->did);
    }

    $this->scheduledCharges = $count;
    $this->scheduledValue = $total;
  }

  protected function calculateOtherProperties() {
    $successes = 0;
    $failures = 0;
    $processedDids = array();
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
    // Get all revisions that were created on this day.
    // Ordering these so the new transitions get checked first, and earlier
    // ones get ignored if there are multiple transitions per day.
    // @todo Is that a good idea?
    $results = db_query("SELECT revision_id, master_did, did, revision_timestamp, attempts, gateway_resp FROM {fundraiser_sustainers_revision} WHERE revision_timestamp >= :begin AND revision_timestamp < :end ORDER BY revision_timestamp DESC", $replacements);

    foreach ($results as $new) {
      // Skip it if we've seen this donation id before.
      if (in_array($new->did, $processedDids)) {
        continue;
      }

      $replacements = array(
        ':did' => $new->did,
        ':revision_id' => $new->revision_id,
        ':attempts' => $new->attempts,
      );

      // Get the preceding revision that would indicate processing happened.
      $old = db_query("SELECT revision_id, master_did, did, revision_timestamp, attempts, gateway_resp FROM {fundraiser_sustainers_revision} WHERE did = :did AND revision_id < :revision_id AND attempts < :attempts ORDER BY revision_id DESC LIMIT 0,1", $replacements)->fetchObject();

      if (is_object($old)) {

        // Normalize the gateway response.
        if (is_null($old->gateway_resp)) {
          $old->gateway_resp = '';
        }
        if (is_null($new->gateway_resp)) {
          $new->gateway_resp = '';
        }

        // Get the amount value from the related commerce order.
        $value = $this->getValueFromOrder($old->did);

        if ($old->gateway_resp != 'success' && $new->gateway_resp == 'success') {
          $successes++;
          $successValue += $value;
        }
        elseif (in_array($new->gateway_resp, array('retry', 'failed'))) {
          $failures++;
          $failureValue += $value;
        }

        if ($old->gateway_resp == 'retry') {
          $retriedCharges++;
          $retriedValue += $value;
        }

        if ($new->gateway_resp == 'retry') {
          $rescheduledCharges++;
          $rescheduledValue += $value;
        }
        elseif ($new->gateway_resp == 'failed') {
          $abandonedCharges++;
          $abandonedValue += $value;
        }

        $processedDids[] = $old->did;
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
   *
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
