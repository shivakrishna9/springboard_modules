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
   * Name of the SalesforceQueue this processor is processing
   *
   * @var string
   */
  public $queue_name;

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
   *  A running log of the new records created in Salesforce
   *
   * @var array
   */
  public $new_sf_records = array();

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
   * The time at which the current claim on the queue will expire
   *
   * @var int
   */
  protected $lease_expire = 0;

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
   * SalesforceSoapPartner object to connect to Salesforce via SOAP
   *
   * @var SalesforceSoapPartner
   */
  protected $soap;

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
    $this->queue_name = $queue->queue_name;
    $this->max_items = variable_get('salesforce_queue_max_items', 1000);
    $this->lease_time = variable_get('salesforce_queue_lease_item', 3600);
    $this->salesforce_max = variable_get('salesforce_queue_salesforce_max', 200);
    $this->sfapi = salesforce_get_api();
    $this->soap = new SalesforceSoapPartner($this->sfapi);
  }

  /**
   * Kicks off processing up-to the maximum allowed items from the given queue
   */
  public function process() {
    // Invokes hook_salesforce_queue_preprocess_queue().
    module_invoke_all('salesforce_queue_preprocess_queue', $this->queue);

    // Check Salesforce connection
    $this->sfapi = $this->sfapi->isAuthorized() ? $this->sfapi : salesforce_get_api();
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

        // Sort the items into a set of logical batches that can be sent off to
        // Salesforce
        $this->createBatches();
        if (empty($this->batches)) {
          $this->end_status = SALESFORCE_QUEUE_PROCESS_FAIL_NO_VALID_ITEMS;
        }
        else {
          $this->processBatches();
        }
      }
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
      // Make sure we don't have a claim on the queue by releasing it
      $this->releaseQueue();

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
        $this->lease_expire = variable_get('salesforce_queue_' . $this->queue_name . '_lease_expire', 0);
      }
    }
    return $this->items;
  }

  /**
   * Releases this processor's claim on the queue
   */
  public function releaseQueue() {
    $current_lease_key = variable_get('salesforce_queue_' . $this->queue_name . '_lease_key', NULL);
    if (!empty($this->lease_key) && $this->lease_key == $current_lease_key) {
      $this->queue->releaseBatch($this->lease_key);
      $this->lease_key = NULL;
    }
  }

  /**
   * Group the items into batches that can be sent to Salesforce in one call
   *
   * A batch of items are defined as items that can be sent to Salesforce in
   * one call. They can if they match the following requirements:
   *   - Same Salesforce object type
   *     - If operation is DELETE, object type doesn't matter
   *   - Same operation
   *     - If operation is UPSERT, they have the same dedupe field
   *   - No more than 200 items ($salesforce_max)
   *
   * @NOTE There may be an additional complication when considering row-by-row
   *       comparision.  A field map may have individual fields check against
   *       Salesforce to see if it should write/overwrite/ignore the submitted
   *       value when sending to Salesforce
   */
  public function createBatches() {
    // The goal is to have one loop through the items
    $batch_groups = array();
    $count = count($this->items);
    while ($count > 0) {
      $item = array_shift($this->items);
      $count--;

      // Adjust just in case an UPSERT command falls through without a dedupe
      if ($item->operation == 'UPSERT' && empty($item->dedupe_field)) {
        $item->operation = 'CREATE';
      }

      // Test the item for validity and fail it if it does not pass
      if ($this->isValidItem($item)) {
        // Slot the item into a Object->Operation->Dedupe batch group
        $this->addItemToBatchGroups($item, $batch_groups);
      }
      else {
        $this->failItem($item, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_NOT_VALID);
      }
    }

    // Grab all the batches from the groups and put them in a linear order by
    // how it has been configured
    $batches = $this->sortBatches($batch_groups);

    // All done! Add the new set of batches to the end of the current set
    $this->batches = array_merge($this->batches, $batches);
    return $this->batches;
  }

  /**
   * Slot the given item into a Object->Operation->Dedupe batch group
   *
   * @param object $item
   *   A queue item
   * @param array $batch_groups
   *   Temporary holder that categorizes all of the items and places them in
   *   group no larger than salesforce_max
   */
  private function addItemToBatchGroups($item, &$batch_groups) {
    // Set up our brevity vars
    $batch_key = NULL;
    $op = $item->operation;
    $object = $item->salesforce_type;
    $dedupe = $item->dedupe_field;
    $batch_template = array(
      'object' => $object,
      'operation' => $op,
      'count' => 0,
      'items' => array(),
      'sobjects' => array(),
    );

    // Check to see if a batch group of the type of the current item is
    // already made, if not, build one
    if (!isset($batch_groups[$object])) {
      $batch_groups[$object] = array();
    }
    if (!isset($batch_groups[$object][$op])) {
      $batch_groups[$object][$op] = array();
      $batch_key = 0;
      $batch = $batch_template;
    }
    if ($op == 'UPSERT' && !isset($batch_groups[$object][$op][$dedupe])) {
      $batch_groups[$object][$op][$dedupe] = array();
      $batch_key = 0;
      $batch = $batch_template;
    }

    // Check to see if the current batch is filled up or not.  If it is full,
    // start a new batch.
    if (is_null($batch_key)) {
      $batch_group = ($op == 'UPSERT') ? $batch_groups[$object][$op][$dedupe] : $batch_groups[$object][$op];
      end($batch_group);
      $last_key = key($batch_group);
      if ($batch_group[$last_key]['count'] >= $this->salesforce_max) {
        // Create a new batch
        $batch_key = $last_key + 1;
        $batch = $batch_template;
      }
      else {
        // Use current batch
        $batch_key = $last_key;
        $batch = $batch_group[$batch_key];
      }
    }

    // When sending to Salesforce we just need the sobject, but in other
    // processes we need the items.  To prevent looping through the items
    // once again, we'll store a copy of each.
    $batch['items'][] = $item;
    $batch['sobjects'][] = ($op == 'DELETE') ? $item->sobject->Id : $item->sobject; // Because DELETE has to be weird like that
    $batch['count']++;
    if ($op == 'UPSERT') {
      $batch['dedupe'] = $dedupe;
      $batch_groups[$object][$op][$dedupe][$batch_key] = $batch;
    }
    else {
      $batch_groups[$object][$op][$batch_key] = $batch;
    }
  }

  /**
   * Grab all the batches from the groups and sort them in a linear order by how
   * it has been configured for this site
   *
   * @param array $batch_groups
   *   The temporary holder of queue items as created from addItemToBatchGroups
   *
   * @return array
   *   Array of batches that can be processed by processBatches and processBatch
   */
  private function sortBatches($batch_groups) {
    $batches = array();

    // Move all of the batch items into the batches list in the configured order
    // @TODO set default object order
    $object_order = variable_get('salesforce_queue_object_order', array('Account', 'Contact', 'Opportunity'));
    $op_order = array('CREATE', 'UPSERT', 'UPDATE', 'DELETE');
    foreach ($object_order as $object) {
      if (isset($batch_groups[$object])) {
        foreach ($op_order as $op) {
          if (isset($batch_groups[$object][$op])) {
            if ($op == 'UPSERT') {
              foreach ($batch_groups[$object][$op] as $field => $field_batches) {
                $batches = array_merge($batches, $batch_groups[$object][$op][$field]);
                unset($batch_groups[$object][$op][$field]);
              }
              unset($batch_groups[$object][$op]);
            }
            else {
              $batches = array_merge($batches, $batch_groups[$object][$op]);
              unset($batch_groups[$object][$op]);
            }
          }
        }
        if (empty($batch_groups[$object])) {
          unset($batch_groups[$object]);
        }
      }
    }

    // Lastly, If any Salesforce objects were not configured for a specific
    // order, append those to the end of the batch list
    foreach ($batch_groups as $object => $operations) {
      foreach ($op_order as $op) {
        if (isset($batch_groups[$object][$op])) {
          if ($op == 'UPSERT') {
            foreach ($batch_groups[$object][$op] as $field => $batches) {
              $batches = array_merge($batches, $batch_groups[$object][$op][$field]);
            }
          }
          else {
            $batches = array_merge($batches, $batch_groups[$object][$op]);
          }
        }
      }
    }

    return $batches;
  }

  /**
   * Check to see if the item can be sent to Salesforce as-is
   *
   * @param object $item
   *   Queue item
   * @return boolean
   *   TRUE if the item is valid and can be sent to Salesforce with the given
   *   operation.  FALSE if the item cannot be sent to Salesforce with the
   *   given operation and provided information.
   */
  private function isValidItem($item) {
    $op = $item->operation;
    $sobject = $item->sobject;
    switch ($op) {
      case 'CREATE':
        // Cannot have an Id field, requires salesforce type and fields
        if (!isset($sobject->Id) && isset($sobject->type) && isset($sobject->fields)) {
          return TRUE;
        }
        break;
      case 'UPSERT':
        // Cannot have an Id field, requires dedupe field, salesforce type and fields
        if (!isset($sobject->Id) && !empty($item->dedupe_field) && isset($sobject->type) && isset($sobject->fields)) {
          return TRUE;
        }
        break;
      case 'UPDATE':
        // Requires Id field, salesforce type and either fields or fields to null
        if (isset($sobject->Id) && isset($sobject->type) && (isset($sobject->fields) || isset($sobject->fieldsToNull))) {
          return TRUE;
        }
        break;
      case 'DELETE':
        // Only requires Id
        if (isset($sobject->Id)) {
          return TRUE;
        }
    }
    return FALSE;
  }

  /**
   * Wrapper for processBatch() to process all batches ready for processing
   *
   * @return array
   *   Array of result objects provided by Salesforce
   */
  public function processBatches() {
    $results = array();
    $count = count($this->batches);
    while ($count > 0) {
      $count--;
      $result = $this->processBatch();
      $results = array_merge($results, $result);
    }
    return $results;
  }

  /**
   * Processes the next batch in line for processing
   *
   * @return array
   *   Array of result objects provided by Salesforce
   */
  public function processBatch() {
    $batch = array_shift($this->batches);
    $results = array();

    // Invokes hook_salesforce_queue_preprocess_batch().
    module_invoke_all('salesforce_queue_preprocess_batch', $batch, $this->queue);

    // Invokes hook_salesforce_queue_process_batch_alter().
    drupal_alter('salesforce_queue_batch', $batch, $this->queue);

    // Make sure we have enough time to send these items to Salesforce before
    // our lease expires. We do not want to risk the chance of items getting
    // sent to Salesforce twice, so we will not send items outside our lease or
    // when it is leased to someone else.
    $current_lease_key = variable_get('salesforce_queue_' . $this->queue_name . '_lease_key', '');
    if ($this->lease_expire < time() && $current_lease_key == $this->lease_key) {
      // Lease expired, fail
      $this->end_status = SALESFORCE_QUEUE_PROCESS_FAIL_LEASE_EXPIRED;
      $this->failBatch($batch, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_LEASE_EXPIRED);
    }
    else {
      // Woot, lease active. Send this baby off to Salesforce
      try {
        switch ($batch['operation']) {
          case 'CREATE':
            // Whole sobject
            $results = $this->soap->create($batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_CREATE;
            break;
          case 'UPSERT':
            // Whole sobject + dedupe field
            $results = $this->soap->upsert($batch['dedupe'], $batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_UPSERT;
            break;
          case 'UPDATE':
            // ID + fields to overwrite + fields to null
            $results = $this->soap->update($batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_UPDATE;
            break;
          case 'DELETE':
            // Require just ID of the object
            $results = $this->soap->delete($batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_DELETE;
            break;
        }
      }
      catch (Exception $error) {
        $this->failBatch($batch, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_BATCH, $error);
      }

      // Respond to responses
      foreach ($results as $key => $result) {
        $item = $batch['items'][$key];
        if ($result->success) {
          $this->passItem($item, $result);
        }
        else {
          // Mark items with errors as failed
          $this->failItem($item, $error, $result);
        }
      }
    }

    // Invokes hook_salesforce_queue_postprocess_batch().
    module_invoke_all('salesforce_queue_postprocess_batch', $batch, $results, $this->queue);

    return $results;
  }

  /**
   * Marks an item has having succeeded processing
   *
   * @param object $item
   */
  private function passItem($item, $result) {
    // Remove successful items from the queue
    $this->queue->deleteItem($item);

    // Keep a log of newly created items
    if ($item->operation == 'CREATE' || $item->operation == 'UPSERT') {
      $item->sobject->Id = $result->id;
      $this->new_sf_records[] = $item;
    }

    // Invokes hook_salesforce_queue_process_pass_item().
    module_invoke_all('salesforce_queue_process_pass_item', $item, $result, $this->queue);
  }

  /**
   * Marks an item has having failed
   *
   * @param object $item
   * @param int $reason
   *   The reason the item failed
   * @param object $result
   *   Salesforce result if the item had received a result from salesforce
   */
  private function failItem($item, $reason, $result = NULL) {
    // Invokes hook_salesforce_queue_process_fail_item().
    module_invoke_all('salesforce_queue_process_fail_item', $item, $reason, $result, $this->queue);
  }

  /**
   * Wrapper for failItem() so that every item in a batch is failed
   *
   * @param array $items
   *   Array of items as provided in a batch
   * @param int $reason
   *   The reason the batch failed
   * @param object $result
   *   Salesforce result if the item had received a result from salesforce
   */
  private function failBatch($batch, $reason, $result = NULL) {
    foreach ($batch['items'] as $item) {
      $this->failItem($item, $reason, $result);
    }
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
    return $this->releaseQueue();
  }

  /**
   * Helper to externally set the Salesforce API. Used for simpletests.
   */
  public function _setSalesforceApi(Salesforce $sfapi) {
    $this->sfapi = $sfapi;
  }

  /**
   * Helper to externally set the Salesforce SOAP Partner. Used for simpletests.
   */
  public function _setSalesforceSoap($soap) {
    $this->soap = $soap;
  }

  /**
   * Helper to remove newly created items from Salesforce. Used for simpletests.
   */
  public function _deleteNewSfRecords() {
    $batches = array_chunk($this->new_sf_records, $this->salesforce_max);
    $this->new_sf_records = array();
    foreach ($batches as $batch) {
      $sobjects = array();
      foreach ($batch as $key => $item) {
        $batch[$key]->operation = 'DELETE';
        $sobjects[] = $item->sobject->Id;
      }
      $this->batches[] = array(
        'operation' => 'DELETE',
        'items' => $batch,
        'sobjects' => $sobjects,
      );
    }
    $this->processBatches();
  }
}
