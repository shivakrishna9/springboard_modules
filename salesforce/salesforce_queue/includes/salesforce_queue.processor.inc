<?php

/**
 * @file
 * Contains SalesforceQueueProcessor
 */

/**
 * Processes items from SalesforceQueue.
 */
class SalesforceQueueProcessor {

  /**
   * SalesforceQueue object.
   *
   * @var object
   */
  protected $queue;

  /**
   * Name of the SalesforceQueue this processor is processing.
   *
   * @var string
   */
  public $queueName;

  /**
   * Batch of items from the queue that need to be processed.
   *
   * @var array
   */
  protected $items = array();

  /**
   * Array of groups of items that can be sent to Salesforce simultaneously.
   *
   * @var array
   */
  protected $batches = array();

  /**
   *  A running log of the new records created in Salesforce.
   *
   * @var array
   */
  public $newSfRecords = array();

  /**
   * Running log of all the Ids of Salesforce records that have been processed.
   *
   * @var array
   */
  public $processedItemIds = array();

  /**
   * The configured maximum number of items that can be processed at once.
   *
   * This depends upon the system resources
   *
   * @var int
   */
  protected $maxItems = 1000;

  /**
   * The configured default time the queue will be leased to our process.
   *
   * @var int
   */
  protected $leaseTime = 3600;

  /**
   * The time at which the current claim on the queue will expire.
   *
   * @var int
   */
  protected $leaseExpire = 0;

  /**
   * Unique identifier that allows unlocking the lease on the queue.
   *
   * @var string
   */
  protected $leaseKey;

  /**
   * The maximum number of items that can be sent to Salesforce in one call.
   *
   * @var int
   */
  protected $sfMaxBatchSize = 200;

  /**
   * Salesforce object to connect with
   *
   * @var Salesforce
   */
  protected $sfapi;

  /**
   * SalesforceSoapPartner object to connect to Salesforce via SOAP.
   *
   * @var SalesforceSoapPartner
   */
  protected $soap;

  /**
   * Defines a default response to the end of the processing.
   *
   * @var int
   */
  protected $endStatus = SALESFORCE_QUEUE_PROCESS_NOT_STARTED;

  /**
   * A processor object for the given queue.
   *
   * @param SalesforceQueue $queue
   *   The desired queue to process against, or NULL to process against the
   *   multi-queue manager
   *
   * @return SalesforceQueueProcessor
   *   A processor for the given queue
   */
  public static function get($queue = NULL) {
    $queue = empty($queue) ? salesforce_queue_load() : $queue;
    $processor = new SalesforceQueueProcessor($queue);
    return $processor;
  }

  /**
   * Construct the object.
   *
   * @param SalesforceQueue $queue
   *   SalesforceQueue object
   */
  public function __construct(SalesforceQueue $queue) {
    $this->queue = $queue;
    $this->queueName = $queue->queueName;
    $this->maxItems = variable_get('salesforce_queue_max_items', SALESFORCE_QUEUE_MAX_ITEMS_DEFAULT);
    $this->leaseTime = variable_get('salesforce_queue_lease_time', SALESFORCE_QUEUE_LEASE_TIME_DEFAULT);
    $this->sfMaxBatchSize = variable_get('salesforce_queue_sf_max_batch_size', SALESFORCE_QUEUE_SF_MAX_BATCH_SIZE_DEFAULT);
    $this->sfapi = salesforce_get_api();
    $this->soap = new SalesforceSoapPartner($this->sfapi);
  }

