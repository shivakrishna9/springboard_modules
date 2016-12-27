<?php

/**
 * @file
 * Helper functions for the secure prepopulate component.
 */

/**
 * Insert new components on node/add.
 *
 * Called from secure_prepopulate_node_insert();
 *
 * @param $node
 */
function secure_prepopulate_insert_components($node) {
  module_load_include('inc', 'webform', 'includes/webform.components');

  $fields[] = array(
    'nid' => $node->nid,
    'form_key' => 'secure_prepop_autofill',
    'pid' => 0,
    'name' => t('Autofilled'),
    'type' => 'hidden',
    'value' => 0,
    'weight' => 13,
    'email' => 1,
  );


  // Add the component to the Webform.
  foreach ($fields as $field) {
    // Don't insert fields if cloning.
    if (!isset($node->map) || (isset($node->map) && !in_array($field['form_key'], $node->map))) {
      $cid = webform_component_insert($field);
    }
  }
}



