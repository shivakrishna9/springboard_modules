<?php

/**
 * @file
 * Service endpoint callbacks & access functions.
 */


/**
 *
 *
 */
function springboard_api_form_resource_retrieve($nid) {
  $form_details = springboard_api_get_form_details($nid);
  if ($form_details) {
    return $form_details;
  }
  else {
    // throw error.
  }
}

/**
 *
 *
 */
function springboard_api_form_resource_index() {

  $form_types = variable_get('webform_node_types', array());
  $form_select = db_select('node', 'n');
  $form_select->fields('n', array('nid', 'type', 'title'));
  $form_select->condition('type', $form_types, 'in');
  return $form_select->execute()->fetchAll();
  services_resource_build_index_query($form_select, 0, 0, 0, 1, 'form');
  $results = services_resource_execute_index_query($user_select);
  return services_resource_build_index_list($results, 'form', 'nid');

}

/**
 *
 *
 */
function springoard_api_form_action_submit() {
  $submission = _services_arg_value($submission, 'submission');
  return $submission;
  $form_id = 'webform_client_form_' . $nid;
  $node = node_load($nid);
  $form_state['webform_completed'] = 1;
  $form_state['values'] = array(
    'submit' => t('Donate'),
    'op' => t('Donate'),
    'details' => array(
      'nid' => $nid,
      'sid' =>  '',
      'uid' => 0, // fundraiser should figure this out by email address.
      'page_num' => 1,
      'page_count' => 1,
      'finished' => 0,
    ),
  );
  foreach ($node->webform['component'] as $cid => $component) {
    if ($component['type'] != 'fieldset') {
      $form_state['values']['submitted'][$cid] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : '';
    }
  }
}

/**
 * Get form node & field information for a requested form.
 */
function springboard_api_get_form_details($nid) {
  $node = node_load($nid);
  if (is_object($node)) {
    // fish for internal name.
    $lang = $node->language;
    $internal_name = isset($node->field_fundraiser_internal_name) ? $node->field_fundraiser_internal_name[$lang][0]['safe_value'] : '';

    $form_details = array(
      'nid' => $node->nid,
      'title' => $node->title,
      'internal_name' => $internal_name,
      'type' => $node->type,
      'body' => $node->body,
      'fields' => array(),
    );

    // build form field details list.
    foreach ($node->webform['components'] as $cid => $component) {
      $form_details['fields'][] = array(
        'cid' => $cid,
        'pid' => $component['pid'],
        'form_key' => $component['form_key'],
        'type' => $component['type'],
        'name' => $component['name'],
        'required' => $component['mandatory'],
        'default value' => $component['value'],
        'accepted values' => !empty($component['extra']['items']) ? _springboard_api_parse_extra_items($component['extra']['items']) : '',
      );
    }
    return $form_details;
  }
  else {
    return FALSE;
  }
}

function _springboard_api_parse_extra_items($items) {
  $parts = explode("\n", $items);
  foreach ($parts as $part) {
    $sub = explode('|', $part);

    $values[] = $sub[0];
  }
  return $values;
}