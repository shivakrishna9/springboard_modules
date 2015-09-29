<?php

/**
 * Message action settings page, menu callback.
 *
 * @return array
 */
function sba_message_action_message_preview_page($nid) {

  $node = node_load($nid);

  // Check if the step two form has already been submitted.
  // @todo think about this.
  $completed = isset($_SESSION['action_completed']) ? $_SESSION['action_completed'] : TRUE;
  if($completed == TRUE) {
    drupal_set_message(t("This form has already been submitted. !return to the form.", array('!return' => l('Return', 'node/' . $node->nid))));
    return array();
  }

  // Build page header and multi-step form.
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'message-action-preview-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Please edit the messages for your legislators below. Remember to review your legislators and your contact information to ensure both are correct, before sending your messages.'),
      '#prefix' => '<div class="message-action-preview-page">',
      '#suffix' => '</div>',
    ),
    'sba_message_action_preview_form' => drupal_get_form('sba_message_action_preview_form', $node),
  );
  return $build;
}

/**
 * Multi-step form definition.
 *
 * @param $form
 * @param $form_state
 * @param $node
 *
 * @return mixed
 */
function sba_message_action_preview_form($form, $form_state, $node) {

  $form['#attached']['css'][] = drupal_get_path('module', 'sba_message_action') . '/css/message-preview.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_message_action') . '/js/sba_message_preview.js';

  if(!empty($_SESSION['delivered_messages']['messages'])) {

    // Build the form content out of the API response stored in session.
    $messages = $_SESSION['delivered_messages']['messages'];
    // @todo explore other ways of retrieving these two.
    $contact = $_SESSION['contact'];
    $action = $_SESSION['form_details'];

    $form['action_id'] = array(
      '#type' => 'value',
      '#value' => $action['action_id'],
    );
    if (isset($action['test_mode'])) {
      $form['test_mode'] = array(
        '#type' => 'value',
        '#value' => $action['test_mode'],
      );
      $form['test_mode_email'] = array(
        '#type' => 'value',
        '#value' => $action['test_email'],
      );
    }
//    $form['account_id'] = array(
//      '#type' => 'value',
//      '#value' => $action['account_id'],
//    );
    $form['contact'] = array(
      '#type' => 'value',
      '#value' => $contact,
    );

    $form['messages'] = array (
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array(
        'class' => array('message-preview-container'),
      ),
    );

    // Send our second step form through the same helper functions as the first step form.
    module_load_include('inc', 'sba_message_action', 'includes/sba_message_action.form');
    foreach ($messages as $key => $message) {

      // Theme the message header.
      $form['messages'][$key]['sent_to'] = array(
        '#markup' => theme('sba_message_action_preview_message_header', array('message' => $message)),
      );

      // Create the message form elements from the entity. Passed by reference
      // to the same helper function as the first step form.
      $message_entity = $node->messages[$message['message_id']];
      $build = array();
      sba_message_action_message_form($build, $message_entity, 'multi_flow');

      // Customize the resulting form to multi_flow format.
      unset($build['message'][$message_entity->sba_message_id]['subject_show']['#title']);
      $greeting = token_replace($build['message'][$message_entity->sba_message_id]['greeting']['#value'], array('sba_target' => (Object) $message['targets']));
      $build['message'][$message_entity->sba_message_id]['greeting_show']['#markup'] = $greeting;
      unset($build['message'][$message_entity->sba_message_id]['greeting']);
      $signature = token_replace($build['message'][$message_entity->sba_message_id]['signature_tokenized']['#value'], array('sba_contact' => (Object) $contact));
      $build['message'][$message_entity->sba_message_id]['signature']['#type'] = 'item';
      $build['message'][$message_entity->sba_message_id]['signature']['#markup'] = check_markup($signature, 'plain_text');
      unset($build['message'][$message_entity->sba_message_id]['signature_tokenized']);
      $build['message'][$message_entity->sba_message_id]['#attributes']['class'][] = 'message-preview-message-fieldset';
      $build['message'][$message_entity->sba_message_id]['precedence'] = array(
        '#type' => 'value',
        '#value' => $message['precedence'],
      );
      $build['message'][$message_entity->sba_message_id]['target'] = array(
        '#type' => 'value',
        '#value' => array('target_id' => $message['targets']->id, 'primary_delivery_method' => $message['targets']->primary_delivery_method, 'salutation' => $message['targets']->salutation, 'first_name' => $message['targets']->first_name, 'last_name' => $message['targets']->last_name),
      );
      if ($build['message'][$message_entity->sba_message_id]['body_required']['#value']) {
        $build['message'][$message_entity->sba_message_id]['edited_body']['#required'] = TRUE;
        $build['message'][$message_entity->sba_message_id]['edited_body']['#title'] =  t('Your custom message')
        ;

      }
      if ($build['message'][$message_entity->sba_message_id]['subject_required']['#value']) {
        $build['message'][$message_entity->sba_message_id]['subject']['#required'] = TRUE;
      }
      dpm($build['message'][$message_entity->sba_message_id]);

      // Assign the rebuilt messages to the form
      $form['messages'][$key]['message'] = $build['message'][$message_entity->sba_message_id];
    }
    $form['#action_node'] = $node;
    $form['#validate'][] = 'sba_message_action_preview_form_validate';
    $num = !empty($form['messages']) ? count(element_children($form['messages'])) : 0;
    $form['submit'] = array(
      '#prefix' => '<div id="edit-actions">',
      '#suffix' => '</div>',
      '#type' => 'submit',
      '#value' => t('Send Now'),
      '#weight' => 10,
      '#attributes' => array('class' => 'message-action-preview-submit'),
    );
  }
  else {
    // We should never get here.
  }
  return $form;
}

