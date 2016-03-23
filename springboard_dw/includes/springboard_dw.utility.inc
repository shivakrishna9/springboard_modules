<?php

/**
 * @file
 * Utility functions for Springboard data warehouse.
 */

/**
 * Get all entity properties by way of a wrapper.
 * @param $wrapper
 * @return array
 */
function springboard_dw_get_all_entity_properties($wrapper)
{
  $values = array();

  foreach ($wrapper->getPropertyInfo() as $key => $val) {
    $values[$key] = $wrapper->$key->value();
  }

  return $values;
}

/**
 * Get the webform submission dated keyed according to submission.
 * @param $nid
 * @param $sid
 * @return array
 */
function springboard_dw_webform_submission_data_keyed($nid, $sid)
{
  $data = array();
  $node = node_load($nid);

  module_load_include('inc', 'webform', 'includes/webform.submissions');
  $submission = webform_get_submission($nid, $sid);

  foreach ($node->webform['components'] AS $key => $component) {
    if (isset($submission->data[$key])) {
      $data[$component['form_key']] = $submission->data[$key];
    }
  }

  return $data;
}