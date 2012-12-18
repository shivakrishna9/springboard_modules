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
   * A queue object for the given queue
   *
   * @param string $queue_name
   *   Arbitrary string to name a subset of a queue.  Standard queues are 'new',
   *   'retry', and 'failure'.  The default name 'all' is a working name
   *   allowing you to have access to all queue items regardless of which queue
   *   subset they are in.
   */
  public static function get($queue_name = 'all') {
    $queues = &drupal_static(__function__);
    if (!isset($queues)) {
      $queues = array();
    }
    if (!isset($queues[$queue_name])) {
      $queues[$queue_name] = new SalesforceQueue($queue_name);
    }
    return $queues[$queue_name];
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
    $sql = "SELECT COUNT(item_id)
      FROM {salesforce_queue}";
    if ($this->queue_name != 'all') {
      $sql .= " WHERE queue = :queue_name";
    }
    return db_query($sql, array(':queue_name' => $this->queue_name))
      ->fetchField();
  }

  /**
   * Grab a single item from the initiated queue and inform others it is taken
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
    // Claim an item by updating its expire fields. If claim is not successful
    // another thread may have claimed the item in the meantime. Therefore loop
    // until an item is successfully claimed or we are reasonably sure there
    // are no unclaimed items left.
    while (TRUE) {
      $sql = "SELECT *
        FROM {salesforce_queue} q
        WHERE expire = 0";
      $sql_vars = array();
      if ($this->queue_name != 'all') {
        $sql .= " AND queue = :queue_name";
        $sql_vars[':queue_name'] = $this->queue_name;
      }
      $item = db_query_range($sql, 0, 1, $sql_vars)->fetchObject();
      if ($item) {
        // Try to update the item. Only one thread can succeed in UPDATEing the
        // same row. We cannot rely on REQUEST_TIME because items might be
        // claimed by a single consumer which runs longer than 1 second. If we
        // continue to use REQUEST_TIME instead of the current time(), we steal
        // time from the lease, and will tend to reset items before the lease
        // should really expire.
        $update = db_update('salesforce_queue')
          ->fields(array(
            'expire' => time() + $lease_time,
          ))
          ->condition('item_id', $item->item_id)
          ->condition('expire', 0);
        // If there are affected rows, this update succeeded.
        if ($update->execute()) {
          $item->sobject = json_decode($item->sobject);
          $item->sobject->fields = (array)$item->sobject->fields;
          return $item;
        }
      }
      else {
        // No items currently available to claim.
        return FALSE;
      }
    }
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
    return db_delete('salesforce_queue')
      ->condition('item_id', $item->item_id)
      ->execute();
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
}
