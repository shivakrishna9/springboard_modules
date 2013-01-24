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
   * Maximum length a query can be to Salesforce before it is rejected.
   *
   * @var int
   */
  protected $sfMaxQueryLength = 10000;

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
    $this->sfMaxBatchSize = variable_get('salesforce_queue_sf_max_query_length', SALESFORCE_QUEUE_SF_MAX_QUERY_LENGTH_DEFAULT);
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
    $object_type = $item->object_type;
    $dedupe_field = $item->dedupe_field;
    $batch_template = array(
      'operation' => $op,
      'count' => 0,
      'items' => array(),
    );

    // Declare batch_group levels this item will be sorted under.
    $lvl1 = ($op == 'DELETE') ? 'DELETE' : $object_type;
    $lvl2 = ($op == 'DELETE') ? NULL : $op;
    $lvl3 = ($op == 'UPSERT') ? $dedupe_field : NULL;

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
        $batch['object_type'] = $object_type;
        $batch['sobjects'] = array();
      }
      if ($op == 'UPSERT') {
        $batch['dedupe_field'] = $dedupe_field;
        $batch['sync_rules'] = array(
          'fields' => array(),
          'dedupe_values' => array(),
        );
      }
      elseif ($op == 'UPDATE') {
        $batch['sync_rules'] = array(
          'fields' => array(),
          'ids' => array(),
        );
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

    // UPSERT and UPDATE items need to have their sync_rules checked.  Add the
    // information needed to process their sync rules to the batch.
    if ($this->itemNeedsToCheckSalesforce($item)) {
      // UPDATE items need to store all the fields and object Ids.
      if ($op == 'UPDATE') {
        $batch['sync_rules']['ids'][$item->sobject->Id] = $item->sobject->Id;
        foreach ($item->sobject->fields as $field => $value) {
          $batch['sync_rules']['fields'][$field] = $field;
        }
      }

      // UPSERT items need to store all their fields and the values they have
      // set for the given dedupe field.
      elseif ($op == 'UPSERT') {
        $batch['sync_rules']['dedupe_values'][$item->sobject->fields[$item->dedupe_field]] = $item->sobject->fields[$item->dedupe_field];
        foreach ($item->sobject->fields as $field => $value) {
          $batch['sync_rules']['fields'][$field] = $field;
        }
      }
    }

    // $batch_groups referred by reference.
    $batch_group[$batch_key] = $batch;
  }

  /**
   * Check if the item needs to check Salesforce records before sending data.
   *
   * @param object $item
   *   Queue item
   *
   * @return boolean
   *   TRUE if the item needs to check the record on Salesforce before sending
   *   its data to Salesforce, and FALSE if it can just go ahead and send its
   *   data to Salesforce as-is.
   */
  protected function itemNeedsToCheckSalesforce($item) {
    if (($item->operation == 'UPSERT' || $item->operation == 'UPDATE') && isset($item->sync_rules) && !empty($item->sync_rules)) {
      foreach ($item->sync_rules as $rule) {
        // There is no need to pull down the Salesforce record if all the rules
        // are to always replace the Salesforce value.
        if ($rule != SALESFORCE_QUEUE_SYNC_RULE_ALWAYS) {
          return TRUE;
        }
      }
    }
    return FALSE;
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
    foreach ($object_order as $object_type) {
      if (isset($batch_groups[$object_type])) {
        foreach ($op_order as $op) {
          if (isset($batch_groups[$object_type][$op])) {
            if ($op == 'UPSERT') {
              foreach ($batch_groups[$object_type][$op] as $field => $field_batches) {
                $batches = array_merge($batches, $batch_groups[$object_type][$op][$field]);
                unset($batch_groups[$object_type][$op][$field]);
              }
              unset($batch_groups[$object_type][$op]);
            }
            else {
              $batches = array_merge($batches, $batch_groups[$object_type][$op]);
              unset($batch_groups[$object_type][$op]);
            }
          }
        }
        if (empty($batch_groups[$object_type])) {
          unset($batch_groups[$object_type]);
        }
      }
    }

    // Lastly, If any Salesforce objects were not configured for a specific
    // order, append those to the end of the batch list.
    foreach ($batch_groups as $object_type => $operations) {
      foreach ($op_order as $op) {
        if (isset($batch_groups[$object_type][$op])) {
          if ($op == 'UPSERT') {
            foreach ($batch_groups[$object_type][$op] as $field => $batches) {
              $batches = array_merge($batches, $batch_groups[$object_type][$op][$field]);
            }
          }
          else {
            $batches = array_merge($batches, $batch_groups[$object_type][$op]);
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

    // Apply the Salesforce sync rules to the items in this batch.
    $this->applySyncRules($batch);

    // Make sure we have enough time to send these items to Salesforce before
    // our lease expires. We do not want to risk the chance of items getting
    // sent to Salesforce twice, so we will not send items outside our lease or
    // when it is leased to someone else.
    $current_lease_key = variable_get('salesforce_queue_' . $this->queueName . '_lease_key', NULL);
    if ($this->endStatus == SALESFORCE_QUEUE_PROCESS_FAIL_LEASE_EXPIRED || ($this->leaseExpire < time() && $current_lease_key == $this->leaseKey)) {
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
            $results = $this->soap->upsert($batch['dedupe_field'], $batch['sobjects']);
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
   * Takes sync rules of the batch and applies them to the items.
   *
   * @param array $batch
   *   Batch of items to be sent to Salesforce
   */
  protected function applySyncRules(&$batch) {
    if (!isset($batch['sync_rules']) || empty($batch['sync_rules']['fields'])) {
      // This batch does not have anything special to be concerned with.
      return;
    }

    // Retreive records from salesforce about the items in this batch.
    $sf_records = array();
    if ($batch['operation'] == 'UPDATE') {
      // UPDATE merely requires retreiving the Ids of all items.
      $fields = implode(', ', $batch['sync_rules']['fields']);
      // Needs to be an unassociated array or Salesforce will reject it.
      $ids = array_values($batch['sync_rules']['ids']);
      // The actual retreive will be done below.  Both UPSERT and UPDATE will
      // end up with the same output to to put into the same retreive query.
    }
    elseif ($batch['operation'] == 'UPSERT') {
      // UPSERT is more involved and requires a query of all records of the
      // object type that have a value of the given dedupe field of one of the
      // values that all of the items have.  Plus, max query length. Fun.
      $object = $batch['object'];
      $dedupe_field = $batch['dedupe_field'];
      $dedupe_values = "'" . implode("', '", $batch['sync_rules']['dedupe_values']) . "'";
      $query = "SELECT Id, $dedupe_field FROM $object WHERE $dedupe_field IN ($dedupe_values)";

      // Verify the query string does not exceed sfMaxQueryLength.  If it
      // does, chunk up the dedupe values until it does not.  Salesforce
      // cannot handle it when the query is too long.
      $queries = array($query);
      if (strlen($query) > $this->sfMaxQueryLength) {
        $split = 2;
        while (strlen($query) > $this->sfMaxQueryLength) {
          $chunk_size = $batch['count'] / $split;
          $chunks = array_chunk($dedupe_values, $chunk_size);
          $dedupe_values = "'" . implode("', '", $chunks[0]) . "'";
          $query = "SELECT Id, $dedupe_field FROM $object WHERE $dedupe_field IN ($dedupe_values)";
          $split++;
        }
        $queries = array();
        foreach ($chunks as $chunk) {
          $dedupe_values = "'" . implode("', '", $chunk) . "'";
          $queries = "SELECT Id, $dedupe_field FROM $object WHERE $dedupe_field IN ($chunk)";
        }
      }

      // Query will always been in a multiple queries array in order to
      // support the case for if the query has become too long.
      $ids = array();
      $upsert_ids = array();
      foreach ($queries as $query) {
        try {
          $response = $this->soap->query($query);
          if ($response->size > 0) {
            // Store in array keyed by dedupe value.
            foreach ($response->records as $record) {
              // Partner API requests are formatted incorrectly, so we are
              // reciving the Id as an array, and the fields are not retreived
              // in a structured format. Because of this, we are grabbing only
              // the Ids from this query, then passing them on to the retreive
              // request below.  Once the bug is fixed, remove this work around
              // and retreive the records directly from this query.
              // @see http://goo.gl/Xv8k7
              $ids[] = $record->Id[0];
              $search = array(
                '<sf:' . $dedupe_field . '>',
                '</sf:' . $dedupe_field . '>',
              );
              $replace = array('', '');
              $dedupe_value = trim(str_replace($search, $replace, $record->any));
              $upsert_ids[$dedupe_value] = $record->Id[0];
            }
          }
        }
        catch (Exception $error) {
          // Log message for later investigation.
          watchdog('salesforce_queue', 'Could not fetch UPSERT records for sync rules: !faultstring', array('!faultstring' => $error->faultstring), WATCHDOG_DEBUG);
        }
      }
    }

    // Retreive the records for the gathered Ids.
    if (empty($ids)) {
      // If there are no Ids to fetch, there is nothing left to do here.
      // UPSERTs can leave us with zero records existing in Salesforce.
      return;
    }
    else {
      $fields = implode(', ', $batch['sync_rules']['fields']);
      try {
        $response = $this->soap->retrieve($fields, $batch['object'], $ids);
        // Store in array keyed by Salesforce Id.
        foreach ($response as $record) {
          $sf_records[$record->Id] = $record->fields;
        }
      }
      catch (Exception $error) {
        // Log message for later investigation.
        watchdog('salesforce_queue', 'Could not retreive records for sync rules: !faultstring', array('!faultstring' => $error->faultstring), WATCHDOG_DEBUG);
      }
    }

    // Go through the items and apply the sync rules based upon the information
    // just retreived from Salesforce.
    foreach ($batch['items'] as $key => $item) {
      // Make sure the item was actually found in Salesforce before continuing.
      $id = ($item->operation == 'UPDATE') ? $item->sobject->Id : (isset($upsert_ids[$item->sobject->fields[$dedupe_field]]) ? $upsert_ids[$item->sobject->fields[$dedupe_field]] : NULL);
      if (isset($sf_records[$id])) {
        $sf_record = $sf_records[$id];
        foreach ($item->sync_rules as $field => $rule) {
          switch ($rule) {
            case SALESFORCE_QUEUE_SYNC_RULE_ALWAYS:
              // Leave the field as is.
              break;

            case SALESFORCE_QUEUE_SYNC_RULE_BLANK:
              // Check to see if Salesforce version is blank, unset otherwise.
              if (isset($sf_record->$field) && strlen($sf_record->$field) > 0) {
                if (isset($dedupe_field) && $field == $dedupe_field) {
                  // If this is the dedupe field, make sure the value does not
                  // change.
                  $batch['items'][$key]->sobject->fields[$field] = $sf_record->$field;
                }
                else {
                  // Otherwise, just do not send this value to Salesforce.
                  unset($batch['items'][$key]->sobject->fields[$field]);
                }
              }
              break;

            case SALESFORCE_QUEUE_SYNC_RULE_APPEND:
              // When the Salesforce version is not blank, add value from
              // Salesforce to the begining of the item's value.
              if (isset($sf_record->$field) && !empty($sf_record->$field)) {
                $joined_value = $sf_record->$field . ' ' . $item->sobject->fields[$field];
                $batch['items'][$key]->sobject->fields[$field] = $joined_value;
              }
              break;

            case SALESFORCE_QUEUE_SYNC_RULE_NEVER:
              if (isset($dedupe_field) && $field == $dedupe_field) {
                // If this is the dedupe field, make sure the value does not
                // change.
                $batch['items'][$key]->sobject->fields[$field] = $sf_record->$field;
              }
              else {
                // Otherwise, just do not send this value to Salesforce.
                unset($batch['items'][$key]->sobject->fields[$field]);
              }
              break;
          }
        }

        // The 'blank' and 'never' sync rules will unset fields, which can cause
        // the case where an item will have no fields left to send to
        // Salesforce. Remove items that have nothing left to send.
        if (empty($batch['items'][$key]->sobject->fields) && (!isset($batch['items'][$key]->sobject->fieldsToNull) || (isset($batch['items'][$key]->sobject->fieldsToNull) && empty($batch['items'][$key]->sobject->fieldsToNull)))) {
          $resort = TRUE;
          unset($batch['items'][$key]);
          unset($batch['sobjects'][$key]);
        }
      }
    }

    // If an item was removed from the batch because all of its fields were
    // unset, then the keys will need to be put back into straight numeric
    // order.
    if (isset($resort) && $resort) {
      $batch['items'] = array_values($batch['items']);
      $batch['sobjects'] = array_values($batch['sobjects']);
    }
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
      $this->processedItemIds[$item->object_type][$id_key] = $result->id;
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
