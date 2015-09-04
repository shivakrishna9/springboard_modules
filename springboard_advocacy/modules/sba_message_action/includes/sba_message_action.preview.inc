<?php

/**
 * Message action settings page, menu callback.
 *
 * @return array
 */
function sba_message_action_message_preview_page($nid) {
  $node = node_load($nid);
  //@TODO if node multi flow is false, deny access.

  if($_SESSION['action_' . $node->nid . '_completed'] == TRUE) {
    drupal_set_message("This form has already been submitted.");
    return array();
  }
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

function sba_message_action_preview_form($form, $form_state, $node) {

  //@TODO user edit flag update submission


  $form['#attached']['css'][] = drupal_get_path('module', 'sba_message_action') . '/css/message-preview.css';
  if(!empty($_SESSION['delivered_messages']['messages'])) {
    $messages = $_SESSION['delivered_messages']['messages'];
    $contact = $_SESSION['contact'];
    $action = $_SESSION['form_details'];

    $form['action_id'] = array(
      '#type' => 'value',
      '#value' => $action['action_id'],
    );
    $form['test_mode'] = array(
      '#type' => 'value',
      '#value' => $action['test_mode'],
    );
    $form['test_mode_email'] = array(
      '#type' => 'value',
      '#value' => $action['test_email'],
    );
    $form['account_id'] = array(
      '#type' => 'value',
      '#value' => $action['account_id'],
    );
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

    module_load_include('inc', 'sba_message_action', 'includes/sba_message_action.form');
    foreach ($messages as $message) {
      $form['messages'][]['sent_to'] = array(
        '#markup' => theme('sba_message_action_preview_message_header', array('message' => $message)),
      );
      $message_entity = $node->messages[$message['message_id']];
      $build = array();
      sba_message_action_message_form($build, $message_entity, FALSE);
      unset($build['message'][1]['#title']);
      unset($build['message'][1]['subject_show']['#title']);
      $greeting = token_replace($build['message'][1]['greeting']['#value'], array('sba_target' => (Object) $message['targets']));
      $build['message'][1]['greeting_show']['#markup'] = $greeting;
      unset($build['message'][1]['greeting']);
      $signature = token_replace($build['message'][1]['signature_tokenized']['#value'], array('sba_contact' => (Object) $contact));
      $build['message'][1]['signature']['#markup'] = $signature;
      unset($build['message'][1]['signature_tokenized']);
      $build['message'][1]['#attributes']['class'][] = 'message-preview-message-fieldset';
      $build['message'][1]['precedence'] = array(
        '#type' => 'value',
        '#value' => $message['precedence'],
      );
      $build['message'][1]['target'] = array(
        '#type' => 'value',
        '#value' => array('target_id' => $message['targets']->id, 'primary_delivery_method' => $message['targets']->primary_delivery_method),
      );
      $form['messages'][] = $build['message'][1];
    }
  }
  $form['#action_node'] = $node;
  $form['#validate'][] = 'sba_message_action_preview_form_validate';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send Message'),
    '#weight' => 10,
    '#attributes' => array('class' => 'message-action-preview-submit'),
  );
  return $form;
}


function sba_message_action_preview_form_validate($form, &$form_state) {
  $messages = element_children($form['messages']);
  foreach ($messages as $id) {
      $message = $form['messages'][$id];
      $body_default = isset($message['edited_body']['#default_value']) ? $message['edited_body']['#default_value'] : '';
      $body_edited = isset($form_state['values']['messages'][$id]['edited_body']) ? $form_state['values']['messages'][$id]['edited_body'] : '';
      $subject_default = isset($message['subject']['#default_value']) ? $message['subject']['#default_value'] : '';
      $subject_edited = isset($form_state['values']['messages'][$id]['subject']) ? $form_state['values']['messages'][$id]['subject'] : '';
      $changed = FALSE;
      // Enforce mandatory editing.
      if ($body_edited == $body_default && isset($message['body_required']) && $message['body_required']['#value'] == 1) {
        form_set_error("messages][$id][edited_body", t('Please personalize your message before sending.'));
      }
      elseif ($body_edited != $body_default) {
        $changed = TRUE;
      }
      if ($subject_edited == $subject_default && isset($message['subject_required']) && $message['subject_required']['#value'] == 1) {
        form_set_error("messages][$id][subject", t('Please personalize the subject line before sending your message.'));
      }
      elseif (!isset($message['subject_show']) && $subject_edited != $subject_default) {
        $changed = TRUE;
      }
      // Set user_edited_flag
//    if ($changed && isset($form['submitted']['sba_user_edit_flag'])) {
//      form_set_value($form['submitted']['sba_user_edit_flag'], 1, $form_state);
//    }
    }
}


function sba_message_action_preview_form_submit($form, &$form_state) {
  module_load_include('inc', 'sba_message_action', 'includes/sba_message_action.form');
  $messages = $form_state['values']['messages'];
  $formatted_messages = array();
  foreach ($messages as $key => $message) {
    $edited_body = isset($message['edited_body']) ? $message['edited_body'] : '';
    $form_state['values']['messages'][$key]['body'] = sba_message_action_build_message_bodies($message['sba_message_id'], $edited_body);
    unset($form_state['values']['messages'][$key]['sba_message_id']);
    unset($form_state['values']['messages'][$key]['edited_body']);
    unset($form_state['values']['messages'][$key]['body_required']);
    $formatted_messages[$key]['message']['body'] =  $message['body'];
    $formatted_messages[$key]['message']['subject'] =  $message['subject'];
    $formatted_messages[$key]['message']['precedence'] =  $message['precedence'];
    $formatted_messages[$key]['message']['id'] =  $message['message_id'];
    $formatted_messages[$key]['deliverable'] = $message['target'];
  }
  $payload = array(
    'multiflow' => 'step_two',
    'alert_id' => $form_state['values']['action_id'],
    'test_mode' => $form_state['values']['test_mode'],
    'test_email' => $form_state['values']['test_mode_email'],
    'account_id' => $form_state['values']['account_id'],
    'contact' => $form_state['values']['contact'],
    'messages' => $formatted_messages,
  );
  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('resolveTargets', $payload);
  $node = $form['#action_node'];
    $form_state['redirect'] = array('node/' . $node->nid . '/done' , array('query' => array('sid' => $_GET['sid'])));
  $_SESSION['action_' . $node->nid . '_completed'] = TRUE;
}

function theme_sba_message_action_preview_message_header($vars) {

  $intro = t('The message below will be sent to:');
  $person = $vars['message']['person']['name'];
  $title = isset($vars['message']['person']['title']) ? $vars['message']['person']['title'] . ', ' : '' ;
  $organization = isset($vars['message']['person']['organization'])  ? $title . $vars['message']['person']['organization'] : '';
  $affiliation = isset($vars['message']['person']['district'])  ? $vars['message']['person']['district'] . ' (' . $vars['message']['person']['party'] . ')' : $organization;

  $output = '<div class="message-preview-header"><div class="message-preview-intro">';
  $output .= $intro;
  $output .= '</div><div class="message-preview-person">';
  $output .= $person . ' ' . $affiliation;
  $output .= '</div></div>';
  return $output;
}