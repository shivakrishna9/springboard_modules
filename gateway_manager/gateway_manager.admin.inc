<?php

/**
 * @file
 * Admin functionality for gateway_manager module.
 */


/**
 * Alter the gateway form elements on the donation node form.
 */
function gateway_manager_donation_node_form(&$form, &$form_state) {
  $node = $form['#node'];
  $form['#validate'][] = 'gateway_manager_donation_node_form_validate';

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment methods'),
    '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#theme' => 'gateway_manager_node_form_payment_methods',
  );

  // Get all available payment methods.
  $all_payment_methods = gateway_manager_all_payment_methods();
  $node_payment_methods = isset($node->gateway_manager['payment_methods']) ?
    $node->gateway_manager['payment_methods'] : array();
  foreach (array_keys($all_payment_methods) as $method_key) {
    $all_payment_methods[$method_key]['#weight'] = isset($node_payment_methods[$method_key]->weight) ?
      $node_payment_methods[$method_key]->weight : 0;
  }

  // Sort all payment methods by weight.
  uasort($all_payment_methods, '_gateway_manager_payment_method_weight_compare');

  foreach ($all_payment_methods as $method_key => $method) {
    $node_method_settings = isset($node_payment_methods[$method_key]) ?
      $node_payment_methods[$method_key] : new stdClass();

    // Build the #options array for gateways supporting this payment method.
    $gateway_options = array(NULL => '- ' . t('select') . ' -');
    foreach (element_children($method) as $gateway_key) {
      $gateway_options[$gateway_key] = $method[$gateway_key]['admin_title'];
    }

    $fieldset[$method_key] = array('#tree' => TRUE);
    $fieldset[$method_key]['status'] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($node_method_settings->status) ? $node_method_settings->status : 0,
    );
    $fieldset[$method_key]['_status'] = array(
      '#type' => 'item',
      '#title' => t('Method'),
      '#value' => $method['#label'],
    );
    $fieldset[$method_key]['method'] = array(
      '#type' => 'value',
      '#value' => $method_key,
    );
    $fieldset[$method_key]['gateway_instance'] = array(
      '#type' => 'select',
      '#title' => t('Gateway'),
      '#options' => $gateway_options,
      '#default_value' => isset($node_method_settings->gateway_instance) ? $node_method_settings->gateway_instance : NULL,
    );
    $fieldset[$method_key]['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#size' => 20,
      '#default_value' => isset($node_method_settings->label) ? $node_method_settings->label : NULL,

    );
    $fieldset[$method_key]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 25,
      '#default_value' => isset($node_method_settings->weight) ? $node_method_settings->weight : 0,
      '#attributes' => array('class' => 'gateway-manager-payment-methods-weight'),
    );
    if (!isset($fieldset['_default_method'])) {
      $fieldset['_default_method'] = array(
        '#type' => 'radios',
        '#title' => t('Default'),
        '#options' => array(),
      );
    }
    $fieldset['_default_method']['#options'][$method_key] = '';
    if ($node_method_settings->is_default) {
      $fieldset['_default_method']['#default_value'] = $method_key;
    }
  }

  $form['fundraiser_settings']['gateway_manager'] = $fieldset;

  // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
  $form['fundraiser_settings']['#collapsed'] = FALSE;
  // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
}

/**
 * Sort payment methods by weight on the node form.
 */
function _gateway_manager_payment_method_weight_compare($a, $b) {
  $weight = $a['#weight'] - $b['#weight'];
  return $weight;
}

/**
 * Validate callback for the donation node edit/add form.
 */
