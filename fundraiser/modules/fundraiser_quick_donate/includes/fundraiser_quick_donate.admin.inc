<?php

/**
 * @file
 * Springboard Quick Donation admin functions.
 */

/**
 * Admin settings form.
 */
function fundraiser_quick_donate_admin_settings($form, $form_state) {
  $form['default_forms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default Forms'),
    '#description' => t('When a user is directed to a donation form from an HMAC token with an unsupported payment gateway, they will be redirected to a default form that does support the payment gateway.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $gateways = array_keys(_fundraiser_gateway_info());
  foreach ($gateways as $gateway_key) {
    $default = '';
    $data = variable_get(FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX . $gateway_key, array());
    if (!empty($data)) {
      $data = unserialize($data);
      $default = $data['title'];
    }
    $form['default_forms'][FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX . $gateway_key] = array(
      '#type' => 'textfield',
      '#title' => t('Default node ID for @gateway', array(
        '@gateway' => $gateway_key,
      )),
      '#description' => t('The node ID of the form you want to be used as default.'),
      '#autocomplete_path' => 'admin/springboard/options/fundraiser/fundraiser_quick_donate/autocomplete/' . $gateway_key,
      '#default_value' => $default,
    );
  }

  $form['#submit'][] = 'fundraiser_quick_donate_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Autocomplete callback function for fundraiser_quick_donate_admin_settings().
 */
function fundraiser_quick_donate_admin_settings_autocomplete_callback($gateway_key, $string = '') {
  $result = db_select('fundraiser', 'f');
  $result->join('node', 'n', 'f.nid = n.nid');
  $result = $result->fields('n', array('nid', 'title'))
    ->fields('f', array('gateways'))
    ->condition('f.gateways', '%' . db_like($gateway_key) . '%', 'LIKE')
    ->condition('n.title', db_like($string) . '%', 'LIKE')
    ->range(0, 10)
    ->execute()
    ->fetchAll();
  $matches = array();
  foreach ($result as $node) {
    $matches[$node->title . " [$node->nid]"] = check_plain($node->title);
  }
  drupal_json_output($matches);
}

/**
 * Submission handler for fundraiser_quick_donate_admin_settings().
 */
function fundraiser_quick_donate_admin_settings_submit($form, &$form_state) {
  dsm($form_state);
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX) === FALSE) {
      continue;
    }
    $match = preg_match('/.*\[([0-9]+)\].*/', $value, $matches);
    if ($match !== FALSE && count($matches) < 2) {
      continue;
    }
    $data = array(
      'title' => $form_state['values'][$key],
      'nid' => $matches[1],
    );
    $form_state['values'][$key] = serialize($data);
  }
}
