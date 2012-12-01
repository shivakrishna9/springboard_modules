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

  gateway_manager_gateway_load_fields('credit', 'test_gateway');

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment methods'),
    '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#theme' => 'gateway_manager_node_form_payment_methods',
  );

  // Get all available payment methods.
  foreach (gateway_manager_all_payment_methods() as $method_key => $method) {
    $node_method_settings = isset($node->gateway_manager['payment_methods'][$method_key]) ?
      $node->gateway_manager['payment_methods'][$method_key] : new stdClass();

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
    $fieldset[$method_key]['is_default'] = array(
      '#type' => 'radio',
      '#title' => t('Default'),
      '#name' => 'gateway_manager[_default]',
      '#return_value' => $method_key,
      '#default_value' => isset($node_method_settings->is_default) ? $node_method_settings->is_default : NULL,
    );
    $fieldset[$method_key]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 25,
      '#default_value' => isset($node_method_settings->weight) ? $node_method_settings->weight : 0,
    );
  }

  $form['fundraiser_settings']['gateway_manager'] = $fieldset;
  unset($form['fundraiser_settings']['gateway']);

  // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
  $form['fundraiser_settings']['#collapsed'] = FALSE;
  $form['fundraiser_settings']['gateway_manager']['#collapsed'] = FALSE;
  // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG
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
    // Write a record for each defined payment method.
    foreach ($node->gateway_manager as $payment_method => $record) {
      $record['nid'] = $node->nid;
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

    $rows[] = array(
      'data' => $row,
    );
  }

  return theme('table', $header, $rows);
}
