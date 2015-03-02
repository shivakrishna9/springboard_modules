<?php

/**
 * Submit handler sends to Target Resolver on API server
 * 
 * Gets Zip+4 from Smarty streets.
 * Builds a webform user contact array with zip, county and lat/long
 * Gets the messages associated with the webform and prepares them for sending
 * to the API.
 * Merges the contact, messages, and form_id and sends to the target resolver.
 *
 */
function springboard_advocacy_webform_submit($form, $form_state) {
  module_load_include('inc', 'springboard_advocacy', 'includes/springboard_advocacy.smarty_streets');

  $values =$form_state['values'];
  $webform_values = $values['submitted_tree'];
  $geo = springboard_advocacy_smarty_lookup($webform_values);
  if (!empty($geo)) {
    $_SESSION['alert_geo'] = $geo;
    if(!empty($geo['zip']) && !empty($geo['plus4'])) {
       $zip = $geo['zip'] . '-' . $geo['plus4'];
    }
  }

  if(!empty($zip)) {
    $contact = springboard_advocacy_build_contact($webform_values, $zip, $geo);
    $form_id = $values['advocacy_id'];
    if(isset($values['messages'])) {
      $messages = springboard_advocacy_build_messages($values['messages']);

      $submission = array(
        'contact' => $contact,
        'form_id' => $form_id,
        'messages' => $messages,
      );
      $loader = new AdvocacyApiClientLoader();
      $client = $loader->getClient();
      $response = $client->resolveTargets($submission);
      if (!empty($response->data)) {
//      $persons = springboard_advocacy_prep_for_display($response->data);
//      $_SESSION['legislators'] = $persons;
      }
    }
  }
}

/**
 * Loads the messages associated with this webform and formats
 * them into an array for the target resolver.
 * @param $message_ids
 * @return array
 */
function springboard_advocacy_build_messages($message_ids) {
    $all_messages = entity_load('sba_message', $message_ids);
    $messages = array();
    $x = 0;
    foreach ($all_messages as $message) {
      $message_wrapper = entity_metadata_wrapper('sba_message', $message);
      $message_id = $message->data['message_id'];
      $weight = !empty($message->data['precedence']) ? $message->data['weight'] : $x++;
      //$group_id = $message->data['group_id'];
      $body = $message_wrapper->field_sba_message->value();
      $subject = $message_wrapper->field_sba_subject->value();
      $messages[$weight] = array(
        'message_id' => $message_id,
        'subject' => $subject,
        'body' => $body,
        'precedence' => isset($message->data['precedence']) ? 1 : 0,
       );
    }
    ksort($messages);
    return $messages;
}

/**
 * Merges the webform users contact data with data returned from smarty streets
 * in preparation for sending to target resolver.
 *
 * @param $values
 * @param $zip
 * @param $geo
 * @return array
 */
function springboard_advocacy_build_contact($values, $zip, $geo) {

  $contact = array(
    'first_name' => $values['sbp_first_name'],
    'last_name' => $values['sbp_last_name'],
    'address' => $values['sbp_address'], 
    'address_line_2' => $values['sbp_address_line_2'], 
    'city' => $values['sbp_city'],
    'state' => $values['sbp_state'],
    'phone' => $values['sba_phone'],
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
 * @todo complete this when wireframes finalized
 * Formats API response for presentaion by theme_item_list
 */
function springboard_advocacy_prep_for_display($data) {
  $items = array();

  foreach ($data as $legislator) {
    $items[]['data'] = $legislator->salutation . 
    ' ' . $legislator->first_name . 
    ' ' . $legislator->last_name . 
    ': ' . $legislator->district_name;
  }
  $title = t('Legislators');
  $attributes = array(
    'id' => 'legislator-list',
  );
  if (empty($items)) {
    $items[]['data'] = 'No Results';
  }
  return theme_item_list(array('items' => $items, 'title' => $title, 'type' => 'ul', 'attributes' => $attributes));;
}

/*
 * Implements hook_preprocess_webform_confirmation
 *
 * Show the list of legislators on the confirmation page
 */
function springboard_advocacy_preprocess_webform_confirmation(&$vars) {
  if ($vars['node']->type == 'sba_message_action') {
    if (!empty($_SESSION['legislators'])) {
      $vars['confirmation_message'] = $_SESSION['legislators'];
      unset($_SESSION['legislators']);
    }
  }
}

// /**
//  * Implements hook_webfrom_submission insert
//  *
//  * Save our letter writer's zip and geolocation info
//  */
// function springboard_advocacy_webform_submission_insert($node, $submission) {
//   if(isset($_SESSION['alert_geo']) && $node->type == 'springboard_advocacy') {
//     $data = array(
//       'nid' => $submission->nid,
//       'sid' => $submission->sid,
//       'uid' => $submission->uid,
//       'zip' => isset($_SESSION['alert_geo']['zip']) ? $_SESSION['alert_geo']['zip'] : '',
//       'plus4' => isset($_SESSION['alert_geo']['plus4']) ? $_SESSION['alert_geo']['plus4'] : '',
//       'lat' => isset($_SESSION['alert_geo']['latitude']) ? $_SESSION['alert_geo']['latitude'] : '',
//       'lon' => isset($_SESSION['alert_geo']['longitude']) ? $_SESSION['alert_geo']['longitude'] : '',
//     );
//     drupal_write_record('springboard_advocacy_location', $data);
//     unset($_SESSION['alert_geo']);
//   };
// }
