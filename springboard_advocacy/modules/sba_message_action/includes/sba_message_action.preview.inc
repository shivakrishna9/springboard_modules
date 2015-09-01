<?php

/**
 * Message action settings page, menu callback.
 *
 * @return array
 */
function sba_message_action_message_preview_page($nid) {

  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'message-action-preview-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Please edit the messages for your legislators below. Remember to review your legislators and your contact information to ensure both are correct, before sending your messages.'),
      '#prefix' => '<div class="message-action-preview-page">',
      '#suffix' => '</div>',
    ),
    'sba_message_action_preview_form' => drupal_get_form('sba_message_action_preview_form', $nid),
  );
  return $build;
}

function sba_message_action_preview_form($form, $form_state, $nid) {

  $node = node_load($nid);

  if(!empty($_SESSION['delivered_messages']['messages'])) {
    $messages = $_SESSION['delivered_messages']['messages'];
    $contact = $_SESSION['contact'];
    $action = $_SESSION['form_details'];
    $target_count = $_SESSION['delivered_messages']['target_count'];
    $message_count = $_SESSION['delivered_messages']['message_count'];

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
    );

    module_load_include('inc', 'sba_message_action', 'includes/sba_message_action.form');
    foreach ($messages as $message) {
      $form['messages'][]['sent_to'] = array(
        '#markup' => t('The message below will be sent to:'),
      );
      $form['messages'][]['recipient'] = array(
        '#markup' => '<p>' . $message['person']['name'],
      );

      $message_entity = $node->messages[$message['message_id']];
      $build = array();
      sba_message_action_message_form($build, $message_entity, FALSE);
      unset($build['message'][1]['#title']);
      unset($build['message'][1]['subject_show']['#title']);
      $greeting = token_replace($build['message'][1]['greeting']['#value'], array('sba_target' => (Object) $message['targets']));
      $build['message'][1]['greeting_show']['#markup'] = $greeting;
      $signature = token_replace($build['message'][1]['signature_tokenized']['#value'], array('sba_contact' => (Object) $contact));
      $build['message'][1]['signature']['#markup'] = $signature;
      $form['messages'][]['fields'] = $build['message'][1];

      $form['messages'][]['precedence'] = array(
        '#type' => 'value',
        '#value' => $message['precedence'],
      );

      $form['messages'][]['target_id'] = array(
        '#type' => 'value',
        '#value' => $message['targets']->id,
      );
    }

  }

  return $form;
}

function sba_message_action_settings_form_submit($form, $form_state) {

}
