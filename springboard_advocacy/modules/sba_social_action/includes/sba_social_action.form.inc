<?php

/**
 * @file
 * Helper functions for building message action client forms.
 */

/**
 * Message action webform alterations.
 *
 * Add the message entities to the action webform node view.
 * Attach CSS and JS to Action nodes.
 * If it's a multiple message action, attach the ctools modal view of them.
 * Define custom submit and validate hooks.
 *
 * @param $form
 * @param $form_state
 */
function sba_social_action_additional_elements(&$form, &$form_state) {
  // Override core's theme_form in order to remove <form> tag
  // and use it in a template variable defined in
  // sba_social_action_build_preprocess and
  // theme_social_action_form_layout_form
  if (!empty($form['#theme_wrappers']) && !empty($form['#node']->form_layouts) && $form['#node']->form_layouts != 'one_column') {
    foreach ($form['#theme_wrappers'] as $key => $wrapper) {
      if ($wrapper == 'form') {
        unset($form['#theme_wrappers'][$key]);
      }
    }
    $form['#theme_wrappers'][] = array('social_action_form_layout_form');
  }

  // Attach the JS and CSS.

  // Is form layouts enabled?
  if (!empty($form['#node']->form_layouts)) {
    $form['#attached']['css'][] = drupal_get_path('module', 'sba_social_action')
      . '/css/social-action-form-layouts.css';
  }
  else {
    $form['#attached']['css'][] = drupal_get_path('module', 'sba_social_action')
      . '/css/social-no-form-layouts.css';
  }

  $form['smarty_geo'] = array(
    '#type' => 'value',
    '#value' => '',
  );

  $form['webform_flat'] = array(
    '#type' => 'value',
    '#value' => array(),
  );

  // Create a container on the form for all messages.
  $form['sba_messages'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array(
      'id' => array('sba-messages-form'),
    ),
  );

  // Multiple messages for this form?
  $multiple = count($form['#node']->messages) > 1 ? TRUE : FALSE;

  $multi_flow = TRUE;
  $multi_flow_prompt = '';
  $action_type = 'single_not_multi_flow';
  if ($action_type == 'multi_flow') {
    $form['sba_messages']['multiflow'] = array(
      '#type' => 'fieldset',
      '#title' => t('Step 1 of 1'),
      '#attributes' => array('class' => array('sba-message')),
    );
    $form['sba_messages']['multiflow']['prompt'] =  array(
      '#markup' => $multi_flow_prompt,
    );
  }

  // Create a set of fields for each message assigned to the form.
  $districted = false;
  foreach ($form['#node']->messages as $message) {
    sba_social_action_message_form($form['sba_messages'], $message, $multiple, $message->type, $districted);
  }
  if ($districted == FALSE) {
    $form['submitted']['sbp_address']['#access'] = FALSE;
    $form['submitted']['sbp_address_line_2']['#access'] = FALSE;
    $form['submitted']['sbp_city']['#access'] = FALSE;
    $form['submitted']['sbp_state']['#access'] = FALSE;
    $form['submitted']['sbp_zip']['#access'] = FALSE;
  }

  $form['#validate'][] = 'sba_social_action_form_validate';
  array_unshift($form['#submit'], 'sba_social_action_form_submit');
}

/**
 * Build a form to represent each message attached to the action webform node.
 *
 * @param array $form
 *   The webform client form.
 * @param object $message
 *   The message entity.
 * @param bool $multiple
 *    Whether this webform has multiple messages.
 */
function sba_social_action_message_form(&$form, $message, $multiple, $message_entity_type, &$districted = NULL) {
  // Get some message values via an entity wrapper.
  $wrapper = entity_metadata_wrapper('sba_message', $message);
  $message_body = check_plain($wrapper->field_sba_twitter_message->value());
  $message_editable = $wrapper->field_sba_twitter_editable->value() == 'Not editable' ? FALSE : TRUE;
  $required_message = $wrapper->field_sba_twitter_editable->value() == 'Editing is required' ? TRUE : FALSE;
  $multi_flow = TRUE;
  $preview_form = empty($form) ? TRUE : FALSE;
  $access = TRUE;
  if (!$preview_form && $multi_flow) {
    $access = FALSE;
  }

  if($districted == FALSE) {
    $districted = empty($wrapper->field_sba_target_options->value()) ? FALSE : TRUE;
  }

  $form['message'][$message->sba_message_id] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('sba-message')),
  );

