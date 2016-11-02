<?php
/**
 * @file
 * Provides form alters for Braintree Paypal.
 */

/**
 * Implements hook_module_implements_alter().
 * 
 */
function braintree_payment_module_implements_alter(&$implementations, $hook) {
  // move our hook to the end of the list to ensure fundraiser has had a chance
  // to add its overrides.
  if ($hook == 'form_node_form_alter' || $hook == 'form_alter') {
    $group = $implementations['braintree_payment'];
    unset($implementations['braintree_payment']);
    $implementations['braintree_payment'] = $group;
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 * Add Paypal 'compact' option to donation forms using the Braintree Paypal
 * payment method.
 */
function braintree_payment_form_node_form_alter(&$form, &$form_state, $form_id) {
  if (empty($form['#node']->type) || !fundraiser_is_donation_type($form['#node']->type)) {
    return;
  }
  // If this is an appropriate node edit/add form, add the "paypal compact"
  // checkbox to the "Payment methods" fieldset.
  if (empty($form['payment_methods']['gateways']) || !element_get_visible_children($form['payment_methods']['gateways'])) {
    return;
  }
  // Is Paypal Braintree an available payment method?
  if (isset($form['payment_methods']['gateways']['paypal']) &&
    array_key_exists('braintree_paypal|commerce_payment_braintree_paypal', $form['payment_methods']['gateways']['paypal']['id']['#options'])) {
    $enabled_form_ids = variable_get('braintree_paypal_compact', array());
    $form['payment_methods']['braintree_paypal_compact'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable PayPal compact flow on this donation form?'),
      '#description' => t('If enabled, the default fundraiser donor information and billing fields will be hidden and automatically populated by Paypal.'),
      '#default_value' => in_array($form['#node']->nid, $enabled_form_ids),
      '#states' => array(
        'visible' => array(
          ':input[name="gateways[paypal][status]"]' => array('checked' => TRUE),
          'select[name="gateways[paypal][id]"]' => array('value' => 'braintree_paypal|commerce_payment_braintree_paypal'),
        ),
      ), 
              
    );
    $form['actions']['submit']['#submit'][] = '_braintree_payment_node_form_submit';
  }
}

/**
 * Additional submit handler for donation_form node form.
 */
function _braintree_payment_node_form_submit($form, &$form_state) {
   if (isset($form_state['values']['gateways']['paypal']) &&
      $form_state['values']['gateways']['paypal']['id'] == 'braintree_paypal|commerce_payment_braintree_paypal' &&
      $form_state['values']['gateways']['paypal']['status'] &&
      isset($form_state['values']['braintree_paypal_compact']) &&
      $form_state['values']['braintree_paypal_compact']) {
    $enabled_form_ids = variable_get('braintree_paypal_compact', array());
    if (!in_array($form['#node']->nid, $enabled_form_ids)) {
      $enabled_form_ids[] = $form['#node']->nid;
      variable_set('braintree_paypal_compact', $enabled_form_ids);
    }
    
  }
  else {
    // refactor this
    $enabled_form_ids = variable_get('braintree_paypal_compact', array());
    $index = array_search($form['#node']->nid, $enabled_form_ids);
    if ($index !== FALSE) {
      unset($enabled_form_ids[$index]);
      variable_set('braintree_paypal_compact', $enabled_form_ids);
    }
    // delete the var if in array
  }
}

/**
 * Implements hook_form_alter().
 * @see https://api.drupal.org/comment/53678#comment-53678
 */
function braintree_payment_form_alter(&$form, &$form_state, $form_id) {
}

/**
 * Implements hook_form_FORM_ID_alter().
 * Merge into braintree js settings on Braintree Paypal enabled donation forms.
 */
function braintree_payment_form_webform_client_form_alter(&$form, &$form_state) {
  if (empty($form['#node']->type) || !fundraiser_is_donation_type($form['#node']->type)) {
    return;
  }
  $enabled_form_ids = variable_get('braintree_paypal_compact', array());
  if (in_array($form['#node']->nid, $enabled_form_ids)) {
    $settings = array(
     'paypal' => array(
        'compact' => TRUE,
      ),
    );
    if (isset($form['#node']->form_layouts)) {
      $settings['paypal']['form_layout'] = $form['#node']->form_layouts;
    }
    drupal_add_js(array('braintree' => $settings), 'setting');
  }
}