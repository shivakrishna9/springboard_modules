<?php

/**
 * @file
 * Queue functions for Springboard data warehouse.
 */

/**
 * Queue processing hook.
 * Ideas:
 *  - Add a time argument.
 *  - Add a queue argument (like salesforce queue).
 *  - Make sure this is locked correctly.
 */
function springboard_dw_process_queue()
{
  $callback = 'springboard_dw_export_item';

  $end = time() + 60;

  // Get an instance of the Drupal data warehouse queue
  $queue = DrupalQueue::get('springboard_dw_export');

  // Lock and claim an item for queue processing
  while (time() < $end && ($item = $queue->claimItem())) {

    // If the api responds back correctly, remove the queue item
    if (call_user_func($callback, $item->data)) {
      try {
        $queue->deleteItem($item);
      } catch (Exception $e) {
        // In case of exception log it and leave the item in the queue
        // to be processed again later.
        watchdog_exception('cron', $e);

        // Release the item back onto the queue for retry
        $queue->releaseItem($item);
      }
    } else {
      // Log the API error
      drush_log('Bad API call, releasing item. ', 'warning');

      // Release the item back onto the queue for retry
      $queue->releaseItem($item);
      break;
    }
  }
}

/**
 * Implements hook_cron_queue_info().
 */
function springboard_dw_cron_queue_info()
{
  $queues['springboard_dw_export'] = array(
    'worker callback' => 'springboard_dw_export_item',
    'time'            => 60,
    'skip on cron'    => true,
  );

  return $queues;
}

/**
 * Helper function to query the data warehouse table to determine if this item has been recorded as being warehoused already.
 * @param $type
 *  The type of warehouse object object to check against in the DB query.
 * @param $drupal_id
 *  The unique identifier for the item (typically a Drupal ID unless it is a message item in which case it is a MongoId string)
 */
function springboard_dw_item_is_warehoused($type, $drupal_id)
{
  // Query items from the data warehouse table that match the item id and type
  $query = db_query("select * from {springboard_data_warehouse} where drupal_id = :drupal_id and type = :type", array(':type' => $type, ':drupal_id' => $drupal_id));

  // If an item is found, we do not need to warehouse this item
  if ($query->rowCount() > 0) {
    return true;
  }

  // If this item is not warehoused, return false
  return false;
}

/**
 * Mark an item in Drupal as being warehoused.
 * @param $type
 *  The type or identifier for what kind of item we are tracking.
 * @param $identifier
 *  The unique identifier for the item (typically a Drupal ID unless it is a message item in which case it is a MongoId string)
 * @throws \Exception
 */
function springboard_dw_track_item_warehouse_queued($type, $identifier)
{
  // Insert the user account into the Drupal data warehouse table
  db_insert('springboard_data_warehouse')->fields(array(
    'type'      => $type,
    'drupal_id' => $identifier,
    'created'   => time()
  ))->execute();
}