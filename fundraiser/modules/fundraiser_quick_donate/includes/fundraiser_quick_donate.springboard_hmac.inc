<?php

/**
 * @file
 * springboard_hmac hooks for Springboard Quick Donate.
 */


/**
 * Create HMAC token link for quick donations.
 *
 * @param array $payload
 *   The payload values.
 *
 * @return string
 *   The quick donation url.
 */
function fundraiser_quick_donate_create_link($payload, $rollback = FALSE) {
  $link = '';
  $amount = $payload['amount'];

  $time = time() + (60 * 60 * 24 * 30);
  $values = array(
    $payload['uid'],
    $time,
    'qd',
    $amount,
    $payload['did'],
    $payload['nid'],
    $payload['ms'],
    $payload['cid'],
  );

  $payload_string = implode('.', $values);

  $payload_string = base64_encode($payload_string);
  $key = springboard_hmac_get_key();
  $hmac = springboard_hmac_create_hmac($values, $key);

  $link = url('hmac/' . $payload_string . '/' . $hmac, array('absolute' => TRUE));

  return $link;
}

/**
 * Implements hook_springboard_hmac_allowed_actions().
 */
function fundraiser_quick_donate_springboard_hmac_allowed_actions_alter(&$actions) {
  $actions[] = 'qd';
}

/**
 * Implements hook_springboard_hmac_success().
 *
 * Redirect to the donation form if hmac validated.
 */
function fundraiser_quick_donate_springboard_hmac_success($payload) {
  if ($payload['action'] != 'qd') {
    return;
  }

  $mapping = _fundraiser_quick_donate_hmac_mapping();
  $nid = $payload[$mapping['fundraiser_id']];
  $parameters = drupal_get_query_parameters();
  if (isset($parameters['form_id']) && is_numeric($parameters['form_id'])) {
    $nid = $parameters['form_id'];
  }
  $node = node_load($nid);

  // Normally, the conditional statements below would be placed in the verify
  // function. But since we need to redirect to a default form node if one is
  // set, we need to do it here instead because of the way the springboard_hmac
  // module is structured.

  $did = $payload[$mapping['donation_id']];
  $donation = fundraiser_donation_get_donation($did);
  $gateway_id = $donation->gateway['id'];
  $default_data = variable_get(FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX . $gateway_id, array());
  $default_nid = 0;
  if (!empty($default_data)) {
    $default_data = unserialize($default_data);
    $default_nid = $default_data['nid'];
  }

  if (!fundraiser_is_donation_type($node->type)) {
    _fundraiser_quick_donate_redirect_to_default_node($default_nid);
    return springboard_hmac_failure(t('Not a valid node type.'), $payload);
  }

  if (empty($node->gateways)) {
    _fundraiser_quick_donate_redirect_to_default_node($default_nid);
    return springboard_hmac_failure(t('No gateway information available.'), $payload);
  }

  if (empty($donation->gateway['payment_method'])) {
    _fundraiser_quick_donate_redirect_to_default_node($default_nid);
    return springboard_hmac_failure(t('No payment information available.'), $payload);
  }

  $payment_method = $donation->gateway['payment_method'][0];
  if (!array_key_exists($payment_method, $node->gateways) || !$node->gateways[$payment_method]['status'] || $donation->gateway['id'] !== $node->gateways[$payment_method]['id']) {
    _fundraiser_quick_donate_redirect_to_default_node($default_nid);
    return springboard_hmac_failure(t('The payment method is not available for the specified node.'), $payload);
  }

  if (!node_access('view', $node)) {
    _fundraiser_quick_donate_redirect_to_default_node($default_nid);
    return springboard_hmac_failure(t('Access denied for current user.'), $payload);
  }

  $status = TRUE;
  // Need to do some more validation at this point since we don't want to show
  // an access denied message.
  if (!user_is_logged_in()) {
    $cookie = springboard_cookie_get_cookie();
    if (isset($cookie['uid']) && $cookie['uid'] != $payload[$mapping['uid']]) {
      // Set a flag to tell us later that uid authentication failed, so we can
      // set an appropriate message on the donation form.
      $_SESSION['springboard_hmac']['quickdonate'] = array(
        'status' => FALSE,
        'reason' => FUNDRAISER_QUICK_DONATE_UID_NOT_EQUAL_TOKEN,
      );
      $status = FALSE;
    }
  }

  // Move quickdonate values into their own array for our own sanity.
  if ($status) {
    foreach ($mapping as $key) {
      $_SESSION['springboard_hmac']['quickdonate'][$key] = $payload[$key];
      unset($_SESSION['springboard_hmac'][$key]);
    }
  }

  $parameters = drupal_get_query_parameters();
  if (isset($payload[2]) && is_numeric($payload[2]) || isset($parameters['form_id']) && is_numeric($parameters['form_id'])) {
    // Default for the form_id parameter if it's set, otherwise the payload nid.
    $nid = isset($parameters['form_id']) ? $parameters['form_id'] : $payload[2];
    $node = node_load($nid);
    if (!empty($node->type) && fundraiser_is_donation_type($node->type)) {
      drupal_goto('node/' . $node->nid);
    }
  }
}

/**
 * Helper function to redirect to a node if $nid > 0.
 */
function _fundraiser_quick_donate_redirect_to_default_node($nid = 0) {
  if ($nid) {
    drupal_goto('node/' . $nid);
  }
}

/**
 * Implements hook_springboard_hmac_failure().
 *
 * Redirect to the donation form if hmac not validated.
 */
function fundraiser_quick_donate_springboard_hmac_failure($reason, $payload) {
  $mapping = _fundraiser_quick_donate_hmac_mapping();

  // If the Springboard cookie is set and matches the uid from the token,
  // redirect user to donation form. This scenario is treated as a success
  // condition.
  $cookie = springboard_cookie_get_cookie();
  if (!empty($cookie['uid']) && $cookie['uid'] == $payload[$mapping['uid']]) {
    drupal_goto(drupal_get_path_alias('node/' . $payload[$mapping['fundraiser_id']]));
  }

  // Otherwise this is a failure condition. Output the reason if configured to
  // do so.
  if (variable_get('fundraiser_quick_donate_debug', FALSE)) {
    drupal_set_message($reason, 'warning', FALSE);
  }

  // Redirect to the login form.
  drupal_session_start();
  $_SESSION['quickdonate'] = TRUE;

  $nid = $payload[$mapping['fundraiser_id']];
  drupal_goto('/user', array(
    'query' => array(
      'destination' => drupal_get_path_alias('node/' . $nid),
    ),
  ));
}

/**
 * Implements hook_springboard_hmac_verify_payload().
 */
function fundraiser_quick_donate_springboard_hmac_verify_payload($payload) {
  $qd_hmac_mapping = _fundraiser_quick_donate_hmac_mapping();
  $did = $payload[$qd_hmac_mapping['donation_id']];
  if ($did > 0) {
    $donation = fundraiser_donation_get_donation($did);
  }
  else {
    return t('No donation information available.');
  }

  if (empty($donation->gateway['id'])) {
    // This should never happen, but as a precaution.
    return t('No gateway information available.');
  }

  return NULL;
}

/**
 * Helper function to get a mapping of hmac values for quick donations.
 *
 * Needed since by default the array isn't mapped by key but instead by index.
 *
 * @return array
 *   A map of hmac types to their corresponding keys.
 */
function _fundraiser_quick_donate_hmac_mapping() {
  return array(
    'amount' => 0,
    'donation_id' => 1,
    'uid' => 'uid',
    'fundraiser_id' => 2,
    'ms' => 3,
    'cid' => 4,
  );
}
