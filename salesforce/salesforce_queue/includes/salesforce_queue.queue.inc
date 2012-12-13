<?php
/**
 * @file
 * Salesforce specific implementation of SystemQueue.
 */
class SalesforceQueue extends SystemQueue {

  public function __construct($name) {
    watchdog('queue', '%name queue created', array('%name' => $name));
    parent::__construct($name);
  }

  /**
   * Overridden createItem.
   *
   * This version prevents duplicates.
   */
  public function createItem($data) {

    // Create an md5 has of the item's data to check for uniqueness
    $data = serialize($data);
    $hash = md5($data);
    $query = db_select('queue', 'q')
      ->fields('q', array('item_id'))
      ->where('md5(data) = :hash', array(':hash' => $hash));

    $result = $query->execute();

    // Do not re-add duplicate item
    if ($result->rowCount() > 0) {
      return FALSE;
    }

    $query = db_insert('queue')
        ->fields(array(
      'name' => $this->name,
      'data' => $data,
      // We cannot rely on REQUEST_TIME because many items might be created
      // by a single request which takes longer than 1 second.
      'created' => time(),
    ));
    return (bool) $query->execute();
  }

  /**
   * Retrieves a specific number of items from the queue.
   *
   * This is specific to Salesforce and is not part of the DrupalQueueInterface.
   */
  public function getItems($count) {
    $result = array();
    $items = db_query('SELECT data FROM {queue} q WHERE name = :name ORDER BY item_id ASC LIMIT :count', array(':name' => $this->name, ':count' => $count))->fetchAll();
    foreach ($items as $item) {
      $result[] = unserialize($item->data);
    }
    return $result;
  }

}
