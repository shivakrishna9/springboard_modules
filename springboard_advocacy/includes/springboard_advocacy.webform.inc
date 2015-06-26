<?php


/**
 * Gets Zip+4 from Smarty streets.
 *
 * Builds a user contact array with zip, county and lat/long.
 * Gets the messages associated with the webform and prepares them for sending
 * to the API.
 * Merges the contact, messages, and form_id and sends to the target resolver.
 *
 * @param array $user_message
 *    Message submitted by the user.
 * @param array $form_state
 *   The submitted form state.
 */
function springboard_advocacy_send_message(array $user_message, array $form_state) {
  $values = $form_state['values'];
  $webform_values = $values['submitted'];
  $zip = '';
  $response = '';

  // Unset the recipients from any prior submissions.
  if (isset($_SESSION['message_recipients'])) {
    unset($_SESSION['message_recipients']);
  }

  // Get the zip+4 from smarty streets.
  $geo = isset($form_state['values']['smarty_geo']) ? $form_state['values']['smarty_geo'] : array();
  if (!empty($geo) && !empty($geo['zip']) && !empty($geo['plus4'])) {
    $zip = $geo['zip'] . '-' . $geo['plus4'];
  }

  // We've got a full zip, so we can prepare the submission to send
  // to the resolver.
  if (!empty($zip)) {

    // Build the contact array.
    $contact = springboard_advocacy_build_contact($webform_values, $zip, $geo);

    // Build the message array.
    $message_build = springboard_advocacy_build_messages($user_message);
    // Build the submission array.
    $submission = array(
      'contact' => $contact,
      'form_id' => $values['advocacy_id'],
      'messages' => $message_build,
    );
    $node = $form_state['complete form']['#node'];
    $test_mode = field_get_items('node', $node, 'field_sba_test_mode');
    if (!empty($test_mode[0]['value']) && $test_mode[0]['value'] == 1) {
      $test_email = field_get_items('node', $node, 'field_sba_test_mode_email');
      $submission['test_mode'] = TRUE;
      $submission['test_email'] = !empty($test_email[0]['safe_value']) ? $test_email[0]['safe_value'] : '';
    }

    // Send submission to API Client.
    $api_call = new AdvocacyApiCall();
    $response = $api_call->invokeClientMethod('resolveTargets', $submission);
    if (isset($response->data)) {
      // We got a valid response, build the confirmation message array.
      springboard_advocacy_build_confirmation($response->data, $contact);
    }
  }
  return !empty($response->data) ? $response->data : NULL;
}

/**
 * Loads the messages associated with this webform.
 *
 * Formats them into an array for the target resolver.
 *
 * @param array $user_message
 *   The submitted messages.
 *
 * @return array
 *   Messages formatted for the API call.
 */
function springboard_advocacy_build_messages(array $user_message) {
  $all_messages = entity_load('sba_message', array_keys($user_message));
  $messages = array();
  $x = 0;

  foreach ($all_messages as $message) {
    $message_wrapper = entity_metadata_wrapper('sba_message', $message);

    $message_unique_id = $message->data['message_id'];
    $weight = !empty($message->data['precedence']) ? $message->data['weight'] : $x++;

    // Attach legislative issue search terms.
    $legislative_terms = array("Other");
    // Provide a generic default.
    if ($message_wrapper->field_sba_action_id
      ->__isset('field_sba_legislative_issues')) {
      // Field_sba_action_id is a reference to the action webform.
      $legislative_parent_terms = $message_wrapper->field_sba_action_id
        ->field_sba_legislative_issues->value();
      $legislative_terms = springboard_advocacy_legislative_terms($legislative_parent_terms);
    }

    $messages[$weight] = array(
      'message_id' => $message_unique_id,
      'subject' => $user_message[$message->sba_message_id]['subject'],
      'body' => $user_message[$message->sba_message_id]['body'],
      'topic' => $legislative_terms,
      'precedence' => !empty($message->data['precedence']) ? 1 : 0,
    );
  }
  ksort($messages);
  return $messages;
}

/**
 * Parse the taxonomy tree.
 *
 * @param array $parent_terms
 *   The parent terms.
 *
 * @return array
 *   Terms merged with their children.
 */
function springboard_advocacy_legislative_terms($parent_terms) {
  $terms = array();
  if (!empty($parent_terms)) {
    $tree = taxonomy_get_tree($parent_terms[0]->vid);
    foreach ($parent_terms as $parent_term) {
      $children = _springboard_advocacy_legislative_terms_descendents($parent_term->tid, $tree);
      $terms = array_merge($terms, $children);
    }
  }
  return $terms;
}

