<?php

/**
 * Message action settings page, menu callback.
 *
 * @return array
 */
function sba_social_action_message_preview_page($nid) {

  // $nid already sanitized in the access hook.
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
    'page_description_content' => array(
      '#id' => 'social-action-preview-page',
      '#type' => 'markup',
      '#markup' => t('Please review the messages below; or if you prefer, simply <span id="multiflow-send-now-wrapper" style="display:inline-block"><a href="#" id="multiflow-send-now">send now</a></span>.'),
      '#prefix' => '<div class="social-action-preview-page"><p>',
      '#suffix' => '</p></div>',
      '#weight' => -8,
    ),

    'sba_social_action_preview_form' => drupal_get_form('sba_social_action_preview_form', $node),
  );

  $messages = $build['content']['sba_social_action_preview_form']['messages'];
  $children = array_flip(element_children($messages));
  $subject_required = FALSE;
  $body_required = FALSE;
  $editable_but_not_required = FALSE;
  foreach ($messages as $key => $message) {
    if(isset($children[$key]) && (!$subject_required && !$body_required)) {
      if(isset( $message['subject']) && !isset($message['subject_show'])) {
        $editable_but_not_required = TRUE;
        $subject_required = isset($message['message']['subject']['#required']) ? $message['message']['subject']['#required'] : FALSE;
      }
      if (isset($message['message']['edited_body'])) {
        $editable_but_not_required = TRUE;
        $body_required = $message['message']['edited_body']['#required'];
      }
    }
  }

  $build['content']['page_description_label']['#prefix'] = '<h4>';
  $build['content']['page_description_label']['#suffix'] = '</h4>';
  $build['content']['page_description_label']['#weight'] = -10;
  $build['content']['page_description_label']['#markup'] = t('Step 2 of 2');
  if (isset($_SESSION['twitter']['values']['screen_name'])) {
    $build['content']['page_description_login']['#prefix'] = '<div><p>';
    $build['content']['page_description_llogin']['#suffix'] = '</p></div>';
    $build['content']['page_description_login']['#weight'] = -9;
    $build['content']['page_description_login']['#markup'] = t('You will be posting to Twitter as @%name. If that is not the correct account, !clickhere to log out and start over. You will need to submit the first form again to re-sign into Twitter.', array('%name' => $_SESSION['twitter']['values']['screen_name'], '!clickhere' => l('click here', 'clear/session', array('query' => array('nid' => $node->nid)))));
  }

  if ($subject_required || $body_required) {
    $editable_but_not_required = FALSE;
    $build['content']['page_description_content']['#markup'] = t('One or more of the messages below requires editing. Please make your edits and then click "Send Now" at the bottom of the page.');
  }
  if (!empty($editable_but_not_required)) {
    $build['content']['page_description_content']['#markup'] = t('Please review and edit the messages below; or if you prefer, simply <span id="multiflow-send-now-wrapper"><a href="#" id="multiflow-send-now">send now</a></span>.');
  }

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
function sba_social_action_preview_form($form, $form_state, $node) {

  $form['#attached']['css'][] = drupal_get_path('module', 'sba_social_action') . '/css/message-preview.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/social_message_preview.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/jQuery.simplyCountable.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/social_counter.js';

  // Build the form content out of the API response stored in session.
  $messages = array();
  if (!empty($_SESSION['deliverable_social_messages']['messages'])) {

    foreach ($_SESSION['deliverable_social_messages']['messages'] as $id => $message) {
      $messages[$node->messages[$message['message_id']]->data['weight'] . $message['person']['organization'] . $message['targets']->last_name] = $message;
      unset($messages[$id]);
    }
  }

  if (!empty($_SESSION['undeliverable_social_messages']['messages'])) {
    foreach ($_SESSION['undeliverable_social_messages']['messages'] as $message) {
      $messages[$node->messages[$message['message_id']]->data['weight'] . $message['person']['organization'] . $message['targets']->last_name] = $message;
    }
  }

  ksort($messages);

  // @todo explore other ways of retrieving these two.
  $contact = $_SESSION['contact'];
  $action = $_SESSION['form_details'];

  $form['action_id'] = array(
    '#type' => 'value',
    '#value' => $action['action_id'],
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

  // Used to send our second step form through the same helper functions as the first step form.
  module_load_include('inc', 'sba_social_action', 'includes/sba_social_action.form');

  foreach ($messages as $key => $message) {
    $message_entity_type = $node->messages[$message['message_id']]->type;

    // Theme the message header.
    $form['messages'][$key]['sent_to'] = array(
      '#markup' => theme('sba_social_action_preview_message_header', array('message' => $message, 'type' => $message_entity_type)),
    );

    // Create the message form elements from the entity. Passed by reference
    // to the same helper function as the first step form.
    $message_entity = $node->messages[$message['message_id']];
    $build = array();
    sba_social_action_message_form($build, $message_entity, 'multi_flow', $message_entity_type);
    $build['message'][$message_entity->sba_message_id]['#attributes']['class'][] = 'message-preview-message-fieldset';

    // Add hold value.
    $build['message'][$message_entity->sba_message_id]['hold'] = array(
      '#type' => 'value',
      '#value' => isset($message['hold']) ? TRUE : FALSE,
    );
    // Build an array of target properties
    $build['message'][$message_entity->sba_message_id]['target'] = array(
      '#type' => 'value',
      '#value' => array('target_id' => $message['targets']->id, 'twitter' => $message['targets']->twitter, 'salutation' => $message['targets']->salutation, 'first_name' => $message['targets']->first_name, 'last_name' => $message['targets']->last_name),
    );
    // Check for required fields
    if (isset($build['message'][$message_entity->sba_message_id]['body_required']) && $build['message'][$message_entity->sba_message_id]['body_required']['#value']) {
      $build['message'][$message_entity->sba_message_id]['edited_body']['#required'] = TRUE;
      $build['message'][$message_entity->sba_message_id]['edited_body']['#title'] =  t('You must edit the body of this message before sending');
    }
    if (isset($build['message'][$message_entity->sba_message_id]['subject_required']) && $build['message'][$message_entity->sba_message_id]['subject_required']['#value']) {
      $build['message'][$message_entity->sba_message_id]['subject']['#required'] = TRUE;
      $build['message'][$message_entity->sba_message_id]['subject']['#title'] =  t('Subject - You must edit this subject line before sending');
    }
    $form['messages'][$key]['message'] = $build['message'][$message_entity->sba_message_id];
  }
  // Add the node object to use in validation and submit.
  $form['#action_node'] = $node;
  // Define the validation function.
  $form['#validate'][] = 'sba_social_action_preview_form_validate';

  // Add submit button
  $form['submit'] = array(
    '#prefix' => '<div id="edit-actions">',
    '#suffix' => '</div>',
    '#type' => 'submit',
    '#value' => t('Send Now'),
    '#weight' => 10,
    '#attributes' => array('class' => 'social-action-preview-submit'),
  );
  return $form;
}

/**
 * Validate the  messages.
 *
 * @param $form
 * @param $form_state
 */
function sba_social_action_preview_form_validate($form, &$form_state) {

  $messages = !empty($form['messages']) ? element_children($form['messages']) : array();
  $changed = FALSE;
  // To prevent duplicate error messages.
  $body_error_once = NULL;
  $subject_error_once = NULL;

  // Loop through the messages and validate whether user is required to edit subject or body.
  // Also set a value for future use by the "user_edit_flag" component.
  foreach ($messages as $id) {

    // The message
    $message = $form['messages'][$id]['message'];

    // The default value for the user-editable body if any.
    $body_default = isset($message['edited_body']['#default_value']) ? $message['edited_body']['#default_value'] : '';
    // The edited value in the user-editable body if any.
    $body_edited = isset($form_state['values']['messages'][$id]['message']['edited_body']) ? $form_state['values']['messages'][$id]['message']['edited_body'] : '';

    // The default value for the user-editable subject if any.
    $subject_default = isset($message['subject']['#default_value']) ? $message['subject']['#default_value'] : '';
    // The edited value in the user-editable subject if any.
    $subject_edited = isset($form_state['values']['messages'][$id]['message']['subject']) ? $form_state['values']['messages'][$id]['message']['subject'] : '';

    // Enforce mandatory editing of the body if set.
    $body_error = !isset($body_error_once) ? t('Please personalize your message before sending.') : '';
    if ($body_edited == $body_default && isset($message['body_required']) && $message['body_required']['#value'] == 1) {
      $body_error_once = TRUE;
      form_set_error("messages][$id][message][edited_body", $body_error);
    }
    elseif ($body_edited != $body_default) {
      // Set change value for use by the "user_edit_flag" component.
      $changed = TRUE;
    }

    // Enforce mandatory editing of the subject if set.
    $subject_error = !isset($subject_error_once) ? t('Please personalize the subject line before sending your message.') : '';
    if ($subject_edited == $subject_default && isset($message['subject_required']) && $message['subject_required']['#value'] == 1) {
      $subject_error_once = TRUE;
      form_set_error("messages][$id][message][subject", $subject_error);
    }
    elseif (!isset($message['subject_show']) && $subject_edited != $subject_default) {
      // Set change value for use by the "user_edit_flag" component.
      $changed = TRUE;
    }
  }
  // Set value for the user_edited_flag
  $form_state['storage']['changed'] = $changed;

  if (!empty($_SESSION['messages']['error'])) {
    foreach ($_SESSION['messages']['error'] as $key => $message) {
      if (strpos($message, 'You must edit the body of this message before sending') !== FALSE) {
        unset($_SESSION['messages']['error'][$key]);
        drupal_set_message('Please personalize your message before sending', 'error', FALSE);
      }
      if (strpos($message, 'You must edit the subject of this message before sending') !== FALSE) {
        unset($_SESSION['messages']['error'][$key]);
        drupal_set_message('Please personalize the subject line before sending', 'error', FALSE);
      }
    }
  }

}

/**
 * Submit the messages to the API endpoint.
 *
 * @param $form
 * @param $form_state
 */
function sba_social_action_preview_form_submit($form, &$form_state) {

  module_load_include('inc', 'twitter');

  foreach ($form_state['values']['messages'] as $message) {

    $message_text = isset($message['message']['edited_body']) ? $message['message']['edited_body'] : $message['message']['body'];
    $target = parse_url($message['message']['target']['twitter']);
    $body = '.@' . substr($target['path'], 1) . ' ' . $message_text;

    try {
      $twitter = new Twitter(
        variable_get('twitter_consumer_key', ''),
        variable_get('twitter_consumer_secret', ''),
        $_SESSION['twitter']['values']['oauth_token'],
        $_SESSION['twitter']['values']['oauth_token_secret']
      );

      $twitter->statuses_update($body);

    } catch (TwitterException $e) {
      drupal_set_message(t('An error occurred when posting to Twitter. Check the system log for details.'), 'error');
    }
  }

  // Get sba_social_action_build_message_bodies().
  module_load_include('inc', 'sba_social_action', 'includes/sba_social_action.form');

  $messages = $form_state['values']['messages'];
  $node = $form['#action_node'];

  // Add session flag to prevent resubmission of the form
  $_SESSION['action_completed'] = TRUE;

  // Redirect to the original confirmation page.
  // $sid is set in sba_social_action_multifow_submit()
  $sid = !empty($_SESSION['action_sid']['sid']) ? $_SESSION['action_sid']['sid'] : FALSE;
  if ($sid) {
    unset($_SESSION['action_sid']);
    $form_state['redirect'] = array(
      'node/' . $node->nid . '/done',
      array('query' => array('sid' => $sid))
    );
  }
  else {
    // Gotta go somewhere.
    $form_state['redirect'] = array('node/' . $node->nid);
  }

  //@todo everything below here can go in advocacy module along with corresponding code in message actions
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
  if ($sid) {
    $complete = db_query('SELECT * FROM {webform_component} WHERE nid = :nid AND form_key = :form_key',
      array(
        ':nid' => $node->nid,
        ':form_key' => 'sba_multistep_complete'
      ))->fetchObject();
    if (!empty($complete->cid)) {
      db_update('webform_submitted_data')->fields(array('data' => 1))
        ->condition('nid', $node->nid, '=')
        ->condition('sid', $sid, '=')
        ->condition('cid', $complete->cid, '=')
        ->condition('no', 0, '=')
        ->execute();
    }
  }
  if ($sid) {
    // Update to trigger salesforce resync.
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    $submission = webform_get_submission($node->nid, $sid);
    webform_submission_update($node, $submission);
  }
}

/**
 * Theme the message preview header, i.e., recipient name
 *
 * @param $vars
 * @return string
 */
function theme_sba_social_action_preview_message_header($vars) {
  $intro = t('We will automatically prefix the tweet below with ".@@name".', array('@name' => $vars['message']['person']['twitter']));
  $output = '<div class="message-preview-header"><div class="message-preview-intro">';
  $output .= $intro;
  $output .= '</div><div class="message-preview-person">';
  $output .= theme('sba_social_action_person_details', $vars);
  $output .= '</div></div>';
  return $output;
}