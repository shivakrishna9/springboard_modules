<?php

/**
 * Get an instance of the API client to use in API calls.
 * @return \SpringboardAdvocacyAPIClient
 */
function springboard_advocacy_message_action_api_client()
{

  $apiClient = &drupal_static(__FUNCTION__);

  // If the API client is not saved, instantiate a new client
  if (!isset($apiClient['client'])) {
    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();

    $apiClient['client'] = $client;
  }

  // Return the API client instance for use in API calls
  return $apiClient['client'];
}

/**
 * Callback handler for 'node/%/message-queue' menu item.
 *
 * Displays message queue delivery metrics and form actions for changing queue message items status.
 */
function sba_message_action_queue_page($nid) {

  // CSS assets
  drupal_add_css(drupal_get_path('module', 'sba_message_action') . '/css/message-queues.css');

  // Get the node ID/name for API calls
  $nodeId = node_load($nid)->advocacy_id;

  // Get an instance of the advocacy API client.
  $client = springboard_advocacy_message_action_api_client();

  $queueReadyItems = $client->getQueueMessagesCountByStatus($nodeId, 'Ready')->data->count;
  $queuePausedItems = $client->getQueueMessagesCountByStatus($nodeId, 'Paused')->data->count;
  $queueCanceledItems = $client->getQueueMessagesCountByStatus($nodeId, 'Canceled')->data->count;

  // Create the tabs
  $tabs = theme('messsage_deliverability_tabs', array(
    'nodeId' => $nid
  ));

  // Render the action form for changing queue items status
  $renderForm = drupal_get_form('sba_message_action_queues_form', $nodeId);
  $queueActionsForm = drupal_render($renderForm);

  // Setup the page theme
  $page = theme('messsage_action_queues', array(
    'tabs'               => $tabs,
    'queueReadyItems'    => $queueReadyItems,
    'queuePausedItems'   => $queuePausedItems,
    'queueCanceledItems' => $queueCanceledItems,
    'queueActionsForm'   => $queueActionsForm
  ));

  // Return the page to the browser
  return $page;
}

/**
 * Form handler for rendering the action form that allows changing message status.
 * @param $form
 * @param $form_state
 * @param $nodeId
 * @return mixed
 */
function sba_message_action_queues_form($form, &$form_state, $nodeId) {

  // Setup the bounds of the allowed and rendered queue actions
  $active = array('Pause' => t('Pause'), 'Restart' => t('Restart'), 'Cancel' => t('Cancel') );

  // Create the form radios for queue update status
  $form['queue_update_type'] = array(
    '#type' => 'radios',
    '#default_value' => 'Pause',
    '#options' => $active,
    '#description' => t('In the event that you need to immediately modify messages pending in the queue, you can do so here.'),
  );

  // Hidden node id for usage in API calls
  $form['node_id'] = array(
    '#type'          => 'value',
    '#name'          => 'node_id',
    '#title'         => t('Node Id'),
    '#default_value' => $nodeId,
    '#required'      => true,
  );

  // Submit button
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Update Queue Items'),
  );

  // Return the form
  return $form;
}

/**
 * Form submission handler for the sba message action queues page.
 * Implements hook_submit()
 * @param $form
 * @param $form_state
 */
function sba_message_action_queues_form_submit($form, &$form_state) {
  // Make the API request to queue up the CSV download
  $apiClient = springboard_advocacy_message_action_api_client();

  if (!form_get_errors()) {

    $updateStatusMessage = '';
    $updateStatusMessageType = 'status';

    // Check the submitted radio state to provide feedback and run appropriate API calls
    switch($form_state['input']['queue_update_type']) {
      case 'Pause':
        $apiRequest = $apiClient->modifyQueueMessagesStatus($form_state['values']['node_id'], 'pause');
        $updateStatusMessage = $apiRequest->data->items_updated == 0 ?
          'Their are no "Ready" messages to pause delivery for.': $apiRequest->data->items_updated.' Queue message/s have been "Paused" and will not be delivered.';
        if($apiRequest->data->items_updated == 0) {
          $updateStatusMessageType = 'warning';
        }
        break;
      case 'Restart':
        $apiRequest = $apiClient->modifyQueueMessagesStatus($form_state['values']['node_id'], 'restart');
        $updateStatusMessage = $apiRequest->data->items_updated == 0 ?
          'Their are no "Paused" messages to restart delivery for.': $apiRequest->data->items_updated.' Queue message/s are now "Ready" and are scheduled for delivery.';
        if($apiRequest->data->items_updated == 0) {
          $updateStatusMessageType = 'warning';
        }
        break;
      case 'Cancel':
        $apiRequest = $apiClient->modifyQueueMessagesStatus($form_state['values']['node_id'], 'cancel');

        $updateStatusMessage = $apiRequest->data->items_updated == 0 ?
          'Their are no "Ready" messages to cancel delivery for.': $apiRequest->data->items_updated.' "Ready" Queue message/s have been "Canceled" indefinitely.';
        if($apiRequest->data->items_updated == 0) {
          $updateStatusMessageType = 'warning';
        }
        break;
    }

    // Send the message back the to user for the update attempt and result.
    drupal_set_message($updateStatusMessage,$updateStatusMessageType);
  }
}

/**
 * Method to validate the form submission of the sba message action queues page.
 * Implements hook_validate()
 * @param $form
 * @param $form_state
 */
function sba_message_action_queues_form_validate($form, &$form_state) {

}
