<?php

/**
 * @file
 * Contains SalesforceQueueProcessor
 */

/**
 * Processes items from SalesforceQueue
 */
class SalesforceQueueProcessor {

  /**
   * SalesforceQueue object
   *
   * @var object
   */
  protected $queue;

  /**
   * Batch of items from the queue that need to be processed
   *
   * @var array
   */
  protected $items;

  /**
   * Kicks off processing items from the given queue
   *
   * @param SalesforceQueue $queue
   */
  public static function process(SalesforceQueue $queue) {
    $this->queue = $queue;

    // Invokes hook_salesforce_queue_preprocess_queue().
    module_invoke_all('salesforce_queue_preprocess_queue', $queue);

    // @TODO all of the processing
    if (empty($this->items)) {
      $this->claimBatch();
    }

    // Invokes hook_salesforce_queue_postprocess_queue().
    module_invoke_all('salesforce_queue_postprocess_queue', $queue);

    return $this;
  }

  /**
   * Claim a batch of items to process
   *
   * @param int $quantity
   *   Number of items to claim from the queue
   * @param int $lease_time
   *   The amount of time you believe it will take to process the batch.  If your
   *   process takes longer, another process could take the items.
   */
  public function claimBatch($quantity = 200, $lease_time = 3600) {
    $items = $this->queue->claimBatch($quantity, $lease_time);
    if (empty($this->items)) {
      $this->items = $items;
    }
    else {
      $this->items += $items;
    }
  }
}