/**
 * Helper function.
 *
 * @param int $ancestor_tid
 * @param array $tree
 *
 * @return array
 */
function _springboard_advocacy_legislative_terms_descendents($ancestor_tid, $tree) {
  $children = array();
  foreach ($tree as $term) {
    foreach ($term->parents as $parent_tid) {
      if ($parent_tid == $ancestor_tid || !empty($children[$parent_tid])) {
        $children[$term->tid] = $term->name;
      }
    }
  }
  return $children;
}

/**
 * Merges the webform users contact data with data returned from smarty streets.
 *
 * Preparation for sending to target resolver.
 *
 * @param array $values
 *    Springboard Profile Values.
 * @param string $zip
 *   A zip + 4 code.
 * @param array $geo
 *   Array of address data returned from Smarty Streets lookup.
 *
 * @return array
 *   Contact's profile data plus smarty data.
 */
function springboard_advocacy_build_contact(array $values, $zip, array $geo) {

  $line_2 = !empty($values['sbp_address_line_2']) ? $values['sbp_address_line_2'] . "\n " : '';
  $contact = array(
    'first_name' => $values['sbp_first_name'],
    'last_name' => $values['sbp_last_name'],
    'address' => $values['sbp_address'],
    'address_line_2' => $values['sbp_address_line_2'],
    'city' => $values['sbp_city'],
    'state' => $values['sbp_state'],
    'phone' => $values['sbp_phone'],
    'zip' => $zip,
    'full_address' => $values['sbp_address'] . "\n"  . $line_2   . $values['sbp_city'] . ", "  . $values['sbp_state'] . " " . $zip,
    'email' => $values['mail'],
    'county' => isset($geo['county']) ? $geo['county'] : '',
    'fips' => isset($geo['fips']) ? $geo['fips'] : '',
    'latitude' => isset($geo['latitude']) ? $geo['latitude'] : '',
    'longitude' => isset($geo['longitude']) ? $geo['longitude'] : '',
  );

  return $contact;
}

/**
 * Create a confirmation message from the API server response.
 *
 * Then place it into Session for use by the confirmation page token.
 *
 * @param object $response
 *   The API response.
 */
function springboard_advocacy_build_confirmation($response, $contact) {

  if (!empty($response->messages)) {
    $_SESSION['message_recipients'] = '';

    $status = springboard_advocacy_message_status($response);
    unset($status['success']['target_count']);
    if (!empty($status['success'])) {
      foreach ($status['success'] as $id => $message) {
        $_SESSION['message_recipients'] .= theme(
          'springboard_advocacy_webform_confirmations_delivered',
          array(
            'subject' => $status['success'][$id]['subject'],
            'recipients' => $message['people'],
          )
        );
      }
    }

    unset($status['hold']['target_count']);
    if (!empty($status['hold'])) {
      foreach ($status['hold'] as $id => $message) {
        $_SESSION['message_recipients'] .= theme(
          'springboard_advocacy_webform_confirmations_delivered',
          array(
            'subject' => $status['hold'][$id]['subject'],
            'body' => $status['hold'][$id]['body'],
            'recipients' => $message['people'],
            'contact' => $contact,
            'targets' => $message['targets'],

          )
        );
      }
    }
  }
}

/**
 * Split up the "deliverable" response into "success" and "hold".
 *
 * Some deliverable target webforms may be temporarily out of action and
 * the target object in the response will have a "webform_status" property
 * set to "hold". If so, we split those off into a separate array for display
 * on the confirmation page.
 *
 * @param object $response
 *   The API server response.
 *
 * @return array
 *   An array of messages.
 */
function springboard_advocacy_message_status($response) {

  $messages = array();
  $message_success = array();
  $message_hold = array();
  $success_count = 0;
  $hold_count = 0;

  foreach ($response->messages as $id => $message) {
    if (!empty($message->deliverable)) {
      $messages[$id]['deliverable'] = $message->deliverable;
      $messages[$id]['subject'] = $message->message->subject;
      $messages[$id]['body'] = $message->message->body;
    }
  }

  foreach ($messages as $id => $message) {
    foreach ($messages[$id]['deliverable'] as $target) {

      // Filter the largish target object in the response down to
      // a few select properties, in an array
      $target_details = springboard_advocacy_target_details($target);

      // Split into success and hold arrays.
      if (empty($target->webform_status)
        || (!empty($target->webform_status) && $target->webform_status != 'Hold')) {
        $success_count++;
        $message_success['target_count'] = $success_count;
        unset($target_details['webform_url']);
        $message_success[$id]['people'][] = implode(' ', $target_details);
        $message_success[$id]['subject'] = $messages[$id]['subject'];
      }
      else {
        $hold_count++;
        $message_hold['target_count'] = $hold_count;
        $message_hold[$id]['people'][] = implode(' ', $target_details);
        $message_hold[$id]['subject'] = $messages[$id]['subject'];
        $message_hold[$id]['body'] = $messages[$id]['body'];
        $message_hold[$id]['targets'][] = $target;
      }
    }
  }
  return array('success' => $message_success, 'hold' => $message_hold);
}


