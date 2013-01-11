<?php

/**
 * @file
 * Webform module payment_method component.
 */
 
// Payment methods element depends on functions provided by select.
webform_component_include('select');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_payment_method() {
  return array(
    'name' => '',
    'form_key' => 'payment_method',
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
      'payment_methods' => array()
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_payment_method($component) {
  // Use the standard fields for the select
  $form = _webform_edit_select($component);
  // Remove some items we don't need
  $form['value']['#access'] = FALSE;
  $form['display']['#access'] = FALSE;
  $form['extra']['description']['#access'] = FALSE;
  $form['extra']['multiple']['#access'] = FALSE;
  $form['extra']['items']['#access'] = FALSE;
  $form['extra']['options_source']['#access'] = FALSE;

  // Initialize the fieldset
  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment methods'),
    '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#theme' => 'fundraiser_webform_component_payment_methods',
  );

  // Get all available payment methods.
  $temp_gateways = _fundraiser_gateway_info();
  if (empty($temp_gateways)) {
    $form['no_gateways_notice']['#markup'] = '<div>' . t('There are no gateways configured. Please create some gateways in the store configuration.') . '</div>'; 
    return $form; 
  }
  foreach($temp_gateways as $temp_gateway) {
    $types = isset($temp_gateway['payment_method']) ? $temp_gateway['payment_method'] : array('credit');
    foreach ($types as $type) {
      $all_payment_methods[$type][] = $temp_gateway;
    }
  }
  $field_payment_methods = !empty($component['extra']['payment_methods']) ? $component['extra']['payment_methods'] : array();
  foreach (array_keys($all_payment_methods) as $method_key) {
    $all_payment_methods[$method_key]['#weight'] = isset($field_payment_methods[$method_key]['weight']) ? $field_payment_methods[$method_key]['weight'] : 0;
  }
  // Sort all payment methods by weight.
  uasort($all_payment_methods, '_fundraiser_webform_payment_method_weight_compare');

  // Loop through each method
  foreach ($all_payment_methods as $method_key => $method) {
    $field_method_settings = isset($field_payment_methods[$method_key]) ? $field_payment_methods[$method_key] : array();
    // Build the #options array for gateways supporting this payment method.
    $gateway_options = array(NULL => '- ' . t('select') . ' -');
    foreach (element_children($method) as $key) {
      $gateway_options[$method[$key]['id']] = $method[$key]['title'];
    }
    $fieldset[$method_key] = array('#tree' => TRUE);
    $fieldset[$method_key]['status'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($field_method_settings['status']) ? $field_method_settings['status'] : 0,
    );
    $fieldset[$method_key]['_status'] = array(
      '#type' => 'markup',
      '#title' => t('Method'),
      '#markup' => $method_key, // Could use a pretty name here
    );
    $fieldset[$method_key]['method'] = array(
      '#type' => 'value',
      '#value' => $method_key,
    );
    $fieldset[$method_key]['gateway_instance'] = array(
      '#type' => 'select',
      '#title' => t('Gateway'),
      '#options' => $gateway_options,
      '#default_value' => isset($field_method_settings['gateway_instance']) ? $field_method_settings['gateway_instance'] : NULL,
    );
    $fieldset[$method_key]['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#size' => 20,
      '#default_value' => isset($field_method_settings['label']) ? $field_method_settings['label'] : NULL,
    );
    $fieldset[$method_key]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 25,
      '#default_value' => isset($field_method_settings['weight']) ? $field_method_settings['weight'] : 0,
      '#attributes' => array('class' => array('fundraiser-webform-payment-methods-weight')),
    );
    // Create a radio group for selecting the default
    if (!isset($fieldset['_default_method'])) {
      $fieldset['_default_method'] = array(
        '#type' => 'radios',
        '#title' => t('Default'),
        '#options' => array(),
      );
    }
    $fieldset['_default_method']['#options'][$method_key] = '';
  }
  // Set the default value on the default select
  if (!empty($field_payment_methods['_default_method'])) {
    $fieldset['_default_method']['#default_value'] = $field_payment_methods['_default_method'];
  }
  $form['extra']['payment_methods'] = $fieldset;
  return $form;
}

