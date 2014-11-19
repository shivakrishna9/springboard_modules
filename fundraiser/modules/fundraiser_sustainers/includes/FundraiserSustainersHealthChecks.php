<?php
/**
 * @file
 * Contains sustainer health checks.
 */

class FundraiserSustainersHealthChecks {

  /**
   * @var array
   *   Each array item is an array with keys that match params for watchdog().
   */
  protected $issues = array();

  /**
   * Runs all of the available sustainer checks.
   *
   * Currently only looks for stuck sustainers.
   *
   * @return array
   *   The resulting array of issues. Can be iterated over and provided to
   *   watchdog(), or used for any other purpose, such as the status report.
   */
  public function runChecks() {
    // Reset the issues in case this isn't the first run.
    $this->issues = array();

    $this->checkStuckSustainers();

    return $this->issues;
  }

  /**
   * Looks for stuck sustainers and adds an issue if any are found.
   *
   * @return array
   *   The issue generated by this check, if any.
   *   Otherwise an empty array.
   */
  public function checkStuckSustainers() {
    $stuck_sustainers = $this->getStuckDids();

    $issue = array();
    if (count($stuck_sustainers)) {
      $issue = array(
        'type' => 'fundraiser_sustainers',
        'message' => 'The following sustainer donations are locked but not processed: %dids',
        'variables' => array('%dids' => implode(', ', $stuck_sustainers)),
        'severity' => WATCHDOG_CRITICAL,
        'link_text' => t('Report'),
        'link_url' => 'admin/springboard/reports/reconciliation/locked-not-processed-sustainers',
      );
      $this->issues[] = $issue;
    }

    return $issue;
  }

  /**
   * Queries for stuck sustainers.
   *
   * A "stuck" sustainer means its lock id is non-zero and gateway response
   * is not null, 'success', or 'failed' and the number of attempts is not 3.
   *
   * This usually means that a sustainer was marked to process it, but was not
   * updated with a response, for whatever reason.
   *
   * @return NULL|array
   *   An array of donation IDs.
   */
  protected function getStuckDids() {
    $query = "SELECT did FROM {fundraiser_sustainers} WHERE lock_id != 0 AND attempts != 3 AND (gateway_resp NOT IN ('success', 'failed') OR gateway_resp IS NULL)";

    return db_query($query)->fetchCol();
  }
}