/**
 * Creates an array of recipients for each message.
 *
 * An array with name and organization details
 * that eventually gets converted to a readable string in the
 * confirmation theme function.
 *
 * If the recipient is a congressperson whose webform_status is "hold"
 * we add a popup link pointing to their webform.
 *
 * @param object $target
 *   A target object extracted from the API response.
 *
 * @return array
 *   An array containing a subset of target properties.
 */
function springboard_advocacy_target_details($target) {

  $name = array();
  $target_details = array();

  if (!empty($target->salutation)) {
    array_push($name, $target->salutation);
    array_push($target_details, $target->salutation);
  }
  if (!empty($target->first_name)) {
    array_push($name, $target->first_name);
    array_push($target_details, $target->first_name);
  }
  if (!empty($target->last_name)) {
    array_push($name, $target->last_name);
    array_push($target_details, $target->last_name);
  }
  if (!empty($target->party)) {
    array_push($target_details, '(' . $target->party . ')');
  }
  if (!empty($target->state) && empty($target->district_name)) {
    array_push($target_details, $target->state);
  }
  if (!empty($target->district_name)) {
    array_push($target_details, $target->district_name);
  }

  // Run the details through check_plain().
  foreach ($target_details as $key => $detail) {
    $target_details[$key] = check_plain($detail);
  }

  // Add the URL, if exists, for webforms that are on 'hold'.
  // Javascript popup link.
  if (!empty($target->webform_url)) {
    $name = implode(' ', $name);
    $clicker = "var extWrapper=window.open(this.href,'targetWindow','toolbar=no,scrollbars=yes,status=no,menubar=no,resizable=yes,top=0,left=0,width=1024,height=800');extWrapper.focus();return false;";
    $link = l(t('Click here'), check_plain($target->webform_url), array('attributes' => array('onclick' => $clicker)));
    array_push($target_details, t('. !click to contact @name directly.', array('!click' => $link, '@name' => $name)));
  }

  return $target_details;
}


/**
 * Theme the message recipients in an item list for each message.
 *
 * @param array $vars
 *   Array of message recipients.
 *
 * @return string
 *   Rendered HTML.
 */
function theme_springboard_advocacy_webform_confirmations_delivered(array $vars) {

  // Put the recipients in an array for the item list.
  $items = array();
  foreach ($vars['recipients'] as $person) {
    $items[]['data'] = $person;
  }
  if (empty($items)) {
    $items[]['data'] = t('No Results.');
  }

  $attributes = array(
    'class' => 'legislator-list',
  );

  // If the body was not passed to the theme function, that means this
  // was a "success" delivery.
  if(empty($vars['body'])) {
    $output =  theme_item_list(
      array(
        'items' => $items,
        'title' => t('Delivered').': ' . html_entity_decode($vars['subject']),
        'type' => 'ul',
        'attributes' => $attributes,
      )
    );
  }
  else {
    // It was a "hold" delivery.
    $output = theme_item_list(
      array(
        'items' => $items,
        'title' => t('Not Delivered').': '. html_entity_decode($vars['subject']),
        'type' => 'ul',
        'attributes' => $attributes,
      )
    );

    // For "hold" status, we output the message text so the user can copy
    // and paste it into the congressional webform herself.
    $output .= '<h4 class="message-subject">Message text</h4>';
    // Prepare the contact object so we can replace tokens in the message.
    $contact = (object) $vars['contact'];
    // Prepare the target object so we can replace tokens in the message.
    // If there was only a single "hold" target, we can replace the target
    // token with the target's name. Otherwise create a pseudo target object
    // with last_name = "Multitple Recipients".
    if (count($vars['targets']) === 1) {
      $target = $vars['targets'][0];
    }
    else {
      $target = new stdClass();
      $target->last_name = '(Multiple Recipients)';
    }
    // Replace the tokens.
    $vars['body'] = token_replace(
      $vars['body'],
      array(
        'sba_contact' => $contact,
        'sba_target' => $target,
      )
    );
    // Convert linebreaks to html.
    $output .= check_markup($vars['body'], 'filtered_html');
  }

  return $output;
}