function _gateway_manager_donation_node_form_validate($form, &$form_state) {
  $values = $form_state['values']['gateway_manager'];

  // Capture the default method and remove it from the array.
  $default_method = $values['_default_method'];
  unset($values['_default_method']);

  // Filter the values array to just enabled methods.
  $enabled_methods = array_filter($values, 'gateway_manager_donation_node_form_validate_filter_enabled');

  // Check that there is at least one method/gateway enabled.
  if (empty($enabled_methods)) {
    form_set_error('gateway_manager', t('At least one payment method and gateway must be enabled.'));
  }
  else {
    // Check whether the default method is one of the enabled methods.
    if (!isset($enabled_methods[$default_method])) {
      if (count($enabled_methods) == 1) {
        // If there's only one enabled method, skip validating _default_method
        // and just change the value. Stop wasting people's time.
        $form_state['values']['gateway_manager']['_default_method'] = key($enabled_methods);
      }
      else {
        form_set_error('gateway_manager][_default_method', t('Default method must be one of the enabled methods.'));
      }
    }

    // Validate that each enabled method has a corresponding gateway.
    $all_methods = gateway_manager_all_payment_methods();
    foreach ($enabled_methods as $method => $method_settings) {
      if (empty($method_settings['gateway_instance'])) {
        // Get the method label for the error message.
        $method_label = isset($all_methods[$method]['#label']) ? $all_methods[$method]['#label'] : $method;
        // Set the error on this method.
        form_set_error('gateway_manager][' . $method . '][gateway_instance',
          t('Gateway is missing or invalid for %method_label.', array('%method_label' => $method_label)));
      }
    }
  }
}

/**
 * Array_filter callback for filtering by method status.
 */
function gateway_manager_donation_node_form_validate_filter_enabled($method_settings) {
  return !empty($method_settings['status']);
}

/**
 * Save changes to the gateway_manager_node schema on a node save.
 */
function gateway_manager_donation_node_save(&$node, $op) {
  if (!isset($node->gateway_manager)) {
    return;
  }

  // For simplicity's sake, delete everything, and re-add.
  db_query("DELETE FROM {gateway_manager_node} WHERE nid = %d", $node->nid);

  if ($op == 'delete') {
    // Exit now, since we just deleted everything.
    return;
  }
  else {
    // Capture the default method.
    if (isset($node->gateway_manager['_default_method'])) {
      $default_method = $node->gateway_manager['_default_method'];
      unset($node->gateway_manager['_default_method']);
    }
    // Write a record for each defined payment method.
    foreach ($node->gateway_manager as $payment_method => $record) {
      $record['nid'] = $node->nid;
      $record['is_default'] = ($default_method == $payment_method);
      drupal_write_record('gateway_manager_node', $record);
    }
  }
}

/**
 * Pulls the settings function out of an array of payment gateways.
 */
function _gateway_manager_get_settings_function($gateways, $type) {
  foreach($gateways as $gateway) {
    if ($gateway['id'] == $type) {
      return $gateway['settings'];
    }
  }
}

/**
 * Theme function for the payment methods table on the node form.
 */
function theme_gateway_manager_node_form_payment_methods($fieldset) {
  $header = array();
  $rows = array();

  $default_method = $fieldset['_default_method'];
  unset($fieldset['_default_method']);

  foreach (element_children($fieldset) as $i) {
    $row = array();

    foreach (element_children($fieldset[$i]) as $field) {
      $this_element = $fieldset[$i][$field];
      if (!isset($header[$field])) {
        $header[$field] = isset($this_element['#title']) ? $this_element['#title'] : '';
      }
      unset($this_element['#title']);
      $row[$field] = drupal_render($this_element);
    }

    if (!isset($header['_default_method'])) {
      $header['_default_method'] = isset($default_method['#title']) ? $default_method['#title'] : '';
    }
    $row['_default_method'] = drupal_render($default_method[$i]);

    $rows[] = array(
      'data' => $row,
      'class' => 'draggable',
    );
  }

  // Adding tabledrag support to this table.
  drupal_add_tabledrag('gateway-manager-payment-methods', 'order', 'sibling', 'gateway-manager-payment-methods-weight');

  return theme('table', $header, $rows, array('id' => 'gateway-manager-payment-methods'));
}
