<?php

/**
 * @file
 * Webform module event_table component.
 */

// Payment Fields depends on functions provided by fieldset.
webform_component_include('fieldset');

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
      'collapsible' => 0,
      'collapsed' => 0,
      'description' => '',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_payment_fields($component) {
  $form = _webform_edit_fieldset($component);
  $form['edit_notice']['#markup'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_payment_fields($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $element = _webform_render_fieldset($component, $value, $filter);
  $element['#pre_render'][] = '_fundraiser_component_payment_fields_pre_render';

  // Loop through the payment methods and add the fields for each
  if (!empty($node->gateway_manager['payment_methods'])) {
    foreach ($node->gateway_manager['payment_methods'] as $payment_method) {
      // Load up the instance
      $gateway_instance = gateway_manager_method_load($payment_method->method, $payment_method->gateway_instance);

      if (function_exists($gateway_instance['render_callback'])) {
        $element[$payment_method->method] = array(
          '#type' => 'fieldset',
          '#attributes' => array(
          'id' => 'fundraiser-payment-method-' . str_replace('_', '-', $payment_method->method),
          'class' => array('fundraiser-payment-method')
          ),
        );

        // Call the render callback function to get the fields
        $payment_method_fields = $gateway_instance['render_callback']();
        // allow other modules to alter the fields before adding to form
        drupal_alter('payment_method_fields', $payment_method_fields, $payment_method->method);
        $element[$payment_method->method] += $payment_method_fields;
      }
    }
  }
  $element['#validate_callback'] = '_gateway_manager_donation_form_validate_required';
  return $element;
}

/**
 * Pre-render function to add our id to the js settings
 */
function _fundraiser_component_payment_fields_pre_render($element) {
  $node = isset($element['#webform_component']['nid']) ? node_load($element['#webform_component']['nid']) : NULL;

  // Add the id of the outer fieldset to the js settings
  drupal_add_js(array('fundraiserWebform' => array('components' => array('payment_fields' => array('id' => $element['#attributes']['id'])))), 'setting');
  return $element;
}