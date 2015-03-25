<?php

/**
 *
 * Gets Zip+4 from Smarty streets.
 * Builds a webform user contact array with zip, county and lat/long
 * Gets the messages associated with the webform and prepares them for sending
 * to the API.
 * Merges the contact, messages, and form_id and sends to the target resolver.
 *
 */


function springboard_advocacy_send_message($user_message, $form_state) {
  $values = $form_state['values'];
  $webform_values = $values['submitted'];
  $zip = '';

  if(isset($_SESSION['message_recipients'])) {
    unset($_SESSION['message_recipients']);
  }

  module_load_include('inc', 'springboard_advocacy', 'includes/springboard_advocacy.smarty_streets');

  $geo = springboard_advocacy_smarty_lookup($webform_values);

  if (!empty($geo) && !empty($geo['zip']) && !empty($geo['plus4'])) {
    $zip = $geo['zip'] . '-' . $geo['plus4'];
  }

  if(!empty($zip)) {
    $contact = springboard_advocacy_build_contact($webform_values, $zip, $geo);
    $form_id = $values['advocacy_id'];
    $message_build = springboard_advocacy_build_messages($user_message);

    $submission = array(
      'contact' => $contact,
      'form_id' => $form_id,
      'messages' => $message_build,
    );

    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();
    $response = $client->resolveTargets($submission);
    if(isset($response->data)) {
      springboard_advocacy_build_confirmation($response->data);
    }
    else {
      if (user_access('administer springboard advocacy')) {
        if (isset($response->error)) {
          drupal_set_message(t('An error occurred when attempting to resolve targets'), 'error');
        } else {
          drupal_set_message(t('A response was not received from the server when attempting to resolve targets'), 'error');
        }
      }
    }
  }
}

function theme_springboard_advocacy_webform_confirmations($vars) {
  $items = array();
  foreach ($vars['recipients'] as $person) {
    $items[]['data'] = $person;
  }
  $attributes = array(
    'class' => 'legislator-list',
  );
  if (empty($items)) {
    $items[]['data'] = 'No Results';
  }
  return theme_item_list(array('items' => $items, 'title' => $vars['subject'], 'type' => 'ul', 'attributes' => $attributes));;
}

/**
 * Loads the messages associated with this webform and formats
 * them into an array for the target resolver.
 * @param $message_ids
 * @return array
 */
function springboard_advocacy_build_messages($user_message) {
    $all_messages = entity_load('sba_message', array_keys($user_message));
    $messages = array();
    $x = 0;


    foreach ($all_messages as $message) {
      $message_wrapper = entity_metadata_wrapper('sba_message', $message);
//      $legislative_parent_terms = $message_wrapper->field_sba_alert_id  // field_sba_alert_id is a reference to the alert webform
//        ->field_sba_legislative_issues->value()
//      ;
//      $legislative_terms = springboard_advocacy_legislative_terms($legislative_parent_terms);

      $test_mode = $message_wrapper->field_sba_alert_id->field_test_mode[0]->value();
      $test_email = $message_wrapper->field_sba_alert_id->field_test_mode_email->value();

      $message_unique_id = $message->data['message_id'];
      $weight = !empty($message->data['precedence']) ? $message->data['weight'] : $x++;

      //$group_id = $message->data['group_id'];
      $messages[$weight] = array(
        'message_id' => $message_unique_id,
        'subject' => $user_message[$message->sba_message_id]['subject'],
        'body' => $user_message[$message->sba_message_id]['body'],
        'topic' => $legislative_terms,
        'precedence' => !empty($message->data['precedence']) ? 1 : 0,
        'test_mode' => !empty($test_mode) ? 1 : 0,
        'test_email' => !empty($test_email) ? $test_email : '',
      );
    }
    ksort($messages);
    return $messages;
}

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

function springboard_advocacy_build_confirmation($response) {
  $recipients = array();
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
    $_SESSION['message_recipients'] = t('@message_count @message_text delivered to @deliverable_count @deliverable_text.', array(
      '@message_count' => $message_count,
      '@message_text' => $message_text,
      '@deliverable_count' => $deliverable_count,
      '@deliverable_text' => $deliverable_text,
    ));

    foreach ($messages as $id => $message) {
      foreach ($messages[$id]['deliverable'] as $target) {

        $person = array();
        if(!empty($target->salutation)) {
          array_push($person, $target->salutation);
        }
        if(!empty($target->first_name)) {
          array_push($person, $target->first_name);
        }
        if(!empty($target->last_name)) {
          array_push($person, $target->last_name);
        }
        if(!empty($target->party)) {
          array_push($person, '(' . $target->party . ')');
        }
        if(!empty($target->state) && empty($target->district_name)) {
          array_push($person, $target->state);
        }
        if(!empty($target->district_name)) {
          array_push($person, $target->district_name);
        }

        $target_messages[$id]['people'][] = implode(' ', $person);
        $target_messages[$id]['subject'] = $messages[$id]['subject'];
      }
    }
    if(!empty($target_messages)) {
      foreach($target_messages as $id => $message) {
        $_SESSION['message_recipients'] .= theme('springboard_advocacy_webform_confirmations', array('subject' => $target_messages[$id]['subject'], 'recipients' => $message['people']));
      }
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