  /**
   * Kicks off processing up-to the maximum allowed items from the given queue.
   */
  public function process() {
    // Invokes hook_salesforce_queue_preprocess_queue().
    module_invoke_all('salesforce_queue_preprocess_queue', $this->queue);

    // Gather items to process.
    // Because processor is run on cron, and an error is issued if Salesforce
    // cannot connect, first check to see if there are any items in the queue.
    // Cron may be run on numerous occasions before the site administrator
    // starts actually using the Salesforce Queue, so lets send them more severe
    // errors only if there are actually any queue items to process.
    $this->claimItems();
    if (!empty($this->items)) {
      // Check Salesforce connection.
      // This extra check is for simpletests and drushwhich require a specific
      // $sfapi in certain instances.
      $this->sfapi = $this->sfapi->isAuthorized() ? $this->sfapi : salesforce_get_api();
      if (!$this->sfapi->isAuthorized()) {
        // Cannot connect to Salesforce.
        watchdog('salesforce_queue', 'Could not process SalesforceQueue because '
          . 'I cannot connect to Salesforce.', array(), WATCHDOG_ALERT);
        $this->endStatus = SALESFORCE_QUEUE_PROCESS_FAIL_CONNECT;
      }

      // Continue only if there is a Salesforce connection.
      else {
        // Invokes hook_salesforce_queue_process_items_alter().
        drupal_alter('salesforce_queue_process_items', $this->items, $this->queue);

        // Sort the items into a set of logical batches that can be sent off to
        // Salesforce.
        $this->createBatches();
        if (empty($this->batches)) {
          // If there were items in the queue, but no batches, then that means
          // no claimed items were valid.
          $this->endStatus = SALESFORCE_QUEUE_PROCESS_FAIL_NO_VALID_ITEMS;
        }
        else {
          $this->processBatches();
        }
      }
    }

    if ($this->endStatus == SALESFORCE_QUEUE_PROCESS_NOT_STARTED) {
      // We got to the end without anyone else reporting anything!
      $end_status = SALESFORCE_QUEUE_PROCESS_COMPLETE;
    }
    else {
      $end_status = $this->endStatus;
    }

    $this->resetInternalState();

    // Invokes hook_salesforce_queue_postprocess_queue().
    module_invoke_all('salesforce_queue_postprocess_queue', $this->queue, $end_status);

    return $end_status;
  }

