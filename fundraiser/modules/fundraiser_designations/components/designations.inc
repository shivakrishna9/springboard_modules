<?php

/**
 * @file
 */

/**
 * Default values for designation form.
 *
 * @return array
 *   Component defaults.
 */
function _webform_defaults_designations() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'field_prefix' => '',
      'field_suffix' => '',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_designations($component) {
  $form = array();
  $form['edit_notice']['#markup'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  return $form;
}

/**
 * Implements _webform_submit_component().
 *
 * Convert submitted keys and values into serialized, saveable data.
 */
function _webform_submit_designations($component, $values) {
  $return = array();
  foreach ($values as $key => $submitted_value) {
    $return[] = serialize(array($key => $submitted_value));
  }
  return $return;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_designations($component, $value = NULL, $filter = TRUE) {

  $element = array(
    '#type' => 'item',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#weight' => $component['weight'],
    '#attributes' => array('class' => array('webform-component-designations')),
    '#title_display' => 'none',
    '#webform_component' => $component,
    '#theme' => 'fundraiser_designations_webform_table',
    '#attached' => array('js' => array(drupal_get_path('module', 'fundraiser_designations') . '/js/fundraiser_designations.js')),
  );

  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $node_wrapper = entity_metadata_wrapper('node', $node);
  if (fundraiser_designations_is_designation_type($node->type)) {


  }
  return $element;
}

/**
 * Pre-render function to add our id to the js settings.
 */
function webform_designations_prerender($element) {
  $id = 'webform-component-' . str_replace('_', '-', implode('--', array_slice($element['#parents'], 1)));
  $element['#attributes']['id'] = $id;
  drupal_add_js(array('fundraiserDesignations' => array('table' => array('id' => $id))), 'setting');
  return $element;
}

/**
 * Theme the form elements into a table.
 */
function theme_fundraiser_designations_webform_table($variables) {

  $output = 'test';
  $element = $variables['element'];

  $node = node_load($element['#webform_component']['nid']);

  return $output;
}
