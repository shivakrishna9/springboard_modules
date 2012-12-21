<?php

/**
 * @file
 * Contains SalesforceQueue
 */

/**
 * Salesforce specific implementation of DrupalQueue
 */
class SalesforceQueue implements DrupalReliableQueueInterface {

  /**
   * The name of the queue this instance is working with.
   *
   * @var string
   */
  protected $queue_name;

  /**
   * List of IDs used to identify unique leasors
   *
   * @var array
   */
  protected $leasor_ids;

  /**
   * A queue object for the given queue
   *
   * @param string $queue_name
   *   Arbitrary string to name a subset of a queue.  Standard queues are 'new',
   *   'retry', and 'failure'.  The default name 'all' is a working name
   *   allowing you to have access to all queue items regardless of which queue
   *   subset they are in.
   */
  public static function get($queue_name = 'all') {
    $queue = new SalesforceQueue($queue_name);
    return $queue;
  }

  /**
   * @name DrupalQueueInterface  The required interface methods
   * @{
   */

  /**
   * @param string $queue_name
   *   Declare which queue subset you are working with.  Use 'all' to work with
   *   all items in the entire queue.
   */
  public function __construct($queue_name) {
    $this->queue_name = $queue_name;
  }

  /**
   * Add an item to the queue
   *
   * If an item with identical values for drupal_id, module, delta and
   * salesforce_type is already in the queue, the item in the queue will be
   * updated with the data provided rather than another queue item created.
   *
   * @param array $data
   *   A key array with the following:
   *     - drupal_id: Unique identifier of the Drupal object, such as the
   *         entity ID
   *     - module: Module implementing the data type
   *     - delta: Name or id of the subtype
   *     - salesforce_type: Salesforce object type
   *     - operation: Action to take on Salesforce, CREATE, UPDATE, UPSERT, or
   *         DELETE
   *     - dedupe_field: Salesforce field to dedupe on during UPSERT operations
   *     - sobject: Salesforce sObject
   *
   * @return bool
   *   If the item insert or update failed, returns FALSE. If it succeeded,
   *   returns SAVED_NEW or SAVED_UPDATED, depending on the operation performed.
   */
  public function createItem($data) {
    $keys = array();

    if ($this->itemInQueue($data)) {
      // If this item is in the queue, replace the original.
      // With drupal_write_record, replacement is just a matter of filling in
      // the $keys array
      $keys = array('drupal_id', 'module', 'delta', 'salesforce_type');
    }
    else {
      // As a new record, fill in all the new fields
      $data['queue'] = $this->queue_name == 'all' ? 'new' : $this->queue_name;

      // We cannot rely on REQUEST_TIME because many items might be created
      // by a single request which takes longer than 1 second.
      $data['created'] = time();
    }

    // Invoke hook_salesforce_queue_create_item_alter().
    drupal_alter('salesforce_queue_create_item', $data['sobject'], $data['drupal_id'], $data['module'], $data['delta']);

    // Insert this baby
    $data['sobject'] = json_encode($data['sobject']);
    $result = drupal_write_record('salesforce_queue', $data, $keys);

    // Invoke hook_salesforce_queue_create_item().
    module_invoke_all('salesforce_queue_create_item', $data['sobject'], $data['drupal_id'], $data['module'], $data['delta'], $result);

    return $result;
  }

  /**
   * Quantity of items in the queue you have initialized
   *
   * @return int
   */
  public function numberOfItems() {
    $query = db_select('salesforce_queue', 'q')
      ->fields('q', array('item_id'));
    if ($this->queue_name != 'all') {
      $query->condition('queue', $this->queue_name);
    }
    return $query
      ->countQuery()
      ->execute()
      ->fetchField();
  }

  /**
   * Wrapper for claimBatch() to claim just a single item from the queue
   *
   * @TODO Figure out best default lease time
   *
   * @param int $lease_time
   *   The amount of time you believe it will take to process the item.  If your
   *   process takes longer, another process could take the item.
   *
   * @return object
   *   Returns all the information about the item from the queue.
   */
  public function claimItem($lease_time = 30) {
    $items = $this->claimBatch(1, $lease_time);
    if (empty($items)) {
      return FALSE;
    }
    return $items[0];
  }

  /**
   * Give up your lease on an item
   *
   * @param object $item
   *   The original queue item as given to you in claimItem()
   *
   * @return bool
   *   TRUE on successful release, FALSE if the release could not be performed
   */
  public function releaseItem($item) {
    $update = db_update('salesforce_queue')
      ->fields(array(
        'expire' => 0,
      ))
      ->condition('item_id', $item->item_id);
    return $update->execute();
  }

  /**
   * Remove an item from the queue
   *
   * @param object $item
   *   The original queue item as given to you in claimItem()
   *
   * @return bool
   *   TRUE on successful delete, FALSE if the delete could not be performed
   */
  public function deleteItem($item) {
    // Invoke hook_salesforce_queue_delete_item().
    module_invoke_all('salesforce_queue_delete_item', $item->drupal_id, $item->module, $item->delta);

    $result = db_delete('salesforce_queue')
      ->condition('drupal_id', $item->drupal_id)
      ->condition('module', $item->module)
      ->condition('delta', $item->delta)
      ->condition('salesforce_type', $item->salesforce_type)
      ->execute();

    // @TODO log removal

    return $result;
  }

