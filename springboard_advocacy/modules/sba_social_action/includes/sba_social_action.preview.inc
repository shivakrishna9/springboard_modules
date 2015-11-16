<?php

/**
 * Message action settings page, menu callback.
 *
 * @return array
 */
function sba_social_action_message_preview_page($nid) {

  // $nid already sanitized in the access hook.
  $node = node_load($nid);
  if (isset($_SESSION['messages']['status'])) {
    // Prevent the confirmation message from appearing prematurely.
    $_SESSION['saved_messages']['status'] = $_SESSION['messages']['status'];
    // clear all
    drupal_get_messages('status');
  }
  // Check if the step two form has already been submitted.
  // @todo think about this.
  $completed = isset($_SESSION['social_action_completed']) ? $_SESSION['social_action_completed'] : TRUE;
  if($completed == TRUE) {
    drupal_set_message(t("This form has already been submitted. !return to the form.", array('!return' => l('Return', 'node/' . $node->nid))));
    return array();
  }

  // Build page header and multi-step form.
  $build = array();
  $build['content'] = array(
    'noscript' => array('#markup' => '<noscript><div class="alert error">Javascript is required. Please enable javascript in your browser.</div></noscript>'),
    'sba_social_action_preview_form' => drupal_get_form('sba_social_action_preview_form', $node),
  );

  return $build;
}

/**
 * Multi-step form definition.
 *
 * This form is mostly within a container that gets ajax replaced when the Oauth
 * login window popup is closed, which causes the form elements to
 * become non-disabled.
 *
 * @param $form
 * @param $form_state
 * @param $node
 *
 * @return mixed
 */
