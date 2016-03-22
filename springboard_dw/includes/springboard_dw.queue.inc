<?php

/**
 * @file
 * Queue functions for Springboard data warehouse.
 */

/**
 * STOLEN FROM drupal_cron_run. Needs improvement.
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
      $queue->deleteItem($item);
    } else {
      // Release the item back onto the queue for retry
      //
      drush_log('Bad API call, releasing item. ', 'warning');
      // drush_print_r($item);
      $queue->releaseItem($item);
      break;
    }

    /*
     * WHAT DO WE BENEFIT WITH USING WATCHDOG EXCEPTION DIRECTLY?
     *
        try {
          $queue->deleteItem($item);

        } catch (Exception $e) {

          // In case of exception log it and leave the item in the queue
          // to be processed again later.
          watchdog_exception('cron', $e);

          // Release the item back onto the queue for retry
          // $queue->releaseItem($item);
        }
    */

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