  /**
   * Create a new queue
   */
  public function createQueue() {
    // All tasks are stored in a single database table (which is created when
    // salesforce_queue is first installed) so there is nothing we need to do to
    // create a new queue.
  }

  /**
   * Delete all queued items in the initiated queue
   *
   * @return bool
   *   TRUE on successful delete, FALSE if the delete could not be performed
   */
  public function deleteQueue() {
    $query = db_delete('salesforce_queue');
    if ($this->queue_name != 'all') {
      $query->condition('queue', $this->queue_name);
    }
    return $query->execute();
  }

  /**
   * @} DrupalQueueInterface
   */

  /**
   * Helper method to open an item after it has been retreived from the database
   */
  private function unfoldItem($item) {
    if (is_object($item)) {
      $item->sobject = json_decode($item->sobject);
      if (isset($item->sobject->fields)) {
        $item->sobject->fields = (array)$item->sobject->fields;
      }
    }
    return $item;
  }

  /**
   * Grab a batch of items from the initiated queue and inform others they are taken
   *
   * @TODO Figure out best default lease time
   *
   * @param int $quantity
   *   The maximum number of items to retreive from the queue.  If the queue
   *   does not have that many items, all remaining items will be retreived.
   * @param int $lease_time
   *   The amount of time you believe it will take to process the item.  If your
   *   process takes longer, another process could take the item.
   *
   * @return array
   *   Returns an array of objects retreived from the queue up to the number of
   *   items requested for $quantity. FALSE if no items were retreived from the
   *   queue.
   */
  public function claimBatch($quantity = 200, $lease_time = 3600) {
    // DrupalQueue first selects a row and then tries to update the row, and
    // will try again if the update fails because another worker took it in the
    // mean time.  For a batch situation, we will first update the rows and then
    // claim all rows that have our leasor id.  This should allow us to claim
    // batches while avoiding multiple workers steping on each other's toes.

    // Update the number of rows that match our quantity
    $id = uniqid();
    $this->leasor_ids[] = $id;
    $expire = time() + $lease_time; // REQUEST_TIME not fast enough
    $placeholders = array(
      ':expire' => $expire,
      ':leasor' => $id,
    );
    $sql = "UPDATE {salesforce_queue}
      SET expire = :expire,
        leasor = :leasor
      WHERE expire = 0";
    if ($this->queue_name != 'all') {
      $sql .= " AND queue = :queue_name";
      $placeholders[':queue_name'] = $this->queue_name;
    }
    // placeholders fail in limit, and range fails for UPDATE
    $sql .= " ORDER BY created ASC LIMIT " . (int)$quantity;
    db_query($sql, $placeholders);

    // Select the items leased to us
    $result = db_select('salesforce_queue', 'q')
      ->fields('q')
      ->condition('leasor', $id)
      ->orderBy('created', 'ASC')
      ->execute();
    $items = array();
    foreach ($result as $record) {
      $items[] = $this->unfoldItem($record);
    }
    return empty($items) ? FALSE : $items;
  }

  /**
   * Check to see if a given item is already in the queue
   *
   * @param array $data
   *   An array of data that is typically used in createItem() to send to queue.
   *   The nessisary keys for this method are:
   *     - drupal_id: Unique identifier of the Drupal object, such as the
   *         entity ID
   *     - module: Module implementing the data type
   *     - delta: Name or id of the subtype
   *     - salesforce_type: Salesforce object type
   *
   * @return bool
   *   TRUE if the item is in the queue, FALSE if the item is not
   */
  public function itemInQueue($data) {
    $count = db_select('salesforce_queue', 'q')
      ->fields('q', array('item_id'))
      ->condition('drupal_id', $data['drupal_id'])
      ->condition('module', $data['module'])
      ->condition('delta', $data['delta'])
      ->condition('salesforce_type', $data['salesforce_type'])
      ->countQuery()
      ->execute()
      ->fetchField();
    return $count > 0;
  }

  /**
   * Retreive an item from the queue
   *
   * Do NOT use this to process an item. 'Get' does not lease queue items and
   * merely provides you the information about the queue item.  In order to
   * retreive an item for processing, you must 'claim' it.
   *
   * @param int $drupal_id
   *   Unique identifier of the Drupal object, such as the entity ID
   * @param string $module
   *   Module implementing the data type
   * @param varchar $delta
   *   Name or id of the subtype
   * @param string $salesforce_type
   *   Salesforce object type
   *
   * @return object
   *   The queue item you requested, or FALSE if the item is not there
   */
  public function getItem($drupal_id, $module, $delta, $salesforce_type) {
    $item = db_select('salesforce_queue', 'q')
      ->fields('q')
      ->condition('drupal_id', $drupal_id)
      ->condition('module', $module)
      ->condition('delta', $delta)
      ->condition('salesforce_type', $salesforce_type)
      ->execute()
      ->fetchObject();
    $item = $this->unfoldItem($item);
    return $item;
  }
}