//  if(isset($_SESSION['sba_twitter']['values']['screen_name'])) {
//    $form['message']['page_description_login']['#prefix'] = '<p>';
//    $form['message']['page_description_login']['#markup'] = t('After you submit this form you will be redirected to a page which will allow you to send tweets to the targets we have selected. You will be able to post to Twitter as @%name. If that is not the correct account, !clickhere.', array(
//      '%name' => $_SESSION['sba_twitter']['values']['screen_name'],
//      '!clickhere' => l('click here', 'clear/session', array('query' => array('nid' => arg(1) )))
//    ));
//  }
//  else {
//    $form['message']['page_description_login']['#prefix'] = '<p>';
//    $form['message']['page_description_login']['#markup'] = t('After you submit this form you will be asked to sign into Twitter, and then redirected to a page which will allow you to send tweets to the targets we have selected.');
//  }

  $form['message'][$message->sba_message_id]['message_id'] = array(
    '#type' => 'value',
    '#value' => $message->data['message_id'],
  );

  $form['message'][$message->sba_message_id]['sba_message_id'] = array(
    '#type' => 'value',
    '#value' => $message->sba_message_id,
  );

  // The body value field is present in either case.
  // The value here is eventually passed to the API server.
  $form['message'][$message->sba_message_id]['body'] = array(
    '#type' => 'value',
    '#value' => $message_body,
  );

  if ($preview_form) {
    if ($required_message) {
      $description = '';
      $required = TRUE;
    }
    else {
      $description = '';
      $required = FALSE;
    }
    if(!$message_editable) {
      $form['message'][$message->sba_message_id]['body_show'] = array(
        '#type' => 'item',
        '#title' => '',
        '#markup' => $message_body,
        '#access' => $access,
      );

    }
    else {
      $form['message'][$message->sba_message_id]['edited_body'] = array(
        '#type' => 'textarea',
        '#default_value' => $message_body,
        '#description' => '<p>You have <span class="counter"></span> characters left.</p>',
        '#required' => $required,
        '#access' => $access,
      );
    }
  }

  $form['message'][$message->sba_message_id]['body_required'] = array(
    '#type' => 'value',
    '#value' => $required_message,
  );

}


/**
 * Validation handler for message action forms.
 *
 * Checks whether editing of subject or body is required.
 * Calls Smarty Streets API and injects response values into form state.
 * Flattens the webform submission tree and injects flattened values into
 * form state for use by the message sending functions
 * in sba_social_action.webform.inc
 *
 * @param $form
 * @param $form_state
 */
function sba_social_action_form_validate($form, &$form_state) {
  form_load_include($form_state, 'inc', 'springboard_advocacy', 'includes/springboard_advocacy.smarty_streets');
  $webform_values_flat = _springboard_advocacy_webform_submission_flatten($form['#node']->nid, $form_state['values']['submitted']);
  form_set_value($form['webform_flat'], $webform_values_flat, $form_state);
  $smarty_geo = springboard_advocacy_smarty_lookup($webform_values_flat);
  if (!empty($smarty_geo) && is_array($smarty_geo)) {
    form_set_value($form['smarty_geo'], $smarty_geo, $form_state);
    if (isset($smarty_geo['zip']) && isset($smarty_geo['plus4'])) {
      form_set_value($form['submitted']['sbp_zip_plus_four'], $smarty_geo['zip'] . '-' . $smarty_geo['plus4'], $form_state);
    }
  }
  else {
    form_set_error('', t('We could not verify your address. Please check that the street, city, state and zip code information is correct.'));
  }
}

/**
 * Submit handler for message action forms.
 *
 * Capture all messages, combine with contact information and
 * submit to API.
 *
 * @param $form
 * @param $form_state
 */
function sba_social_action_form_submit($form, &$form_state) {
  $_SESSION['delivered_messages'] = array();
  $_SESSION['undelivered_messages'] = array();
  $_SESSION['deliverable_social_messages'] = array();
  $_SESSION['undeliverable_social_messages'] = array();
  $_SESSION['contact'] = array();
  $_SESSION['form_details'] = array();
  $_SESSION['action_sid'] = FALSE;
  $_SESSION['action_completed'] = FALSE;

  // Create array of messages to submit for target resolution.
  if (isset($form_state['values']['sba_messages'])) {
    $messages = array();
    foreach ($form_state['values']['sba_messages']['message'] as $id => $message) {
      $edited_body = isset($message['edited_body']) ? $message['edited_body'] : '';
      $messages[$id]['body'] = sba_social_action_build_message_bodies($message['sba_message_id'], $edited_body);
      //$messages[$id]['subject'] = $message['subject'];
    }
    form_load_include($form_state, 'inc', 'sba_social_action', 'includes/sba_social_action.webform');
    $data = sba_social_action_send_message($messages, $form_state);
    // Calculate the deliverable count for sending to salesforce
    //@todo refactr for social url count
    if (!empty($data->messages)) {
      $deliverable = 0;
      foreach ($data->messages as $key => $message) {
        $deliverable += count($message->deliverable);
      }
      if (isset($form_state['values']['submitted']['sba_deliverable_count'])) {
        $form_state['values']['submitted']['sba_deliverable_count'] = $deliverable;
      }
    }
  }
  else {
    drupal_set_message(t('No messages were configured for this action.'));
  }
}


/**
 * Helper function to build message bodies.
 *
 * Combines user edited message data with other static properties
 * to create the final message to deliver to the transaction server.
 *
 * @param int $sba_message_id
 *   The message ID.
 * @param string $message_editable
 *    The user-editable body.
 *
 * @return string
 *   The message in string format.
 */
function sba_social_action_build_message_bodies($sba_message_id, $message_editable) {
  // Load the message entity using the message entity id. We'll use the entity
  // to construct the full message.
  $message = entity_load('sba_message', array($sba_message_id));
  $message = array_shift($message);
  $wrapper = entity_metadata_wrapper('sba_message', $message);

  $user_message = array();
  if(empty($message_editable)) {
    $user_message[] = $wrapper->field_sba_twitter_message->value();
  }
  else {
    $user_message[] = $message_editable;
  }
  $user_message = implode("\n\n", array_filter($user_message));

  return $user_message;
}