/**
 * Helper function, sort payment methods by weight on the node form.
 */
function _fundraiser_webform_payment_method_weight_compare($a, $b) {
  $weight = $a['#weight'] - $b['#weight'];
  return $weight;
}

/**
 * Theme function for the payment methods table on the componenet edit form.
 */
function theme_fundraiser_webform_component_payment_methods($variables) {
  $fieldset = $variables['form'];
  $header = array();
  $rows = array();
  $default_method = $fieldset['_default_method'];
  unset($fieldset['_default_method']);

  foreach (element_children($fieldset) as $i) {
    $row = array();
    foreach (element_children($fieldset[$i]) as $field) {
      $this_element = $fieldset[$i][$field];

      if ($this_element['#type'] != 'value') {
        if (!isset($header[$field])) {
          $header[$field] = isset($this_element['#title']) ? $this_element['#title'] : '';
        }
        unset($this_element['#title']);
        $row[$field] = drupal_render($this_element);
      }
    }
    if (!isset($header['_default_method'])) {
      $header['_default_method'] = isset($default_method['#title']) ? $default_method['#title'] : '';
    }
    $row['_default_method'] = drupal_render($default_method[$i]);

    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  // Adding tabledrag support to this table.
  drupal_add_tabledrag('fundraiser-webform-payment-methods', 'order', 'sibling', 'fundraiser-webform-payment-methods-weight');

  return theme('table', array('rows' => $rows, 'header' => $header, 'attributes' => array('id' => 'fundraiser-webform-payment-methods')));
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_payment_method($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  // Use the default select list function to build the element
  $element = _webform_render_select($component, $value, $filter);
  // Add custom post render function before the final form render
  $element['#post_render'] = array('_fundraiser_component_payment_method_post_render');
  $options = array();
  $default = 'credit';
  // Grab the available gateways per type as configured on the payment_method settings.
  foreach ($component['extra']['payment_methods'] as $key => $payment_method) {
    if ($key == '_default_method' && !empty($payment_method)) {
      $default = $payment_method;
    }
    else {
      // Only include if a gateway_instance is mapped.
      if (!empty($payment_method['gateway_instance'])) {
        $options[ $payment_method['method'] ] = !empty($payment_method['label']) ? $payment_method['label'] :
          (!empty($payment_method['gateway_instance']) ? $payment_method['gateway_instance'] : $payment_method['method']);
      }
    }
  }
  $element['#options'] = $options; 
  return $element;
}

/**
 * Post-render function to add the id and active payment methods to the js settings
 */
function _fundraiser_component_payment_method_post_render($content, $element) {
  // Add the wrapper id to the js settings
  $nested_level = $element['#parents'][0] == 'submitted' ? 1 : 0;
  $parents = str_replace('_', '-', implode('--', array_slice($element['#parents'], $nested_level)));
  drupal_add_js(array('fundraiserWebform' => array('components' => array('payment_method' => array('id' => 'webform-component-' . $parents)))), 'setting');
  // Add the active methods to js settings
  $delta = 1;
  $js_settings = array();
  foreach ($element['#options'] as $key => $value) {
    $js_settings[$delta++] = $key;
  }
  drupal_add_js(array('fundraiserWebform' => array('activeMethods' => $js_settings)), 'setting');
  return $content;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_payment_method($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_payment_method',
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
function _webform_submit_payment_method($component, $value) {
  return _webform_submit_select($component, $value);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_payment_method($component, $sids = array(), $single = FALSE) {
  return webform_analysis_select($component, $sids, $single);
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_payment_method($component, $value) {
  return _webform_table_select($component, $value);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_payment_method($component, $export_options) {
  return _webform_csv_headers_select($component, $export_options);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_payment_method($component, $export_options, $value) {
  return _webform_csv_data_select($component, $export_options, $value);
}