  /**
   * Claim a batch of items to process.
   */
  public function claimItems() {
    // If there are still unprocessed items, lets finish those first.
    if (empty($this->items)) {
      // Make sure we don't have a claim on the queue by releasing it.
      $this->releaseQueue();

      // Claim some items.
      $items = $this->queue->claimBatch($this->maxItems, $this->leaseTime);

      // SalesforceQueue::claimBatch() returns FALSE if there are no more items
      // or when the queue is currently leased.
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
          $this->endStatus = SALESFORCE_QUEUE_PROCESS_FAIL_QUEUE_LEASED;
        }
        elseif ($this->queue->numberOfItems(TRUE) < 1) {
          // No items are in the queue.  Pretty minor issue.
          $message = 'SalesforceQueueProcess was run, but no items were found '
            . 'in the queue to process.  Not much for you to worry about.';
          $severity = WATCHDOG_DEBUG;
          $this->endStatus = SALESFORCE_QUEUE_PROCESS_NO_ITEMS;
        }
        else {
          // If we get here, then we're really clueless as to what is going on.
          $message = 'SalesforceQueueProcess was run, but we could not claim '
            . 'items for an unknown reason.  Eeps.';
          $severity = WATCHDOG_ALERT;
          $this->endStatus = SALESFORCE_QUEUE_PROCESS_FAIL_CLAIM_ITEMS;
        }
        watchdog('salesforce_queue', $message, array(), $severity);
      }
      else {
        $this->leaseKey = $items[0]->leaseKey;
        $this->items = $items;
        $this->leaseExpire = variable_get('salesforce_queue_' . $this->queueName . '_lease_expire', 0);
      }
    }
    return $this->items;
  }

  /**
   * Releases this processor's claim on the queue.
   */
  public function releaseQueue() {
    $current_lease_key = variable_get('salesforce_queue_' . $this->queueName . '_lease_key', NULL);
    if (!empty($this->leaseKey) && $this->leaseKey == $current_lease_key) {
      $this->queue->releaseBatch($this->leaseKey);
      $this->leaseKey = NULL;
    }
  }

  /**
   * Group the items into batches that can be sent to Salesforce in one call.
   *
   * A batch of items are defined as items that can be sent to Salesforce in
   * one call. They can if they match the following requirements:
   *   - Same Salesforce object type
   *     - If operation is DELETE, object type doesn't matter
   *   - Same operation
   *     - If operation is UPSERT, they have the same dedupe field
   *   - No more than 200 items ($sfMaxBatchSize)
   *
   * @NOTE There may be an additional complication when considering row-by-row
   *       comparision.  A field map may have individual fields check against
   *       Salesforce to see if it should write/overwrite/ignore the submitted
   *       value when sending to Salesforce
   */
  public function createBatches() {
    // The goal is to have one loop through the items.
    $batch_groups = array();
    $count = count($this->items);
    while ($count > 0) {
      $item = array_shift($this->items);
      $count--;

      // Adjust just in case an UPSERT command falls through without a dedupe.
      if ($item->operation == 'UPSERT' && empty($item->dedupe_field)) {
        $item->operation = 'CREATE';
      }

      // Test the item for validity and fail it if it does not pass.
      if ($this->isValidItem($item)) {
        // Slot the item into a Object->Operation->Dedupe batch group.
        $this->addItemToBatchGroups($item, $batch_groups);
      }
      else {
        $this->failItem($item, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_NOT_VALID);
      }
    }

    // Grab all the batches from the groups and put them in a linear order by
    // how it has been configured.
    $batches = $this->sortBatches($batch_groups);

    // All done! Add the new set of batches to the end of the current set.
    $this->batches = array_merge($this->batches, $batches);
    return $this->batches;
  }

  /**
   * Slot the given item into a Object->Operation->Dedupe batch group.
   *
   * @param object $item
   *   A queue item
   * @param array $batch_groups
   *   Temporary holder that categorizes all of the items and places them in
   *   group no larger than $sfMaxBatchSize
   */
  protected function addItemToBatchGroups($item, &$batch_groups) {
    // Set up our brevity vars.
    $op = $item->operation;
    $object = $item->salesforce_type;
    $dedupe = $item->dedupe_field;
    $batch_template = array(
      'operation' => $op,
      'count' => 0,
      'items' => array(),
    );

    // Declare batch_group levels this item will be sorted under.
    $lvl1 = ($op == 'DELETE') ? 'DELETE' : $object;
    $lvl2 = ($op == 'DELETE') ? NULL : $op;
    $lvl3 = ($op == 'UPSERT') ? $dedupe : NULL;

    // Find the batch_group for this item, while creating missing batch_groups
    // along the way.
    if (!isset($batch_groups[$lvl1])) {
      $batch_groups[$lvl1] = array();
    }
    // DELETE.
    if (is_null($lvl2)) {
      $batch_group = &$batch_groups[$lvl1];
    }
    else {
      if (!isset($batch_groups[$lvl1][$lvl2])) {
        $batch_groups[$lvl1][$lvl2] = array();
      }
      // CREATE and UPDATE.
      if (is_null($lvl3)) {
        $batch_group = &$batch_groups[$lvl1][$lvl2];
      }
      else {
        // UPSERT.
        if (!isset($batch_groups[$lvl1][$lvl2][$lvl3])) {
          $batch_groups[$lvl1][$lvl2][$lvl3] = array();
        }
        $batch_group = &$batch_groups[$lvl1][$lvl2][$lvl3];
      }
    }

    // Check to see if the current batch is filled up or not.  If it is full,
    // start a new batch.
    end($batch_group);
    $last_key = key($batch_group);
    reset($batch_group);
    if (empty($batch_group) || $batch_group[$last_key]['count'] >= $this->sfMaxBatchSize) {
      // Create a new batch.
      $batch_key = is_numeric($last_key) ? $last_key + 1 : 0;
      $batch = $batch_template;
      if ($op == 'DELETE') {
        $batch['ids'] = array();
      }
      else {
        $batch['object'] = $object;
        $batch['sobjects'] = array();
      }
      if ($op == 'UPSERT') {
        $batch['dedupe'] = $dedupe;
      }
    }
    else {
      // Use current batch.
      $batch_key = $last_key;
      $batch = $batch_group[$batch_key];
    }

    // When sending to Salesforce we just need the sobject, but in other
    // processes we need the items.  To prevent looping through the items
    // once again, we'll store a link to the sobject.
    $count = $batch['count'];
    $batch['items'][$count] = $item;
    if ($op == 'DELETE') {
      // Because DELETE has to be special like that.
      $batch['ids'][$count] = &$item->sobject->Id;
    }
    else {
      // Link to the sobject in the item so editing one edits the other.
      $batch['sobjects'][$count] = &$batch['items'][$count]->sobject;
    }
    $batch['count']++;
    // $batch_groups referred by reference.
    $batch_group[$batch_key] = $batch;
  }

  /**
   * Sort batch groups into a linear order.
   *
   * Grab all the batches from the groups and sort them in a linear order by how
   * it has been configured for this site.
   *
   * @param array $batch_groups
   *   The temporary holder of queue items as created from addItemToBatchGroups
   *
   * @return array
   *   Array of batches that can be processed by processBatches and processBatch
   */
  protected function sortBatches($batch_groups) {
    $batches = array();

    // Remove the DELETE operation so we can place it at the end later.
    if (isset($batch_groups['DELETE'])) {
      $delete_batches = $batch_groups['DELETE'];
      unset($batch_groups['DELETE']);
    }

    // Move all of the batch items into the batches list in the configured order
    // @TODO set default object order.
    $default_object_order = array('Account', 'Contact', 'Opportunity');
    $object_order = variable_get('salesforce_queue_object_order', $default_object_order);
    $op_order = array('CREATE', 'UPSERT', 'UPDATE');
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
    // order, append those to the end of the batch list.
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

    // Append the DELETE batches to the end.
    if (isset($delete_batches)) {
      $batches = array_merge($batches, $delete_batches);
    }

    return $batches;
  }

  /**
   * Check to see if the item can be sent to Salesforce as-is.
   *
   * @param object $item
   *   Queue item
   *
   * @return boolean
   *   TRUE if the item is valid and can be sent to Salesforce with the given
   *   operation.  FALSE if the item cannot be sent to Salesforce with the
   *   given operation and provided information.
   */
  protected function isValidItem($item) {
    $op = $item->operation;
    $sobject = $item->sobject;
    switch ($op) {
      case 'CREATE':
        // Cannot have an Id field, requires salesforce type and fields.
        if (!isset($sobject->Id) && isset($sobject->type) && isset($sobject->fields)) {
          return TRUE;
        }
        break;

      case 'UPSERT':
        // Cannot have an Id field, requires dedupe field, salesforce type and
        // fields.
        if (!isset($sobject->Id) && !empty($item->dedupe_field) && isset($sobject->type) && isset($sobject->fields)) {
          return TRUE;
        }
        break;

      case 'UPDATE':
        // Requires Id field, salesforce type and either fields or fields to
        // null.
        if (isset($sobject->Id) && isset($sobject->type) && (isset($sobject->fields) || isset($sobject->fieldsToNull))) {
          return TRUE;
        }
        break;

      case 'DELETE':
        // Only requires Id.
        if (isset($sobject->Id)) {
          return TRUE;
        }
    }
    return FALSE;
  }

  /**
   * Wrapper for processBatch() to process all batches ready for processing.
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
   * Processes the next batch in line for processing.
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
    drupal_alter('salesforce_queue_batch', $batch, $this->queue, $this);

    // Make sure we have enough time to send these items to Salesforce before
    // our lease expires. We do not want to risk the chance of items getting
    // sent to Salesforce twice, so we will not send items outside our lease or
    // when it is leased to someone else.
    $current_lease_key = variable_get('salesforce_queue_' . $this->queueName . '_lease_key', NULL);
    if ($this->leaseExpire < time() && $current_lease_key == $this->leaseKey) {
      // Lease expired, fail.
      $this->endStatus = SALESFORCE_QUEUE_PROCESS_FAIL_LEASE_EXPIRED;
      $this->failBatch($batch, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_LEASE_EXPIRED);
    }
    else {
      // Woot, lease active. Send this baby off to Salesforce.
      try {
        switch ($batch['operation']) {
          case 'CREATE':
            // Whole sobject.
            $results = $this->soap->create($batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_CREATE;
            break;

          case 'UPSERT':
            // Whole sobject + dedupe field.
            $results = $this->soap->upsert($batch['dedupe'], $batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_UPSERT;
            break;

          case 'UPDATE':
            // ID + fields to overwrite + fields to null.
            $results = $this->soap->update($batch['sobjects']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_UPDATE;
            break;

          case 'DELETE':
            // Require just ID of the object.
            $results = $this->soap->delete($batch['ids']);
            $error = SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_DELETE;
            break;
        }
      }
      catch (Exception $sf_error) {
        $this->failBatch($batch, SALESFORCE_QUEUE_PROCESS_ITEM_FAIL_BATCH, $sf_error);
      }

      // Respond to responses.
      foreach ($results as $key => $result) {
        $item = $batch['items'][$key];
        if ($result->success) {
          $this->passItem($item, $result);
        }
        else {
          // Mark items with errors as failed.
          $this->failItem($item, $error, $result);
        }
      }
    }

    // Invokes hook_salesforce_queue_postprocess_batch().
    module_invoke_all('salesforce_queue_postprocess_batch', $batch, $results, $this->queue);

    return $results;
  }

  /**
   * Marks an item has having succeeded processing.
   *
   * @param object $item
   *   Queue item
   */
  protected function passItem($item, $result) {
    // Remove successful items from the queue.
    $this->queue->deleteItem($item);

    // Keep a log of newly created items.
    if ($item->operation == 'CREATE' || $item->operation == 'UPSERT') {
      $item->sobject->Id = $result->id;
      $this->newSfRecords[] = $item;
      $id_key = $item->module . '-' . $item->delta . '-' . $item->drupal_id;
      $this->processedItemIds[$item->salesforce_type][$id_key] = $result->id;
    }

    // Invokes hook_salesforce_queue_process_pass_item().
    module_invoke_all('salesforce_queue_process_pass_item', $item, $result, $this->queue);
  }

  /**
   * Marks an item has having failed.
   *
   * @param object $item
   *   Queue item
   * @param int $reason
   *   The reason the item failed
   * @param object $result
   *   Salesforce result if the item had received a result from salesforce
   */
  protected function failItem($item, $reason, $result = NULL) {
    // Invokes hook_salesforce_queue_process_fail_item().
    module_invoke_all('salesforce_queue_process_fail_item', $item, $reason, $result, $this->queue);
  }

  /**
   * Wrapper for failItem() so that every item in a batch is failed.
   *
   * @param array $batch
   *   Batch array as set up for processBatch() for execute
   * @param int $reason
   *   The reason the batch failed
   * @param object $result
   *   Salesforce result if the batch had received a result from salesforce
   */
  protected function failBatch($batch, $reason, $result = NULL) {
    foreach ($batch['items'] as $item) {
      $this->failItem($item, $reason, $result);
    }
  }

  /**
   * Eat yum yums.
   */
  protected function eatBatch() {
    // Nom nom nom.
  }

  /**
   * Release our claim on the queue and reset our internal state.
   */
  protected function resetInternalState() {
    $this->items = array();
    $this->batches = array();
    $this->endStatus = SALESFORCE_QUEUE_PROCESS_NOT_STARTED;
    return $this->releaseQueue();
  }

  /**
   * Helper to externally set the Salesforce API. Used for simpletests.
   */
  public function setSalesforceApi(Salesforce $sfapi) {
    $this->sfapi = $sfapi;
  }

  /**
   * Helper to externally set the Salesforce SOAP Partner. Used for simpletests.
   */
  public function setSalesforceSoap($soap) {
    $this->soap = $soap;
  }

  /**
   * Helper to remove newly created items from Salesforce. Used for simpletests.
   */
  public function deleteNewSfRecords() {
    if (empty($this->newSfRecords)) {
      return;
    }
    $batches = array_chunk($this->newSfRecords, $this->sfMaxBatchSize);
    $this->newSfRecords = array();
    foreach ($batches as $batch) {
      $ids = array();
      foreach ($batch as $key => $item) {
        $batch[$key]->operation = 'DELETE';
        $ids[] = $item->sobject->Id;
      }
      $this->batches[] = array(
        'operation' => 'DELETE',
        'count' => count($batch),
        'items' => $batch,
        'ids' => $ids,
      );
    }
    $this->processBatches();
  }
}
