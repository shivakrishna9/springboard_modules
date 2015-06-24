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

  // Unset the recipients from any prior submissions.
  if (isset($_SESSION['message_recipients'])) {
    unset($_SESSION['message_recipients']);
  }

  // Get the zip+4 from smarty streets.
//  module_load_include('inc', 'springboard_advocacy', 'includes/springboard_advocacy.smarty_streets');
//  $geo = springboard_advocacy_smarty_lookup($webform_values);
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
      springboard_advocacy_build_confirmation($response->data);
      return $response->data;
    }
  }
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
function theme_springboard_advocacy_webform_confirmations(array $vars) {
  $items = array();
  foreach ($vars['recipients'] as $person) {
    $items[]['data'] = check_plain($person);
  }

  $attributes = array(
    'class' => 'legislator-list',
  );
  if (empty($items)) {
    $items[]['data'] = 'No Results';
  }
  return theme_item_list(
    array(
      'items' => $items,
      'title' => check_plain($vars['subject']),
      'type' => 'ul',
      'attributes' => $attributes,
    )
  );
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
 * @param $ancestor_tid
 * @param $tree
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

  $contact = array(
    'first_name' => $values['sbp_first_name'],
    'last_name' => $values['sbp_last_name'],
    'address' => $values['sbp_address'],
    'address_line_2' => $values['sbp_address_line_2'],
    'city' => $values['sbp_city'],
    'state' => $values['sbp_state'],
    'phone' => $values['sbp_phone'],
    'zip' => $zip,
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
function springboard_advocacy_build_confirmation($response) {

  $messages = array();
  $message_count = 0;
  $deliverable_count = 0;

  if (!empty($response->messages)) {
    foreach ($response->messages as $id => $message) {
      if (!empty($message->deliverable)) {
        $message_count++;
        $deliverable_count += count($message->deliverable);
        $messages[$id]['deliverable'] = $message->deliverable;
        $messages[$id]['subject'] = $message->message->subject;
        $messages[$id]['body'] = $message->message->body;
      }
    }

    $message_text = format_plural($message_count, 'message was', 'messages were');
    $deliverable_text = format_plural($deliverable_count, 'target', 'targets');
    if ($deliverable_count > 0) {
      $_SESSION['message_recipients'] = t('@message_count @message_text delivered to @deliverable_count @deliverable_text.', array(
        '@message_count' => $message_count,
        '@message_text' => $message_text,
        '@deliverable_count' => $deliverable_count,
        '@deliverable_text' => $deliverable_text,
      ));

      foreach ($messages as $id => $message) {
        foreach ($messages[$id]['deliverable'] as $target) {

          $person = array();
          if (!empty($target->salutation)) {
            array_push($person, $target->salutation);
          }
          if (!empty($target->first_name)) {
            array_push($person, $target->first_name);
          }
          if (!empty($target->last_name)) {
            array_push($person, $target->last_name);
          }
          if (!empty($target->party)) {
            array_push($person, '(' . $target->party . ')');
          }
          if (!empty($target->state) && empty($target->district_name)) {
            array_push($person, $target->state);
          }
          if (!empty($target->district_name)) {
            array_push($person, $target->district_name);
          }

          $target_messages[$id]['people'][] = implode(' ', $person);
          $target_messages[$id]['subject'] = $messages[$id]['subject'];
        }
      }
      if (!empty($target_messages)) {
        foreach ($target_messages as $id => $message) {
          $_SESSION['message_recipients'] .= theme('springboard_advocacy_webform_confirmations', array('subject' => $target_messages[$id]['subject'], 'recipients' => $message['people']));
        }
      }
    }
  }
}