/**
 * Validate the  messages.
 *
 * @param $form
 * @param $form_state
 */
function sba_message_action_preview_form_validate($form, &$form_state) {
  $messages = !empty($form['messages']) ? element_children($form['messages']) : array();
  $changed = FALSE;
  foreach ($messages as $id) {

    $message = $form['messages'][$id]['message'];
    $body_default = isset($message['edited_body']['#default_value']) ? $message['edited_body']['#default_value'] : '';
    $body_edited = isset($form_state['values']['messages'][$id]['message']['edited_body']) ? $form_state['values']['messages'][$id]['message']['edited_body'] : '';
    $subject_default = isset($message['subject']['#default_value']) ? $message['subject']['#default_value'] : '';
    $subject_edited = isset($form_state['values']['messages'][$id]['message']['subject']) ? $form_state['values']['messages'][$id]['message']['subject'] : '';
    // Enforce mandatory editing.
    if ($body_edited == $body_default && isset($message['body_required']) && $message['body_required']['#value'] == 1) {
      form_set_error("messages][$id][message][edited_body", t('Please personalize your message before sending.'));
    }
    elseif ($body_edited != $body_default) {
      $changed = TRUE;
    }
    if ($subject_edited == $subject_default && isset($message['subject_required']) && $message['subject_required']['#value'] == 1) {
      form_set_error("messages][$id][message][subject", t('Please personalize the subject line before sending your message.'));
    }
    elseif (!isset($message['subject_show']) && $subject_edited != $subject_default) {
      $changed = TRUE;
    }
  }
  // Set user_edited_flag
  $form_state['storage']['changed'] = $changed;

}


/**
 * Submit the messages to the API endpoint.
 *
 * @param $form
 * @param $form_state
 */
