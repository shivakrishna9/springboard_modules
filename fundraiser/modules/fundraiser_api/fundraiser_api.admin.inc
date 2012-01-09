<?php

/**
 * Admin settings form.
 */
function fundraiser_api_admin() {
  $form = array();
  $form['exposed_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Expose the following node types via the API'),
    "#collapsible" => TRUE,
  );

  $form['exposed_types']['fundraiser_api_node_types'] = array(
    '#type' => 'checkboxes',
    '#options' => variable_get('webform_node_types', ''),
    '#default_value' => variable_get('fundraiser_api_node_types', ''),
  );

  return system_settings_form($form);
}
