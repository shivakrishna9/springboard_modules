<?php

/**
 * @file
 * Gathers information for the queue and sends it to the queue
 */

/**
 * Handles the actual process of sending to the queue whichever queue that may be
 */
function salesforce_webform_queue($sid, $nid, stdClass $sObject, $dedupe_field = NULL, $operation = NULL) {
  // Use UPSERT if there is a dedupe field set, otherwise use CREATE
  if (empty($operation)) {
    $operation = 'CREATE';
    if (!empty($dedupe_field) && $dedupe_field != 'none') {
      $operation = 'UPSERT';
    }
  }

  // Send to queue based on which queue system we're using
  $queue_system = variable_get('salesforce_webform_queue_system', '');
  switch ($queue_system) {

    case 'instant':
      // @TODO
      drupal_set_message(t('We have no ability to instantly send data to Salesforce.'), 'warning');
      return FALSE;

    case 'drupal_queue':
      $queue = DrupalQueue::get('salesforce_webform');
      $queue->createQueue('salesforce_webform'); // It doesn't hurt to make sure
      $data = array(
        'drupal_id' => $sid,
        'module' => 'webform',
        'delta' => 'webform:'. $nid,
        'salesforce_type' => $sObject->type,
        'operation' => $operation,
        'sObject' => $sObject,
        'dedupe_field' => $dedupe_field,
      );
      return $queue->createItem($data);

    case 'salesforce_queue':
      if (!module_exists('salesforce_queue')) {
        drupal_set_message(t('The salesforce_queue module is not enabled, yet it is set as the queue system.'), 'error');
        return FALSE;
      }
      return salesforce_queue_insert($sid, 'webform', 'webform:'. $nid, $sObject->type, $operation, $sObject, $dedupe_field);
  }

  // @TODO Set error that a queue_system is not set
  // If in dev mode send to screen, in prod mode don't tell the user, send it
  // somewhere else, they're innocent
  drupal_set_message(t('The Queue System configuration is not set.'), 'error');
  return FALSE;
}
