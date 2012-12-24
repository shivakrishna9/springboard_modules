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
  protected $items = array();

  /**
   * Array of groups of items that can be sent to Salesforce simultaneously
   *
   * @var array
   */
  protected $batches = array();

  /**
   * The configured maximum number of items that can be processed at once
   *
   * This depends upon the system resources
   *
   * @var int
   */
  protected $max_items = 1000;

  /**
   * The configured default time the queue will be leased to our process
   *
   * @var int
   */
  protected $lease_time = 3600;

  /**
   * Unique identifier that allows this process to unlock the lease on the queue
   *
   * @var string
   */
  protected $lease_key;

   /**
   * The maximum number of items that can be sent to Salesforce in one call
   *
   * @var int
   */
  protected $salesforce_max = 200;

  /**
   * Salesforce object to connect with
   *
   * @var Salesforce
   */
  protected $sfapi;

  /**
   * Defines a default response to the end of the processing
   *
   * @var int
   */
  protected $end_status = SALESFORCE_QUEUE_PROCESS_NOT_STARTED;

  /**
   * A processor object for the given queue
   *
   * @param SalesforceQueue $queue
   *   The desired queue to process against, or NULL to process against the
   *   multi-queue manager
   *
   * @return SalesforceQueueProcessor
   *   A processor for the given queue
   */
  public static function get($queue = NULL) {
    $queue = empty($queue) ? SalesforceQueue::get() : $queue;
    $processor = new SalesforceQueueProcessor($queue);
    return $processor;
  }

  /**
   * @param SalesforceQueue $queue
   */
  public function __construct(SalesforceQueue $queue) {
    $this->queue = $queue;
    $this->max_items = variable_get('salesforce_queue_max_items', 1000);
    $this->lease_time = variable_get('salesforce_queue_lease_item', 3600);
    $this->salesforce_max = variable_get('salesforce_queue_salesforce_max', 200);
  }

  /**
   * Kicks off processing up-to the maximum allowed items from the given queue
   */
  public function process() {
    // Invokes hook_salesforce_queue_preprocess_queue().
    module_invoke_all('salesforce_queue_preprocess_queue', $this->queue);

    // Check Salesforce connection
    $this->sfapi = salesforce_get_api(); // always refresh
    if (!$this->sfapi->isAuthorized()) {
      // Cannot connect to Salesforce
      watchdog('salesforce_queue', 'Could not process SalesforceQueue because '
        . 'I cannot connect to Salesforce.', array(), WATCHDOG_ALERT);
      $this->end_status = SALESFORCE_QUEUE_PROCESS_FAIL_CONNECT;
    }
    else {
      // Gather items to process
      $this->claimItems();
      if (!empty($this->items)) {
        // Invokes hook_salesforce_queue_process_items_alter().
        drupal_alter('salesforce_queue_process_items', $this->items, $this->queue);
      }

      // Sort the items into a set of logical batches that can be sent off to
      // Salesforce
      $this->createBatches();
    }

    if ($this->end_status == SALESFORCE_QUEUE_PROCESS_NOT_STARTED) {
      // We got to the end without anyone else reporting anything!
      $end_status = SALESFORCE_QUEUE_PROCESS_COMPLETE;
    }
    else {
      $end_status = $this->end_status;
    }

    $this->resetInternalState();

    // Invokes hook_salesforce_queue_postprocess_queue().
    module_invoke_all('salesforce_queue_postprocess_queue', $this->queue, $end_status);

    return $end_status;
  }

  /**
   * Claim a batch of items to process
   */
  public function claimItems() {
    // If there are still unprocessed items, lets finish those first
    if (empty($this->items)) {
      // Claim some items
      $items = $this->queue->claimBatch($this->max_items, $this->lease_time);

      // SalesforceQueue::claimBatch() returns FALSE if there are no more items
      // or when the queue is currently leased
      if (empty($items)) {
        // Is the queue leased?
        if ($this->queue->queueIsLeased()) {
          // The process was run while the queue is already leased.  This could
          // be user error, or could be a sign that processes are piling up due
          // to some sort of conflict with server resources or connections with
          // Salesforce, or the configuration settings about how much to process
          // or even how often we're trying to run this process.
          $message = 'SalesforceQueueProcess was run, but could not claim '
            . 'items because the queue was leased. This could be a minor issue '
            . 'or more severe issue so it would warrant you looking into this.';
          $severity = WATCHDOG_ERROR;
          $this->end_status = SALESFORCE_QUEUE_PROCESS_FAIL_QUEUE_LEASED;
        }
        else if ($this->queue->numberOfItems() < 1) {
          // No items are in the queue.  Pretty minor issue.
          $message = 'SalesforceQueueProcess was run, but no items were found '
            . 'in the queue to process.  Not much for you to worry about.';
          $severity = WATCHDOG_DEBUG;
          $this->end_status = SALESFORCE_QUEUE_PROCESS_NO_ITEMS;
        }
        else {
          // If we get here, then we're really clueless as to what is going on
          $message = 'SalesforceQueueProcess was run, but we could not claim '
            . 'items for an unknown reason.  Eeps.';
          $severity = WATCHDOG_ALERT;
          $this->end_status = SALESFORCE_QUEUE_PROCESS_FAIL_CLAIM_ITEMS;
        }
        watchdog('salesforce_queue', $message, array(), $severity);
      }
      else {
        $this->lease_key = $items[0]->lease_key;
        $this->items = $items;
      }
    }
    return $this->items;
  }

  /**
   * Group the items into batches that can be sent to Salesforce in one call
   *
   * A batch of items are defined as items that can be sent to Salesforce in
   * one call. They can if they match the following requirements:
   *   - Same Salesforce object type
   *   - Same operation
   *     - If operation is UPSERT, they have the same dedupe field
   *   - No more than 200 items
   *
   * @NOTE There may be an additional complication when considering row-by-row
   *       comparision.  A field map may have individual fields check against
   *       Salesforce to see if it should write/overwrite/ignore the submitted
   *       value when sending to Salesforce
   */
  public function createBatches() {
    // Break down the items by object->operation->dedupe
    $sort = array();
    foreach ($this->items as $key => $item) {
      // Adjust just in case an UPSERT command falls through without a dedupe
      if ($item->operation == 'UPSERT' && empty($item->dedupe_field)) {
        $item->operation = 'CREATE';
      }

      // Handle special case UPSERT where they need to share dedupe field
      if ($item->operation == 'UPSERT') {
        $sort[$item->salesforce_type][$item->operation][$item->dedupe_field][] = $item;
      }
      else {
        $sort[$item->salesforce_type][$item->operation][] = $item;
      }
      // Remove items as they are processed
      unset($this->items[$key]);
    }

    // Sort the broken down items and build up the batches
    uksort($sort, '_sfq_sort_objects');
    foreach($sort as $object => $operations) {
      uksort($sort[$object], '_sfq_sort_operations');

      foreach ($sort[$object] as $op => $items) {
        // Generate a batch item
        $batch = array(
          'object' => $object,
          'operation' => $op,
        );
        if ($op == 'UPSERT') {
          foreach ($items as $dedupe => $items2) {
            $batch['dedupe'] = $dedupe;
            // Break up by max number and add to batches
            $chunks = array_chunk($items2, $this->salesforce_max);
            foreach ($chunks as $chunk){
              $batch['items'] = $chunk;
              $this->batches[] = $batch;
            }
          }
        }
        else {
          // Break up by max number and add to batches
          $chunks = array_chunk($items, $this->salesforce_max);
          foreach ($chunks as $chunk){
            $batch['items'] = $chunk;
            $this->batches[] = $batch;
          }
        }
      }
    }

    return $this->batches;
  }

  /**
   * Eat yum yums
   */
  private function eatBatch() {
    // nom nom nom
  }

  /**
   * Release our claim on the queue and reset our internal state
   */
  private function resetInternalState() {
    $this->items = array();
    $this->batches = array();
    $this->end_status = SALESFORCE_QUEUE_PROCESS_NOT_STARTED;
    return $this->queue->releaseBatch($this->lease_key);
  }
}

/**
 * uksort() function to sort keys by user configurable Salesforce object type
 */
function _sfq_sort_objects($a, $b) {
  // @TODO set default object order
  $objects = variable_get('salesforce_queue_object_order', array('Account', 'Contact', 'Opportunity'));
  $object_order = array_flip($objects);
  if ($object_order[$a] == $object_order[$b]) {
    return 0;
  }
  return ($object_order[$a] < $object_order[$b]) ? -1 : 1; // lower is higher priority
}

/**
 * uksort() function to sort keys by Salesforce operation
 */
function _sfq_sort_operations($a, $b) {
  $op_order = array(
    'CREATE' => 0,
    'UPSERT' => 1,
    'UPDATE' => 2,
    'DELETE' => 3,
  );
  if ($op_order[$a] == $op_order[$b]) {
    return 0;
  }
  return ($op_order[$a] < $op_order[$b]) ? -1 : 1; // lower is higher priority
}
