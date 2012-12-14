<?php

/**
 * @file
 * Gathers information for the queue and sends it to the queue
 */

/**
 * Page callback to render the admin page
 */
function salesforce_webform_admin_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'salesforce-webform-admin-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>'. t('Configure how the Salesforce integration will work with webform submissions.') .'</p>',
      '#prefix' => '<div class="salesforce-webform-admin-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_webform_admin_form' => drupal_get_form('salesforce_webform_admin_form'),
  );
  return $build;
}

/**
 * Configuration form for this module
 */
function salesforce_webform_admin_form($form, &$form_state) {
  $form = array();

  // Queue system options
  $queue_options = array(
    'instant' => t('Send Instantaneously'),
    'drupal_queue' => t('Drupal Queue'),
  );
  $salesforce_queue_exists = module_exists('salesforce_queue');
  if ($salesforce_queue_exists) {
    $queue_options['salesforce_queue'] = t('Salesforce Queue');
  }

  // Default queue
  $queue_system = variable_get('salesforce_webform_queue_system', '');
  if (empty($queue_system)) {
    $queue_system = $salesforce_queue_exists ? 'salesforce_queue' : 'instant';
  }

  // Queue system form
  $form['salesforce_webform_queue_system'] = array(
    '#id' => 'salesforce-webform-queue-system',
    '#title' => t('Queue System'),
    '#description' => t(''),
    '#type' => 'radios',
    '#default_value' => $queue_system,
    '#options' => $queue_options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

/**
 * Submit callback for the configuration form
 */
function salesforce_webform_admin_form_submit($form, &$form_state) {
  $former_queue_system = variable_get('salesforce_webform_queue_system', '');
  $queue_system = $form_state['input']['salesforce_webform_queue_system'];
  variable_set('salesforce_webform_queue_system', $queue_system);

  // @TODO: Decommission old queue
  if (!empty($former_queue_system)) {
    // Switch existing queue data to the new queue and delete old queue
  }

  drupal_set_message(t('Your settings have been saved.'));

  // PSA
  if ($queue_system == 'instant') {
    drupal_set_message(t('Just so you know, instantaneously sending submission data to Salesforce does not actually work.'), 'warning');
  }
}

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
 * Handles the actual process of sending the request to dequeue all related items
 */
function salesforce_webform_dequeue_all($nid) {
  $queue_system = variable_get('salesforce_webform_queue_system', '');
  switch ($queue_system) {
    case 'drupal_queue':
      $query = db_select('queue', 'q')
        ->fields('q')
        ->condition('name', 'salesforce_webform');
      $result = $query->execute();
      $delete = array();
      foreach ($result as $record) {
        $data = unserialize($record->data);
        if ($data['delta'] == 'webform:'. $nid) {
          $delete[] = $record->item_id;
        }
      }
      if (empty($delete)) {
        return TRUE;
      }
      return db_delete('queue')
        ->condition('item_id', $delete, 'IN')
        ->execute();

    case 'salesforce_queue':
      if (!module_exists('salesforce_queue')) {
        drupal_set_message(t('The salesforce_queue module is not enabled, yet it is set as the queue system.'), 'error');
        return FALSE;
      }
      return db_delete('salesforce_queue')
        ->condition('module', 'webform')
        ->condition('delta', 'webform:'. $nid)
        ->execute();
 }

 // There is nothing to do here
 // If there is no queue system set, why not return TRUE?  There likely are no
 // queue items to delete anyhow!  'Instant' would be returning TRUE in this
 // case anyway.
 return TRUE;
}
