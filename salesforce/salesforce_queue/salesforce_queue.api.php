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
 * @}
 */