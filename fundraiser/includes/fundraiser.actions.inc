<?php
/**
 * Action hooks for Fundraiser.
 */
/**
 * Implements hook_action_info().
 */
function fundraiser_action_info() {
  return array(
    '_fundraiser_gateway_switch_action' => array(
      'description' => t('Gateway Bulk Switch'),
      'label' => t('Switch the gateway configurations of multiple forms at once.'),
      'type' => 'node',
      'configurable' => TRUE,
      'triggers' => array('any'),
    ),
  );
}

/**
 * An action to change the gateway of a node.
 */
function _fundraiser_gateway_switch_action($node, $context = array()) {
  $orig_config = $node->gateways;
  // Only update the methods that have been selected.
  foreach ($context['update_methods'] as $method) {
    $node->gateways[$method] = $context['gateways'][$method];

    // If we're updating the default setting.
    if (!empty($context['update_default']) && !empty($context['gateways']['_default'])) {
      $node->gateways[$method]['default'] = ($context['gateways']['_default'] == $method) ? 1 : 0;
    }
    // Else use the original setting, if not in the original setting then set it
    // to 0.
    else {
      $node->gateways[$method]['default'] = !empty($orig_config[$method]['default']) ? $orig_config[$method]['default'] : 0;
    }

    // If we're updating the weights.
    if (!empty($context['update_weights'])) {
      $node->gateways[$method]['weight'] = $context['gateways'][$method]['weight'];
    }
    // Else use the original setting, if not in the original setting then set it
    // to 0.
    else {
      $node->gateways[$method]['weight'] = !empty($orig_config[$method]['weight']) ? $orig_config[$method]['weight'] : 0;
    }
  }

  _fundraiser_update_fundraiser($node);
}

/**
 * Configuration form for the gateway switching action.
 */
function _fundraiser_gateway_switch_action_form($context) {
  $gateways = _fundraiser_gateway_info();

  if (empty($gateways)) {
    return array();
  }

  // Create an array of available payment methods
  foreach ($gateways as $gateway) {
    $types = isset($gateway['payment_method']) ? $gateway['payment_method'] : array();
    foreach ($types as $type) {
      $methods[$type] = $type;
    }
  }

  // Checkboxes to select which payment methods will be updated
  $form['update_methods'] = array(
    '#title' => 'Payment Methods to Update',
    '#description' => t('Select the payment methods to update. Only select a payment method if you want its settings to be changed on all the forms.'),
    '#type' => 'checkboxes',
    '#options' => $methods,
  );

  // Option to update the default method across all the forms
  $form['update_default'] = array(
    '#title' => 'Update the default method',
    '#description' => t('Check this to change the default method across all these forms.'),
    '#type' => 'checkbox',
    '#return_value' => 1,
  );

  // Option to update the ordering accross all the forms
  $form['update_weights'] = array(
    '#title' => 'Update the ordering',
    '#description' => t('Check this to change the ordering of methods across all these forms.'),
    '#type' => 'checkbox',
    '#return_value' => 1,
  );

  $fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Payment methods'),
    '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#theme' => 'fundraiser_form_payment_methods',
  );

  $configuration = variable_get('fundraiser_payment_method_config_defaults', array());
  $form['gateways'] = _fundraiser_form_payment_method_config_fieldset($gateways, $configuration, $fieldset, 'gateways');

  foreach ($methods as $method) {
    foreach (element_children($form['gateways'][$method]) as $field_name) {
      // Add states to each config field to disable it if that payment method hasn't been selected
      $form['gateways'][$method][$field_name]['#states'] = array(
        'disabled' => array(
          ':input[name="update_methods[' . $method . ']"]' => array('checked' => FALSE),
        ),
      );
    }

    // Add states to hide the default and weight fields if that option isn't checked
    $form['gateways'][$method]['_default']['#states']['visible'][] = array(':input[name="update_default"]' => array('checked' => TRUE));
    $form['gateways'][$method]['weight']['#states']['visible'][] = array(':input[name="update_weights"]' => array('checked' => TRUE));
  }

  return $form;
}

/**
 * Validate handler for the gateway switch action form.
 */
function _fundraiser_gateway_switch_action_validate($form, $form_state) {
  $update_methods = array_filter($form_state['values']['update_methods']);
  // Ensure at least one method has been selected
  if (empty($update_methods)) {
    form_set_error('update_methods', 'Please select at least one payment method.');
    return;
  }

  // If updating the default method ensure an enabled payment method is selected as default
  $default_enabled = FALSE;
  if (!empty($form_state['values']['update_default'])) {
    foreach ($update_methods as $method) {
      if ($form_state['values']['gateways'][$method]['status'] == 1 && $form_state['values']['gateways']['_default'] == $method) {
        $default_enabled = TRUE;
      }
    }
    if (!$default_enabled) {
      form_set_error('update_default', "You've selected to update the default method but haven't chosen an enabled method.");
    }
  }
}

/**
 * Submit handler for the gateway switch action form.
 */
function _fundraiser_gateway_switch_action_submit($form, $form_state) {
  return array(
    'update_methods' => array_filter($form_state['values']['update_methods']),
    'update_default' => $form_state['values']['update_default'],
    'update_weights' => $form_state['values']['update_weights'],
    'gateways' => $form_state['values']['gateways'],
  );
}
