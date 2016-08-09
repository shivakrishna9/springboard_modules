<?php
/**
 *
 * Save the target message data to the API server
 * @param stdClass $group
 *   ->group_name string
 *   ->group_id int
 *   ->recipients json encoded string
 *   ->is_template boolean
 *   ->message array
 *     message_id => string
 *     form_id => string
 *
 * @param boolean $is_new
 * @return $group_id
 *   The API server group ID. Returns FALSE if there is no valid response.
 *
 *  * The api is expecting an array structured like so:
 *  array(
 *    'name' => 'group name', //required
 *    'is_template' => 0, //required boolean
 *    'chambers' => array(
 *         0 => array('chamber' => 'fs', 'party =>'r', state => 'MA'),
 *         1 => array('chamber' => 'fs', 'party =>'d', state => 'MA')
 *       )
 *    'executives' => array(
 *         0 => array('role' => 'PRES01', 'party =>'r'),
 *         1 => array('role' => 'PRES03', 'party =>'d')
 *       )
 *     )
 *     'target_ids' => array('1', '2', '3', '4', '5', '6', '7'),
 *     'child_group_ids' => array('123', '456', '789'),
 *     'message' => array(
 *       group_id =>
 *       message_id =>
 *       form_id =>
 *      ),
 *   )
 */

function springboard_target_api_save_target_group($group, $is_new = TRUE) {
  // Set up target components for the Group
  $indvds = array();
  $child_ids = array();
  $multi_indvds = array();
  $has_gender_or_social = array();
  $gender_or_social_rows = array();
  $chamber_rows = array();
  $unstructured_chambers = array();

  // If recipients have been chosen, decode the recipients string and
  // structure the items into the expected arrays.
  if (!empty($group['recipients'])) {
    $recipients = html_entity_decode($group['recipients']);
    unset($group['recipients']);
    $recipients = urldecode($recipients);
    $targets = json_decode($recipients);
    foreach ($targets as $target) {
      // group targets
      if (isset($target->id) && $target->type == 'group') {
        $child_ids[$target->id] = $target->id;
      }
      // single individual target
      elseif (isset($target->id)) {
        $indvds[$target->id] = $target->id;
      }
      // multiple individual targets
      elseif (isset($target->ids)) {
        $multi_indvds =  array_merge($multi_indvds, explode('|', $target->ids));
      }
      // gender or social media pseudo-groups
      elseif (isset($target->gender) || isset($target->social)) {
        $has_gender_or_social[] = $target;
      }
      // legislative pseudo-groups
      else {
        $unstructured_chambers[] = $target;
      }
    }
  }

  // If there are gender or social pseudo-groups in use, retrieve the actual
  // individual target IDs
  if (!empty($has_gender_or_social)) {
    $gender_or_social_rows = _springboard_target_api_get_gender_or_social_ids($has_gender_or_social);
  }
  // Format the legislative group targets into the API-expected format.
  if (!empty($unstructured_chambers)) {
    $chamber_rows = _springboard_target_api_build_chambers($unstructured_chambers);
  }

  // Combine all individual targets into one array.
  $combined = array_merge($multi_indvds, $indvds, $gender_or_social_rows);
  $indvds = !empty($combined) ? json_encode($combined) : '';

  // Set the Group components for each type of target.
  $group['chambers'] = $chamber_rows;
  $group['target_ids'] = $indvds;
  $group['custom_group_ids'] = $child_ids;

  $api_call = springboard_advocacy_api_call();
  $group_id = isset($group['group_id']) ? $group['group_id'] : NULL;

  // Is this a new message? Then create a new target group for it.
  if ($is_new) {
    $response = $api_call->invokeClientMethod('createTargetGroup', $group);
    if (!empty($response->data->id)) {
      $group_id = $response->data->id;
    }
    else {
      $group_id = 'FALSE';
    }
  }
  elseif (!$is_new) {
    // In the case of an existing Message that failed to create its group on the
    // advocacy server when initially created, attempt to create a new group.
    if ($group['group_id'] == 'no_response' && isset($group['message'])) {
      $response = _springboard_target_api_get_group_by_message_id($api_call, $group['message']['message_id']);
      if (!empty($response) && !empty($response->data->id)) {
        $group['group_id'] = $response->data->id;
        $response = $api_call->invokeClientMethod('updateTargetGroup', $group, $group_id);
      }
      else {
        unset($group['group_id']);
        $response = $api_call->invokeClientMethod('createTargetGroup', $group);
      }
    }
    // Uncomplicated group updates.
    else {
      $response = $api_call->invokeClientMethod('updateTargetGroup', $group, $group_id);
    }
    // set the group ID based on the API response.
    if (!empty($response->data->id)) {
      $group_id = $response->data->id;
    }
    else {
      $group_id = FALSE;
    }
  }
  return $group_id;
}