function sba_social_action_preview_form($form, $form_state, $node) {
  // Access variable to determine form element accessibility.
  $access = TRUE;
  $twitter_user_cache = session_cache_get('sba_twitter');
  if (empty($twitter_user_cache)) {
    $access = FALSE;
  }
  else {
    $twitter_user_cache['values']['oauth_token'] = decrypt($twitter_user_cache['values']['oauth_token']);
    $twitter_user_cache['values']['oauth_token_secret'] = decrypt($twitter_user_cache['values']['oauth_token_secret']);
  }

  $form['#attached']['css'][] = drupal_get_path('module', 'sba_social_action') . '/css/message-preview.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/jQuery.simplyCountable.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/social_counter.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/social_message_preview.js';
  $form['#attached']['js'][] = drupal_get_path('module', 'sba_social_action') . '/js/jquery.oauthpopup.js';

  // Invisible form element used to trigger ajax update when popup widow closes.
  $form['ajaxify'] = array(
    '#type' =>  'button',
    '#attributes' => array('style' => array('display:none')),
    '#value' => 'off',
    '#ajax' => array(
      'callback' => 'sba_social_actions_reload_form',
      'wrapper' => 'form-container',
      'effect' => 'fade',
      'speed' => 1000,
      'progress' => 'none'
    ),
  );

  // Ajax container
  $form['form_container'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="form-container">',
    '#suffix' => '</div>',
  );

  if (empty($twitter_user_cache)) {
    $form['form_container']['twitter_sign_in'] = array(
      '#type' => 'button',
      '#value' => 'Authorize with Twitter',
      '#attributes' => array('class' => array('twitter-sign'))
    );
  }

  // Build the form content out of the API response stored in session.
  // First sort the messages.
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
  $contact = $_SESSION['social_contact'];
  $action = $_SESSION['social_form_details'];
  // A couple values we'll need later
  $form['action_id'] = array(
    '#type' => 'value',
    '#value' => $action['action_id'],
  );
  $form['contact'] = array(
    '#type' => 'value',
    '#value' => $contact,
  );

  // Rebuild the messages for display
  $form['form_container']['messages'] = array (
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'class' => array('message-preview-container'),
    ),
  );
  // Used to send our second step form through the same helper functions as the first step form.
  module_load_include('inc', 'sba_social_action', 'includes/sba_social_action.form');

  foreach ($messages as $key => $message) {

    // What type of message is this.
    $message_entity = $node->messages[$message['message_id']];
    $message_entity_type = $node->messages[$message['message_id']]->type;

    // Theme the message header.
    $form['form_container']['messages'][$key]['sent_to'] = array(
      '#markup' => theme('sba_social_action_preview_message_header', array('message' => $message, 'type' => $message_entity_type)),
    );

    // Create the message form elements from the entity. Passed by reference
    // to the same helper function as the first step form.
    $build = array();
    sba_social_action_message_form($build, $message_entity, 'multi_flow', $message_entity_type);

    $this_message =& $build['message'][$message_entity->sba_message_id];
    $this_message['#attributes']['class'][] = 'message-preview-message-fieldset';

    // Build an array of target properties
    $this_message['target'] = array(
      '#type' => 'value',
      '#value' => array(
        'target_id' => $message['targets']->id,
        'twitter' => $message['targets']->twitter,
        'salutation' => $message['targets']->salutation,
        'first_name' => $message['targets']->first_name,
        'last_name' => $message['targets']->last_name
      ),
    );

    // Check for required fields
    if (isset($this_message['body_required']) && $this_message['body_required']['#value']) {
      $this_message['edited_body']['#required'] = TRUE;
      $this_message['edited_body']['#title'] =  t('You must edit the body of this message before sending');
    }
    if(isset($this_message['edited_body'])) {
      $this_message['edited_body']['#disabled'] = !$access;
    }

    if (isset($this_message['subject_required']) && $this_message['subject_required']['#value']) {
      $this_message['subject']['#required'] = TRUE;
      $this_message['subject']['#title'] =  t('Subject - You must edit this subject line before sending');
    }

    // Build the alternative social media links
    $alternates = $this_message['alternate_options']['#value'];
    $alt_links = array();
    if (!empty($alternates)) {
      foreach ($alternates as $alternate) {
        if (isset($message['targets']->{strtolower($alternate)})) {
          $alt_links[] =  l($alternate, $message['targets']->{strtolower($alternate)});
        }
      }
    }
    if (!empty($alt_links)) {
      $count = count($alt_links);
      $methods = format_plural($count, 'this other method', 'one of these other methods');
      $this_message['alternate'] = array(
        '#markup' => t('Or, if you prefer, please reach out to @target using @method: ', array('@method' => $methods, '@target' => $message['person']['name'])) . implode(' | ', $alt_links),
        '#prefix' => '<div class="sba-alternate-contact"><p>',
        '#suffix' => '</p></div>',
      );
    }
    // Assign the message to the messages container.
    $form['form_container']['messages'][$key]['message'] = $this_message;
  }

  // Following are some header area content which need to be here in the code
  //  to get ajax updated values.
  if($access == TRUE) {
    $markup = ('Please review the messages below; or if you prefer, simply <span id="multiflow-send-now-wrapper" style="display:inline-block"><a href="#" id="multiflow-send-now">send now</a></span>.');
  }
  else {
    $markup = t('In order to post, you must authorize our web app to access your Twitter account.');
  }
  $messages = $form['form_container']['messages'];
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
  $form['form_container']['page_description_label'] = array(
    '#prefix' => '<h4>',
    '#suffix' => '</h4>',
    '#weight' => -10,
    '#markup' => t('Step 2 of 2'),
  );
  if (isset($twitter_user_cache['values']['screen_name'])) {
    $form['form_container']['page_description_login'] = array(
      '#prefix' => '<div><p>',
      '#suffix' => '</p></div>',
      '#weight' => -9,
      '#markup' => t('You will be posting to Twitter as @%name. If that is not the correct account, !clickhere.', array(
        '%name' => $twitter_user_cache['values']['screen_name'],
        '!clickhere' => l('click here', 'clear/session', array('query' => array('destination' => 'node/' . $node->nid . '/social-message-preview')))
      )),
    );
  }
  if ($subject_required || $body_required) {
    $editable_but_not_required = FALSE;
    $form['form_container']['page_description_content']['#markup'] = t('One or more of the messages below requires editing. Please make your edits and then click "Send Now" at the bottom of the page.');
  }
  if (!empty($editable_but_not_required) && $access == TRUE) {
    $markup = t('Please review and edit the messages below; or if you prefer, simply <span id="multiflow-send-now-wrapper"><a href="#" id="multiflow-send-now">send now</a></span>.');
  }
  $form['form_container']['page_description_content'] = array(
    '#id' => 'social-action-preview-page',
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="social-action-preview-page"><p>',
    '#suffix' => '</p></div>',
    '#weight' => -8,
  );
  // End header area content

  // Add the node object to use in validation and submit.
  $form['#action_node'] = $node;
  // Define the validation function.
  $form['#validate'][] = 'sba_social_action_preview_form_validate';

  // Add submit button
  $form['form_container']['submit'] = array(
    '#prefix' => '<div id="edit-actions">',
    '#suffix' => '</div>',
    '#type' => 'submit',
    '#value' => t('Send Now'),
    '#weight' => 10,
    '#attributes' => array('class' => 'social-action-preview-submit'),
    '#disabled' => !$access,
  );
  return $form;
}