function sba_message_action_preview_form_submit($form, &$form_state) {
  module_load_include('inc', 'sba_message_action', 'includes/sba_message_action.form');
  $messages = $form_state['values']['messages'];
  // Make it like how the resolver likes it.
  $formatted_messages = array();
  foreach ($messages as $key => $message) {
    $edited_body = isset($message['message']['edited_body']) ? $message['message']['edited_body'] : '';
    $formatted_messages[$key]['message']['body'] = sba_message_action_build_message_bodies($message['message']['sba_message_id'], $edited_body);
    $formatted_messages[$key]['message']['subject'] =  $message['message']['subject'];
    $formatted_messages[$key]['message']['precedence'] =  $message['message']['precedence'];
    $formatted_messages[$key]['message']['id'] =  $message['message']['message_id'];
    $formatted_messages[$key]['deliverable'] = $message['message']['target'];
  }
  $payload = array(
    'multiflow' => 'step_two',
    'alert_id' => $form_state['values']['action_id'],
    'test_mode' => $form_state['values']['test_mode'],
    'test_email' => $form_state['values']['test_mode_email'],
    'contact' => $form_state['values']['contact'],
    'messages' => $formatted_messages,
  );

  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('resolveTargets', $payload);

  if (isset($response->status_code) && $response->status_code == 200) {

    $node = $form['#action_node'];
    // Add session flag to prevent resubmission of the form
    $_SESSION['action_completed'] = TRUE;
    $people = array();

    if(!empty($_SESSION['delivered_messages']['messages'])) {
      foreach ($_SESSION['delivered_messages']['messages'] as $message) {
        $people[$message['targets']->id] = $message['person'];
      }

      // Rebuild the the delivered messages session variable with custom text if any.
      $messages_for_confirmation = array();
      foreach ($formatted_messages as $key => $f_message) {
        $messages_for_confirmation[$key] = $formatted_messages[$key]['message'];
        $messages_for_confirmation[$key]['targets'] = (Object) $formatted_messages[$key]['deliverable'];
        $messages_for_confirmation[$key]['person'] = $people[$formatted_messages[$key]['deliverable']['target_id']];
      }

      $_SESSION['delivered_messages'] =
        array(
          'messages' => $messages_for_confirmation,
          'target_count' => count($messages_for_confirmation),
          'message_count' => count($messages_for_confirmation),
        );
    }
    // Redirect to the original confirmation page.
    // $sid is set in sba_message_action_multifow_submit()
    $sid = !empty($_SESSION['action_sid']['sid']) ? $_SESSION['action_sid']['sid'] : FALSE;
    if ($sid) {
      unset($_SESSION['action_sid']);
      $form_state['redirect'] = array(
        'node/' . $node->nid . '/done',
        array('query' => array('sid' => $sid))
      );
    }

    // Update the user edit flag.
    if ($form_state['storage']['changed'] == TRUE) {
      if ($sid) {
        $user_edit = db_query('SELECT * FROM {webform_component} WHERE nid = :nid AND form_key = :form_key',
          array(
            ':nid' => $node->nid,
            ':form_key' => 'sba_user_edit_flag'
          ))->fetchObject();
        if (!empty($user_edit->cid)) {
          db_update('webform_submitted_data')->fields(array('data' => 1))
            ->condition('nid', $node->nid, '=')
            ->condition('sid', $sid, '=')
            ->condition('cid', $user_edit->cid, '=')
            ->condition('no', 0, '=')
            ->execute();
        }
      }
    }
  }
  else {
    drupal_set_message(t("A problem occurred and your message was not submitted. Please try again. If the problem persists, we should have it fixed shortly. Try back in a few minutes."), 'error');
  }
}

/**
 * Theme the message preview header, i.e., recipient name
 *
 * @param $vars
 * @return string
 */
function theme_sba_message_action_preview_message_header($vars) {
  $intro = t('The message below will be sent to:');
  $output = '<div class="message-preview-header"><div class="message-preview-intro">';
  $output .= $intro;
  $output .= '</div><div class="message-preview-person">';
  $output .= theme('sba_message_action_person_details', $vars['message']['person']);
  $output .= '</div></div>';
  return $output;
}