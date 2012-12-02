<?php

/**
 * @file
 * Webform module payment_select component.
 */
 
// event_tickets element depends on functions provided by select.
webform_component_include('select');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_payment_select() {
  return array(
    'name' => '',
    'form_key' => 'payment_type',
    'mandatory' => 1,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'items' => '',
      'aslist' => 0,
      'multiple' => 0,
      'optrand' => 0,
      'other_option' => NULL,
      'other_text' => NULL,
      'title_display' => 0,
      'description' => '',
      'custom_keys' => FALSE,
      'options_source' => '',
      'private' => FALSE,
      'ticket_value' => NULL,
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_payment_select() {
  return array(
    'fundraiser_component_payment_select' => array(
      'arguments' => array('element' => NULL),
      'file' => 'components/payment_select.inc',
    ),
  );
}

/**
 * Theme function for the display of this field
 */
function theme_fundraiser_component_payment_select($element) {
  return theme('webform_display_select' , $element);
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_payment_select($component) {
  // Use the standard fields for the select
  $form = _webform_edit_select($component);
  $form['edit_notice']['#value'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  // Remove some items we don't need
  $form['value']['#access'] = FALSE;
  $form['display']['#access'] = FALSE;
  $form['extra']['description']['#access'] = FALSE;
  $form['extra']['multiple']['#access'] = FALSE;
  $form['extra']['items']['#access'] = FALSE;
  $form['extra']['options_source']['#access'] = FALSE;
  // Set the fields as values, this will override the default in array_merge webform_component_edit_form
  $form['form_key']['#type'] = 'value';
  $form['form_key']['#value'] = $component['form_key'];
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_payment_select($component, $value = NULL, $filter = TRUE) {
  // Use the default select list function to build the element
  $element = _webform_render_select($component, $value, $filter);

  // Populate our options, moved to fundraiser_form_alter
  //$element['#options'] = fundraiser_component_payment_select($component);
  // Add custom post render function before the final form render
  $element['#post_render'] = array_merge(array('fundraiser_component_payment_select_post_render'), $element['#post_render']);

  return $element;
}

/**
 * Return the options for this component
 */
function fundraiser_component_payment_select($component) {
  //$types = gateway_manager_gateway_types();
  //drupal_set_message(print_r($types, TRUE));
  $types = array('credit' => 'Credit', 'paypal' => 'Paypal');
  foreach ($types as $type => $label) {
    $options[$type] = t($label);
  }
  return $options;
}

/**
 * Pre-render function to add our id to the js settings
 */
function fundraiser_component_payment_select_post_render($content, $element) {
  drupal_add_js(array('fundraiser' => array('webformComponents' => array('paymentSelect' => array('id' => $elements['#id'])))), 'setting');
  return $content;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_payment_select($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_payment_select',
    '#theme_wrappers' => $format == 'html' ? array('webform_element', 'webform_element_wrapper') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#format' => $format,
    '#options' => _webform_select_options($component, !$component['extra']['aslist']),
    '#value' => (array) $value,
    '#translatable' => array('title', 'options'),
    '#webform_component' => $component,
  );
}

/**
 * Implements _webform_submit_component().
 *
 * Convert FAPI 0/1 values into something saveable.
 */
function _webform_submit_payment_select($component, $value) {
  return _webform_submit_select($component, $value);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_payment_select($component, $sids = array(), $single = FALSE) {
  return webform_analysis_select($component, $sids, $single);
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_payment_select($component, $value) {
  return _webform_table_select($component, $value);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_payment_select($component, $export_options) {
  return _webform_csv_headers_select($component, $export_options);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_payment_select($component, $export_options, $value) {
  return _webform_csv_data_select($component, $export_options, $value);
}