/**
 * Ajax callback.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function sba_social_actions_reload_form($form, $form_state) {
  return $form['form_container'];
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

  module_load_include('php', 'oauth_common', 'lib/OAuth');
  module_load_include('php', 'sba_social_action', 'includes/socialTwitter.lib');

  $node = $form['#action_node'];
  foreach ($form_state['values']['messages'] as $message) {

    $message_text = isset($message['message']['edited_body']) ? $message['message']['edited_body'] : $message['message']['body'];
    $target = parse_url($message['message']['target']['twitter']);
    $body = '.@' . substr($target['path'], 1) . ' ' . $message_text;

    if(empty($node->field_sba_test_mode)) {
      try {
        $twitter_user_cache = session_cache_get('sba_twitter');
        if(!empty($twitter_user_cache)) {
          $twitter_user_cache['values']['oauth_token'] = decrypt($twitter_user_cache['values']['oauth_token']);
          $twitter_user_cache['values']['oauth_token_secret'] = decrypt($twitter_user_cache['values']['oauth_token_secret']);

          $twitter = new socialTwitter(
            variable_get('social_action_twitter_consumer_key', ''),
            variable_get('social_action_twitter_consumer_secret', ''),
            $twitter_user_cache['values']['oauth_token'],
            $twitter_user_cache['values']['oauth_token_secret']
          );

          $twitter->statuses_update($body);
        }

      } catch (socialTwitterException $e) {
        drupal_set_message(t('An error occurred when posting to Twitter. Check the system log for details.'), 'error', TRUE);
      }
    }
    else {
      drupal_set_message('This action was in test mode. Messages were not sent.', 'warning', TRUE);
    }

  }

  // Get sba_social_action_build_message_bodies().
//  module_load_include('inc', 'sba_social_action', 'includes/sba_social_action.form');
//
//  $messages = $form_state['values']['messages'];

  // Add session flag to prevent resubmission of the form
  $_SESSION['social_action_completed'] = TRUE;

  // Redirect to the original confirmation page.
  // $sid is set in sba_social_action_multifow_submit()
  $sid = !empty($_SESSION['social_action_sid']['sid']) ? $_SESSION['social_action_sid']['sid'] : FALSE;
  if ($sid) {
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    $submission = webform_get_submission($node->nid, $sid);
    unset($_SESSION['social_action_sid']);
    if ($node->webform['redirect_url'] == '<confirmation>') {
      $form_state['redirect'] = array(
        'node/' . $node->nid . '/done',
        array('query' => array('sid' => $sid))
      );
    }
    else {
      $redirect = springboard_advocacy_parse_redirect($node, $submission);

      $form_state['redirect'] = $redirect;
      if (!empty($_SESSION['saved_messages']['status'])) {
        $_SESSION['messages']['status'] =  $_SESSION['saved_messages']['status'];
      }
    }
  }
  else {
    // we don't have a submission identifier, this is not a legit request, redirect to the node
    $form_state['redirect'] = array('node/' . $node->nid);
    if (!empty($_SESSION['saved_messages']['status'])) {
      $_SESSION['messages']['status'] =  $_SESSION['saved_messages']['status'];
    }
  }
  if (!empty($_SESSION['saved_messages']['status'])) {
    unset($_SESSION['saved_messages']['status']);
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
