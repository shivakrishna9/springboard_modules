<?php

/**
 * @file
 * Webform module quicksign component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_quicksign() {
  return array(
    'name' => '',
    'form_key' => 'sba_quicksign',
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'width' => '',
      'unique' => 0,
      'disabled' => 0,
      'title_display' => 0,
      'description' => '',
      'attributes' => array(),
      'private' => FALSE,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_quicksign($component) {

  return array();
}


/**
 * Implements _webform_render_component().
 */
function _webform_render_quicksign($component, $value = NULL, $filter = TRUE) {
  global $user;

  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $form = sba_quicksign_form();

  $settings = isset($node->nid) ? sba_quicksign_settings($node->nid) : array();

  $element = array(
    '#type' => 'fieldset',
    '#title' => isset($settings['quicksign_label']) ? $settings['quicksign_label'] : '',
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : NULL,
    '#weight' => $component['weight'],
    '#description' => $filter ? _webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
    '#collapsible' => FALSE,
    '#attributes' => array('class' => array('webform-component-fieldset')),
    '#pre_render' => array('webform_fieldset_prerender', 'webform_element_title_display'),
    '#translatable' => array('title', 'description'),
  );


  // Add an e-mail class for identifying the difference from normal textfields.
  $element['#attributes']['class'][] = 'quicksign';

  return $element;
}


/**
 * Implements _webform_display_component().
 */
function _webform_display_quicksign($component, $value, $format = 'html') {

  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_quicksign',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
    '#translatable' => array('title'),
  );
}

