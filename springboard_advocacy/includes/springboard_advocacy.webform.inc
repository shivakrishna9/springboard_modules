<?php

/**
 * Submit handler
 * 
 * Gets Zip+4 from Smarty streets.
 * Asks the Advocacy API for a list of Legislators for the zip.
 * Places list in session for use on confirmation page.
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

function springboard_advocacy_build_messages($message_ids) {
    $all_messages = entity_load('sba_message', $message_ids);
    $messages = array();
    foreach ($all_messages as $message) {
      $message_wrapper = entity_metadata_wrapper('sba_message', $message);
      $message_id = $message->data['message_id'];
      //$group_id = $message->data['group_id'];
      $body = $message_wrapper->field_sba_message[0]->value();
      $subject = $message_wrapper->field_sba_subject[0]->value();
      $messages[] = array(
        'message_id' => $message_id,
        'subject' => $subject,
        'body' => $body,
       );
    }
    return $messages;
}

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
//    'county' => $geo['county'],
//    'fips' => $geo['fips'],
  );

  return $contact;
}


/**
 * Formats API response for presentaion by theme_item_list
 * 
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
  if ($vars['node']->type == 'springboard_letter') {
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
