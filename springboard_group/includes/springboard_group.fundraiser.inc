<?php
/**
 * @file
 * Fundraiser functions for springboard groups.
 */

/**
 * Implements hook_fundraiser_gateway_info_alter().
 *
 * If we are on a node add or node edit form, limit the gateways
 * to those allowed by group membership.
 */
function springboard_group_fundraiser_gateway_info_alter(&$gateways) {
//  if (arg(0) == 'node' && (arg(2) == 'edit' || arg(1) == 'add')) {
//    $gateways = springboard_group_fundraiser_filter_gateways($gateways);
//  }
}

/**
 * Update the gateway form after AJAX reload triggered by group selection.
 *
 * Called from springboard_group_node_form_alter().
 */
function springboard_group_fundraiser_node_form__alter(&$form, &$form_state, $callback) {

  // Add the ajax wrapper to the form prior to Ajax reload.
  $form['payment_methods']['gateways']['#prefix'] = '<div id = "springboard-groups-payment-ajax-wrapper">';
  $form['payment_methods']['gateways']['#suffix'] = '</div>';
  $form['#validate'][] = 'springboard_group_fundraiser_node_form_gateway_validate_gateway';
  springboard_group_fundraiser_rebuild_form($form, $form_state);

  // An ajax reload has occurred.
  if (!empty($form_state['values'])) {
    if ($callback != 'springboard_group_og_group_field_ajax_callback') {
      return;
    }
  }
}

/**
 * Validation hook checks for gateway/group mismatch.
 */
function springboard_group_fundraiser_node_form_gateway_validate_gateway($form, $form_state) {
  $gateways = springboard_group_fundraiser_check_gateways($form, $form_state);
  $selected_gateways = $form_state['values']['gateways'];
  foreach ($selected_gateways as $gateway) {
    if (isset($gateway['status']) && !isset($gateways[$gateway['id']])) {
      form_set_error('gateways', t('Invalid gateway selected. Gateway selection does not match group selection.'));
      springboard_group_fundraiser_rebuild_form($form, $form_state);
    }
  }
}

/**
 * Helper function to load valid gateways.
 */
function springboard_group_fundraiser_check_gateways($form, $form_state) {
  // Get the group ID from the form values.
  $language = $form['og_group_ref_' . $form['#node']->type]['#language'];
  if (!empty($form_state['values'])) {
    $option = $form_state['values']['og_group_ref_' . substr($form['#node']->type, 0, 19)][$language];
    $gid = isset($option[0]['target_id']) ? $option[0]['target_id'] : '_none';
  }
  else {
    $option = $form['og_group_ref_' . substr($form['#node']->type, 0, 19)][$language][0]['default'];
    $gid = !empty($option['#default_value'][0]) ? $option['#default_value'][0] : '_none';
  }
  // Get the gateways and filter them by group ownership.
  $all_gateways = _fundraiser_gateway_info();
  $gateways = springboard_group_fundraiser_filter_gateways($all_gateways, $gid);

  return $gateways;
}

function springboard_group_fundraiser_rebuild_form(&$form, &$form_state) {
  $gateways = springboard_group_fundraiser_check_gateways($form, $form_state);

  $node = $form['#node'];
  $lost_a_gateway = FALSE;
  if (!empty($node->gateways)) {
    foreach ($node->gateways as $key => $gateway) {
      if (isset($gateway['id']) && !in_array($gateway['id'], $gateways)) {
        $lost_a_gateway = TRUE;
      }
    }
  }
  // Rebuild the gateway fieldset. This code block is lifted directly from
  // _fundraiser_donation_settings_form() with a slight alteration.
  if (empty($gateways)) {
      $message = t('There are no gateways configured for this group. Please create some gateways in the store configuration.');
      $form_state['group_ajax_messages'][] = $message;

    // Slight alteration to suppress php notice about
    // missing form state values.
    $form['payment_methods']['gateways'] = array(
      '#type' => 'hidden',
      '#value' => array(),
    );
    $form['payment_methods']['gateways']['_default'] = array(
      '#type' => 'hidden',
      '#value' => array(),
    );
  }
  else {
    if ($lost_a_gateway) {
      $message = t('A payment method will be removed. Please review your payment method configuration.');
      $form_state['group_ajax_messages'][] = $message;
    }
    $fieldset = array(
      '#type' => 'fieldset',
      '#title' => t('Payment methods'),
      '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#theme' => 'fundraiser_form_payment_methods',
    );
    $configuration = array();
    $form['payment_methods']['gateways'] = _fundraiser_form_payment_method_config_fieldset($gateways, $configuration, $fieldset, 'gateways');
  }

  // Add the ajax wrapper back to the rebuilt fieldset.
  $form['payment_methods']['gateways']['#prefix'] = '<div id = "springboard-groups-payment-ajax-wrapper">';
  $form['payment_methods']['gateways']['#suffix'] = '</div>';
  if (empty($gateways)) {
    $form['payment_methods']['gateways']['#prefix'] .= '<div>' . $message . '</div>';
  }
}

/**
 * Check group affiliation of gateways.
 *
 * @param $gateways
 * @param null $gid
 *
 * @return mixed
 */
function springboard_group_fundraiser_filter_gateways($gateways, $gid = NULL) {
  foreach ($gateways as $key => $gateway) {

    // Get the gateway's rule configuraiton and checked if it's grouped.
    $name = explode('|', $key);
    $rule = rules_config_load($name[1]);
    $rule_gid = NULL;
    if (!empty($rule->og_group_ref_rules_config[LANGUAGE_NONE][0]['target_id'])) {
      $rule_gid = $rule->og_group_ref_rules_config[LANGUAGE_NONE][0]['target_id'];
    }
    if ($gid == '_none' && !empty($rule_gid)) {
      unset($gateways[$key]);
    }
    elseif ($gid != '_none' && $gid != $rule_gid) {
      unset($gateways[$key]);
    }

  }
  return $gateways;
}