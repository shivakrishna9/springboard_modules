<?php

/**
 * @file
 * Webform module event_table component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_payment_fields() {
  return array(
    'name' => 'Payment Fields',
    'form_key' => 'payment_fields',
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_payment_fields($component) {
  $form = array();
  $form['edit_notice']['#value'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  // Set the form_key field as a value, this will override the default in array_merge webform_component_edit_form
  $form['form_key']['#type'] = 'value';
  $form['form_key']['#value'] = $component['form_key'];
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_payment_fields($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'fieldset',
    '#title' => t('Payment'),
    '#weight' => $component['weight'],
    '#description' => $filter ? _webform_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#attributes' => array('class' => 'fundraiser-payment-fields'),
    '#pre_render' => array('_fundraiser_component_payment_fields_pre_render', 'webform_element_title_display'),
    '#webform_component' => $component,
    //'#theme' => 'fundraiser_component_payment_fields'
  );

  // Hide the event_table title if #title_display is 'none'.
  if (!empty($component['extra']['title_display']) && $component['extra']['title_display'] == 'none') {
    $element['#title'] = NULL;
  }

  return $element;
}

/**
 * Pre-render function to add our id to the js settings
 */
function _fundraiser_component_payment_fields_pre_render($element) {
  $id = 'fundraiser-component-payment-fields';
  $element['#attributes']['id'] = $id;
  drupal_add_js(array('fundraiser' => array('webformComponents' => array('paymentFields' => array('id' => $id)))), 'setting');
  return $element;
}

/**
 * Theme the form elements into a table
 */
function theme_fundraiser_component_payment_fields($element) {
  $output .= '<div' . drupal_attributes($element['#attributes']) . '>' . rand(10, 100) . '</div>';
  return $output;
}