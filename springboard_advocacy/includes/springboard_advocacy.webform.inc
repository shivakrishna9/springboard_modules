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
  $values = $form_state['values']['submitted_tree'];
  $geo = springboard_advocacy_smarty_lookup($values);

  if (!empty($geo)) {
    $_SESSION['message_geo'] = $geo;
    if(!empty($geo['zip']) && !empty($geo['plus4'])) {
       $zip = $geo['zip'] . '-' . $geo['plus4'];
    }
  }

  if(!empty($zip)) {
    $values = $form_state['values']['submitted_tree'];
    $contact = springboard_advocacy_build_contact($values, $geo);
    $message =array(
      'form_id' => $values['advocacy_id'],
      'message' => $values['sba_message'],
      'subject' => $values['sba_message_subject'],
      'message_id' => '',
    );

    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();
    $response = $client->resolveTargets($contact, $message);

    if (!empty($response->data)) {
      $persons = springboard_advocacy_prep_for_display($response->data);
      $_SESSION['legislators'] = $persons;
    }

  }
}


function springboard_advocacy_build_contact($form_values, $geo) {

  $contact = array(
    'first_name' => $values['sbp_first_name'],
    'last_name' => $values['sbp_last_name'],
    'address' => $values['sbp_address'], 
    'address_line_2' => $values['sbp_address_line_2'], 
    'city' => $values['sbp_city'],
    'state' => $values['sbp_state'],
    'phone' => $values['springboard_advocacy_phone'], 
    'zip' => $values['sbp_zip'], 
    'email' => $values['mail'],
    'county' => $geo['county'],
    'fips' => $geo['fips'],
  );

  return $contact;
}


/**
 * Formats API response for presentaion by theme_item_list
 * 
 */
function springboard_advocacy_prep_for_display($data) {
  $items = array();

  foreach ($data as $legistor) {
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
//   if(isset($_SESSION['message_geo']) && $node->type == 'springboard_advocacy') {
//     $data = array(
//       'nid' => $submission->nid,
//       'sid' => $submission->sid,
//       'uid' => $submission->uid,
//       'zip' => isset($_SESSION['message_geo']['zip']) ? $_SESSION['message_geo']['zip'] : '',
//       'plus4' => isset($_SESSION['message_geo']['plus4']) ? $_SESSION['message_geo']['plus4'] : '',
//       'lat' => isset($_SESSION['message_geo']['latitude']) ? $_SESSION['message_geo']['latitude'] : '',
//       'lon' => isset($_SESSION['message_geo']['longitude']) ? $_SESSION['message_geo']['longitude'] : '',
//     );
//     drupal_write_record('springboard_advocacy_location', $data);
//     unset($_SESSION['message_geo']);
//   };
// }
