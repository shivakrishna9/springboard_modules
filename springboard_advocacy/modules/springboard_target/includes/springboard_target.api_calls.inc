<?php

/**
 * Save the target message data to the API server.
 *
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
 *    'chambers' => json_encode(array(
 *         0 => array('chamber' => 'fs', 'party =>'r', state => 'MA'),
 *         1 => array('chamber' => 'fs', 'party =>'d', state => 'MA')
 *       )),
 *     'target_ids' => json_encode(array('1', '2', '3', '4', '5', '6', '7')),
 *     'child_group_ids' => json_encode(array('123', '456', '789')),
 *     'message' => array(
 *       group_id =>
 *       message_id =>
 *       form_id =>
 *      ),
 *   )
 */
function springboard_target_api_save_target_group($group, $is_new = TRUE) {
  // Set up target components for the .
  $indvds = array();
  $child_ids = array();
  $multi_indvds = array();
  $committees = array();
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
      // Group targets.
      if (isset($target->id) && $target->type == 'group') {
        $child_ids[$target->id] = $target->id;
      }
      // Single individual target.
      elseif (isset($target->id)) {
        $indvds[$target->id] = $target->id;
      }
      // Multiple individual targets.
      elseif (isset($target->ids) && !isset($target->search_committee)) {
        $multi_indvds = array_merge($multi_indvds, explode('|', $target->ids));
      }
      // All others.
      else {
        $unstructured_chambers[] = $target;
      }
    }
  }

  // Check child groups for recursion.
  if (!empty($group['group_id'])) {
    foreach ($child_ids as $child_id) {
      if (_springboard_target_api_check_group_recursion($child_id, $group['group_id'])) {
        return FALSE;
      }
    }
  }
  
  if (!empty($unstructured_chambers)) {
    $chamber_rows = _springboard_target_api_build_chambers($unstructured_chambers);
  }

  // Combine all individual targets into one array.
  $combined = array_merge($multi_indvds, $indvds);
  $indvds = !empty($combined) ? json_encode($combined) : '';

  // Set the Group components for each type of target.
  $group['chambers'] = $chamber_rows;
  $group['target_ids'] = $indvds;
  $group['custom_group_ids'] = $child_ids;

  // $group['chambers'] = array();
  // $group['target_ids'] = json_encode(array('8', '10', '111'));
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
        $response = $api_call->invokeClientMethod('updateTargetGroup', $group, $group['group_id']);
      }
      else {
        unset($group['group_id']);
        $response = $api_call->invokeClientMethod('createTargetGroup', $group);
      }
    }
    // Uncomplicated group updates.
    else {
      $response = $api_call->invokeClientMethod('updateTargetGroup', $group, $group['group_id']);
    }
    // Set the group ID based on the API response.
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
 * Take the chambers form input and format it for the API callback.
 *
 * Convert the chamber fields to array and format the keywords field.
 */
function _springboard_target_api_build_chambers($unstructured_chambers) {
  foreach ($unstructured_chambers as $chamber_id => $chamber_fields) {
    $unstructured_chambers[$chamber_id] = (array) $chamber_fields;
    foreach ($chamber_fields as $field_name => $value) {
      if ($field_name == 'fields' || $field_name == 'values') {
        $unstructured_chambers[$chamber_id]['keywords'][$field_name] = $value;
        unset($unstructured_chambers[$chamber_id][$field_name]);
      }
    }
  }
  return $unstructured_chambers;
}

/**
 * Failsafe group lookup.
 *
 * If a group ID is missing in an existing message entity
 * try to retrieve it via the message id. This would happen if the
 * group creation fails or if the creation response was interrupted.
 *
 * @param $api_call
 * @param $message_id
 *
 * @return mixed
 */
function _springboard_target_api_get_group_by_message_id($api_call, $message_id) {
  $response = $api_call->invokeClientMethod('getTargetGroupByMessageID', $message_id);
  return $response;
}

/**
 * Delete a group.
 *
 * @param object $entity
 *   A pseudo entity.
 */
function springboard_target_api_delete_target_group($entity) {
  // Delete the remote API message group.
  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('deleteTargetGroup', $entity->data['group_id']);

  if (isset($response->data)) {
    drupal_set_message(t("The group was deleted."));
  }
}

/**
 * Retrieve a single target group.
 *
 * @param int $group_id
 *   The group id.
 *
 * @return mixed
 *   The API server response.
 */
function _springboard_target_api_get_target_group($group_id) {
  $api_call = springboard_advocacy_api_call();
  $response = $api_call->invokeClientMethod('getTargetGroup', $group_id);

  return $response;
}

/**
 * Prevent infinite recursion of group parentage.
 *
 * @param int $group_id
 *   The group id.
 * @param int $parent_id
 *   The parent group id.
 * @param string $parent_name
 *   The parent group name.
 *
 * @return bool
 *   Whether the group can be nested.
 */
function _springboard_target_api_check_group_recursion($group_id, $parent_id, $parent_name = '') {
  $group = _springboard_target_api_get_target_group($group_id);
  if (isset($group->data->groups)) {
    foreach ($group->data->groups as $nested_group) {
      if ($nested_group->target_group_group_id == $parent_id) {
        drupal_set_message('The custom group "' . ($parent_name != '' ? $parent_name : $group->data->name) . '" cannot be added to this group because it contains this group as a member.', 'error');
        return TRUE;
      }
      else {
        return _springboard_target_api_check_group_recursion($nested_group->target_group_group_id, $parent_id, ($parent_name != '' ? $parent_name : $group->data->name));
      }
    }
  }
}