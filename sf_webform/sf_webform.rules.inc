<?php

/**
 * @file
 * Functions for rules integration.
 */

/**
 * Implementation of hook_rules_action_info().
 */
function sf_webform_rules_action_info() {
  return array(
    'sf_webform_update_component_value_action' => array(
      'label' => t('Update a webform component\'s submitted value'),
      'module' => 'Salesforce webform',
      'arguments' => array(
        'oid' => array(
          'type' => 'value',
          'label' => t('The id of the webform submission to update'),
        ),
      ),
      'eval input' => array('component', 'value'),
    ),
  );
}

/**
 * Settings form for choosing which webform component to update.
 */
function sf_webform_update_component_value_action_form($settings, &$form) {
  $settings += array(
    'component' => '',
    'value' => '',
  );

  // get a list of all webform components
  $components = _sf_webform_get_components();

  $form['settings']['component'] = array(
    '#type' => 'select',
    '#title' => t('Webform component'),
    '#options' => $components,
    '#default_value' => $settings['component'],
    '#description' => t('The component that will be updated.'),
  );

  $form['settings']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#default_value' => $settings['value'],
    '#description' => t('The value to update the component with.'),
  );
}

/**
 * Action to update a webform component field.
 *
 * @param $oid
 *   The id of the webform submission to update.
 * @param $settings
 *   The configured rule settings.
 */
function sf_webform_update_component_value_action($oid, $settings) {
  $component = $settings['component'];
  $value = $settings['value'];

  $nid = db_result(db_query("SELECT nid FROM {webform_submissions} WHERE sid = %d", (int) $oid));
  $cid = db_result(db_query("SELECT cid FROM {webform_component} WHERE nid = %d AND form_key = '%s'", $nid, $component));

  // update the submitted value
  db_query("UPDATE {webform_submitted_data} SET data = '%s' WHERE nid = %d AND sid = %d and cid = %d", $value, $nid, (int) $oid, $cid);
}

/**
 * Gets all defined webform components.
 */
function _sf_webform_get_components() {
  $result = db_query("SELECT distinct form_key, name FROM {webform_component} WHERE type != 'fieldset' ORDER BY name");
  $components = array();

  while ($data = db_fetch_object($result)) {
    $components[$data->form_key] = $data->name;
  }

  return $components;
}
