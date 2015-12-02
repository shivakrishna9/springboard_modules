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
//function _webform_render_quicksign($component, $value = NULL, $filter = TRUE) {
//  global $user;
//
//  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
//  $settings = isset($node->nid) ? sba_quicksign_settings($node->nid) : array();
//
//  $element = array(
//    '#type' => 'markup',
//    '#title' => isset($settings['quicksign_label']) ? $settings['quicksign_label'] : '',
//    '#title_display' => FALSE,
//    '#weight' => $component['weight'],
//    '#description' => '',
//    '#collapsible' => FALSE,
//    '#attributes' => array('class' => array('webform-component-quicksign')),
//    '#pre_render' => array(),
//    '#translatable' => array('title', 'description'),
//  );
//
//
//  // Add an e-mail class for identifying the difference from normal textfields.
//  $element['#attributes']['class'][] = 'quicksign';
//
//  return $element;
//}


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

