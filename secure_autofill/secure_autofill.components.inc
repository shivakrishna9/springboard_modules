<?php

/**
 * @file
 * Helper functions for the secure autofill component.
 */

/**
 * Insert new components on node/add.
 *
 * Called from secure_autofill_node_insert();
 *
 * @param $node
 */
function secure_autofill_insert_components($node) {
  module_load_include('inc', 'webform', 'includes/webform.components');

  $field = array(
    'nid' => $node->nid,
    'form_key' => 'secure_prepop_autofilled',
    'pid' => 0,
    'name' => t('Autofilled'),
    'type' => 'hidden',
    'value' => 0,
    'weight' => 13,
    'email' => 1,
    'extra' => array(
      'description' => '',
      'hidden_type' => 'hidden',
    ),
  );

  // Add the component to the Webform.
  // Don't insert field if cloning.
  if (!isset($node->map) || (isset($node->map) && !in_array($field['form_key'], $node->map))) {
    webform_component_insert($field);
  }
}