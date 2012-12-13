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

/**
 * A submit handler added to webforms that have Salesforce mappings
 */
function salesforce_webform_client_form_submit($form, &$form_state) {
  $node = $form['#node'];
  $sid = $form_state['values']['details']['sid'];
  $map = $node->salesforce_webform_map;
  $input = $form_state['input']['submitted'];

  // Build up the field data
  $fields = array();
  // Salesforce webform map
  foreach ($input as $wf_field => $input_value) {
    if (isset($map->field_map['salesforce_webform_map'][$wf_field])) {
      $sf_field = $map->field_map['salesforce_webform_map'][$wf_field];
      $fields[$sf_field] = $input_value;
    }
  }
  // Node properties map
  if (isset($map->field_map['salesforce_node_map'])) {
    if (isset($map->field_map['salesforce_node_map']['nid'])) {
      $sf_field = $map->field_map['salesforce_node_map']['nid'];
      $fields[$sf_field] = $node->nid;
    }
    if (isset($map->field_map['salesforce_node_map']['title'])) {
      $sf_field = $map->field_map['salesforce_node_map']['title'];
      $fields[$sf_field] = $node->title;
    }
  }
  // Submission properties map
  if (isset($map->field_map['salesforce_submission_map'])) {
    if (isset($map->field_map['salesforce_submission_map']['sid'])) {
      $sf_field = $map->field_map['salesforce_submission_map']['sid'];
      $fields[$sf_field] = $sid;
    }
    if (isset($map->field_map['salesforce_submission_map']['date'])) {
      $sf_field = $map->field_map['salesforce_submission_map']['date'];
      $fields[$sf_field] = time();
    }
  }

  // Create the Salesforce object
  $sObject = new stdClass();
  $sObject->fields = $fields;
  $sObject->type = $map->salesforce_object_type;
  if (!empty($map->salesforce_record_type)) {
    $sObject->record = $map->salesforce_record_type;
  }

  // Queue up the submissions
  $result = salesforce_webform_queue($sid, $node->nid, $sObject, $map->salesforce_dedupe_field);
  if (!$result) {
    // @TODO Send an error to admin that submission didn't go to queue
    drupal_set_message(t('The submission was not sent to Salesforce.'), 'error');
  }
}
