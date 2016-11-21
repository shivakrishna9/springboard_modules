<?php


/**
 * @file
 * These are the hooks that are invoked by the Salesforce queue.
 */

/**
 * @defgroup salesforce_ngoc_hooks Salesforce NGOC Hooks
 * @{
 * Salesforce NGOC's hooks enable other modules to react to events such as
 * as change in processing status of a batch upload object.
 */


/**
 * Fire off behavior after polling of SF batch upload objects has completed.
 *
 * @param array $batch_upload_sobjects
 *   An assoc array containing SObject objects, keyed by the objects SF Id.
 * @param array $changed_records
 *   Array of salesforce_ngoc_sync_map records with their new values.
 */
function hook_salesforce_ngoc_batch_status_change($batch_upload_sobjects, $changed_records) {

}
