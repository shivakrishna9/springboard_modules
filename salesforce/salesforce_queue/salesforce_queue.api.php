<?php

/**
 * @file
 * These are the hooks that are invoked by the Salesforce queue.
 */

/**
 * @defgroup salesforce_queue_hooks Salesforce Queue Hooks
 * @{
 * Salesforce Queue's hooks enable other modules to intercept events within
 * Salesforce Queue, such as the adding an item to the queue.
 */

/**
 * Alter an item before it is added to the queue
 *
 * @param object $sObject
 *   Salesforce sObject
 * @param int $drupal_id
 *   Unique identifier of the Drupal object, such as the entity ID
 * @param string $module
 *   Module implementing the data type
 * @param varchar $delta
 *   Name or id of the subtype
 */
function hook_salesforce_queue_create_item_alter(&$sObject, $drupal_id, $module, $delta) {

}

/**
 * Fire off behavior after an item has been added to the queue
 *
 * @param object $sObject
 *   Salesforce sObject
 * @param int $drupal_id
 *   Unique identifier of the Drupal object, such as the entity ID
 * @param string $module
 *   Module implementing the data type
 * @param varchar $delta
 *   Name or id of the subtype
 * @param bool $result
 *   The result of the attempt to create a new queue item
 */
function hook_salesforce_queue_create_item($sObject, $drupal_id, $module, $delta, $result) {

}

/**
 * Fire off behavior before an item is deleted from the queue
 *
 * @param int $drupal_id
 *   Unique identifier of the Drupal object, such as the entity ID
 * @param string $module
 *   Module implementing the data type
 * @param varchar $delta
 *   Name or id of the subtype
 */
function hook_salesforce_queue_delete_item($drupal_id, $module, $delta) {

}

/**
 * Fire off behavior before the queue processing begins
 *
 * This is triggered before any verification that the process is even capable of
 * being run
 *
 * @param SalesforceQueue $queue
 */
function hook_salesforce_queue_preprocess_queue(SalesforceQueue $queue) {

}

/**
 * Allows you to alter items retreived from the queue before they are processed
 *
 * @param array $items
 *   Array of queue objects
 */
function hook_salesforce_queue_process_items_alter(&$items, $queue) {

}

/**
 * Fire off behavior after the queue processing completes
 *
 * @param SalesforceQueue $queue
 * @param type $end_response
 *   The reason why the process was aborted.  Possible values are:
 *     - SALESFORCE_QUEUE_PROCESS_NOT_STARTED: No processing has been initiated
 *     - SALESFORCE_QUEUE_PROCESS_FAIL_CONNECT: Cannot connect to Salesforce
 *     - SALESFORCE_QUEUE_PROCESS_NO_ITEMS: There were no items found in the
 *         queue to process
 *     - SALESFORCE_QUEUE_PROCESS_FAIL_QUEUE_LEASED: Items cannot be claimed
 *         because the queue is already leased to another process
 *     - SALESFORCE_QUEUE_PROCESS_FAIL_CLAIM_ITEMS: Items cannot be claimed
 *         for an unknown reason
 *     - SALESFORCE_QUEUE_PROCESS_COMPLETE: Items were found and processed all
 *         as expected
 */
function hook_salesforce_queue_postprocess_queue(SalesforceQueue $queue, $end_response) {

}

/**
 * @}
 */