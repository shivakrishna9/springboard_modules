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
      'title_display' => 0,
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

  // Find the payment methods component
  $payment_method_component = _fundraiser_webform_get_component_by_nid_key($node->nid, array('payment_method'));
  $payment_method_field = $node->webform['components'][$payment_method_component->cid];
  // Use the enabled payment methods to create the payment fields
  if (!empty($payment_method_field['extra']['payment_methods'])) {
    $payment_methods = array_filter($payment_method_field['extra']['payment_methods'], '_fundraiser_webform_payment_method_array_filter_enabled');
  }
  if (!empty($payment_methods)) {
    // Loop through the payment methods and add fields for each type.
    foreach ($payment_methods as $payment_method => $config) {
      // Load up the instance
      $element[$payment_method] = array(
        '#type' => 'fieldset',
        '#attributes' => array(
          'id' => 'fundraiser-payment-method-' . str_replace('_', '-', $payment_method),
          'class' => array('fundraiser-payment-method'),
          ),
        '#states' => array(
          'visible' => array(
            ':input[name="submitted[payment_information][payment_method]"]' => array('value' => $payment_method),
          ),
        ),
      );
      // Find the given gateway idea for this, pull from _fundraiser_gateway_info($gateway_id) for fields.
      $gateway_info = _fundraiser_gateway_info($config['id']);
      // Check against the gateway config for a specific form to go with this payment gateway.
      if (isset($gateway_info['form callback'])) {
        $form_func = $gateway_info['form callback'];
        if (function_exists($form_func)) {
          $element[$payment_method] += $form_func($payment_method);
        }
      }
      // Allow other modules to alter the fields before adding to form.
      drupal_alter('payment_method_fields', $element[$payment_method], $payment_method, $config, $node);
    }
  }
  else {
    drupal_set_message(t('Payment method gateways have not been configured on this form. Please configure them.'), 'error');
  }
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