/**
 * @param $target_fields
 * @return array
 */
function _springboard_target_api_get_gender_or_social_ids($target_fields) {

  $api_call = springboard_advocacy_api_call();

  $ids = array();
  foreach ($target_fields as $fields) {
    $fields->limit = 1;
    $response = $api_call->invokeClientMethod('searchTargets', (array) $fields);

    if (!empty($response->data->ids)) {
      $ids = array_unique(array_merge($ids, $response->data->ids));
    }
  }
  return $ids;
}

/**
 * Take the chambers form input and format it for the API callback.
 * @param $unstructured_chambers
 * @return array
 */
function _springboard_target_api_build_chambers($unstructured_chambers) {
  $chamber_rows_container = array();

  // An "Unstructured chambers" example:
  //    0 (Object)
  //        role => FR
  //        party => D
  //    1 (Object)
  //        role => PRES01|PRES03|GOVNR
  //    2 (Object)
  //        state => NY
  //        role => SR|SS


  //Unstructured chambers above will become:
  //0 =>
  //    chamber =>FR
  //    party  => D
  //1 =>
  //    chamber => PRES01
  //    party =>
  //2 =>
  //    chamber => PRES03
  //    party =>
  //3 =>
  //    chamber => GOVNR
  //    party =>
  //4 =>
  //    chamber => SR
  //    party =>
  //    state => NY
  //5 =>
  //    chamber => SS
  //    party =>
  //    state => NY

  foreach ($unstructured_chambers as $chamber_id => $chamber_fields) {

    //create a new container to rewrap our chambers
    $chamber_rows_container[$chamber_id] = array();

    foreach ($chamber_fields as $field_name => $chamber_field_values) {
      $chamber_field_values = explode('|', $chamber_field_values);
      if ($field_name == 'role') {
        $field_name = 'chamber';
      }

      $i = 0;
      $num = count($chamber_rows_container[$chamber_id]);

      if ($num) {
        foreach ($chamber_rows_container[$chamber_id] as $chamber_field) {
          foreach ($chamber_field_values as $value) {
            _springboard_target_api_build_chambers_shmoosher($chamber_rows_container, $chamber_id, $i, $field_name, $value, $chamber_field);
            $i++;
          }
        }
      } else {
        foreach ($chamber_field_values as $value) {
          _springboard_target_api_build_chambers_shmoosher($chamber_rows_container, $chamber_id, $i, $field_name, $value);
          $i++;
        }
      }
    }
  }

  $chamber_rows = array();
  foreach ($chamber_rows_container as $container)  {
    $chamber_rows = array_merge($chamber_rows, $container);
  }
  return $chamber_rows;
}

/**
 * Helper function to build chamber rows
 * @param $chamber_rows_container
 * @param $chamber_id
 * @param $i
 * @param $field_name
 * @param $value
 * @param array $chamber_field
 */
function _springboard_target_api_build_chambers_shmoosher(&$chamber_rows_container, $chamber_id, $i, $field_name, $value, $chamber_field = array()) {

  // the container is passed by reference
  // make sure that values that are already set, get maintained.
  $chamber = isset($chamber_field['chamber']) ? $chamber_field['chamber'] : '';
  $party = isset($chamber_field['party']) ? $chamber_field['party'] : '';
  $state = isset($chamber_field['state']) ? $chamber_field['state'] : '';
  //$gender = isset($chamber_field['gender']) ? $chamber_field['gender'] : '';

  $chamber_rows_container[$chamber_id][$i]['chamber'] = $chamber;
  $chamber_rows_container[$chamber_id][$i]['party'] = $party;
  $chamber_rows_container[$chamber_id][$i]['state'] = $state;
  //$chamber_rows_container[$chamber_id][$i]['gender'] = $gender;

  $chamber_rows_container[$chamber_id][$i][$field_name] = $value;
}

/**
 * If a group ID is missing in an existing message entity
 * try to retrieve it via the message id. This would happen if the
 * group creation fails or if the creation response was interrupted.
 *
 * @param $api_call
 * @param $message_id
 * @return mixed
 */
function _springboard_target_api_get_group_by_message_id($api_call, $message_id) {
  $response = $api_call->invokeClientMethod('getTargetGroupByMessageID', $message_id);
  return $response;
}

/**
 * @param $entity
 */
function springboard_target_api_delete_target_group ($entity) {
  //delete the remote API message group.
  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('deleteTargetGroup', $entity->data['group_id']);

  if (isset($response->data)) {
    drupal_set_message(t("The group was deleted."));
  }
}
