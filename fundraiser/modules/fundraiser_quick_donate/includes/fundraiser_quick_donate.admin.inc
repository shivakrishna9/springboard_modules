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
  // Using db_select here because we need to also join the node table.
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

/**
 * Upgrade link generator form.
 */
function fundraiser_quick_donate_token_generator($form, $form_state) {
  $key = springboard_hmac_get_key();
  if (empty($key)) {
    drupal_set_message(t('Secure Prepopulate key and initialization vector are not set. Upgrade links will not work.'), 'error');
  }

  $defaults = array();
  $default_defaults = array(
    'uid' => 1,
    'card_id' => 1,
    'gateway_id' => 'authnet_aim|commerce_payment_authnet_aim',
  );
  foreach ($default_defaults as $key => $default) {
    if (isset($_SESSION['qd_token_generate'][$key])) {
      $defaults[$key] = $_SESSION['qd_token_generate'][$key];
    }
    else {
      $defaults[$key] = $default;
    }
  }

  $form['fundraiser_quick_donate_springboard_hmac'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick Donate Token'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('To test quick donation tokens, enter the following field information and press "Generate Token".'),
  );

  $form['fundraiser_quick_donate_springboard_hmac']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#size' => 20,
    '#default_value' => $defaults['uid'],
    '#description' => t('The UID of the donator.'),
  );

  $form['fundraiser_quick_donate_springboard_hmac']['card_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Card ID'),
    '#description' => t('The card ID.'),
    '#default_value' => $defaults['card_id'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Token'),
  );

  $form['#submit'][] = 'fundraiser_quick_donate_token_generator_validate';
  $form['#submit'][] = 'fundraiser_quick_donate_token_generator_submit';

  return $form;
}

/**
 * Store temporary upgrade link values in session.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function fundraiser_quick_donate_token_generator_validate($form, $form_state) {
  $_SESSION['qd_token_generate'] = $form_state['values'];
}

/**
 * Create an upgrade link.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function fundraiser_quick_donate_token_generator_submit($form, $form_state) {
  $payload = array(
    'uid' => $form_state['values']['uid'],
    'card_id' => $form_state['values']['card_id'],
  );

  $link = fundraiser_quick_donate_create_link($payload);

  drupal_set_message('Quick donate link generated! ' . l($link, $link));
}
