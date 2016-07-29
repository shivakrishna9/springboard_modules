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
  if ($payload['action'] == 'qd') {
    $status = TRUE;
    $mapping = _fundraiser_quick_donate_hmac_mapping();
    // Need to do some more validation at this point since we don't want to show
    // an access denied message.
    if (!user_is_logged_in()) {
      $cookie = springboard_cookie_get_cookie();
      if (isset($cookie['uid']) && $cookie['uid'] != $payload[$mapping['uid']]) {
        // Set a flag to tell us later that uid authentication failed, so we can
        // set an appropriate message on the donation form.
        $_SESSION['springboard_hmac']['quickdonate'] = array(
          'status' => FALSE,
          'reason' => fundraiser_quick_donate_UID_NOT_EQUAL_TOKEN,
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

    if (isset($payload[2]) && is_numeric($payload[2])) {
      $node = node_load($payload[2]);
      if (!empty($node->type) && fundraiser_is_donation_type($node->type)) {
        drupal_goto('node/' . $node->nid);
      }
    }
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
    global $user;
    if (module_exists('devel') && user_access('access devel information', $user)) {
      dpm($reason);
    }
    else {
      drupal_set_message($reason, 'warning', FALSE);
    }
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

  $nid = $payload[$qd_hmac_mapping['fundraiser_id']];
  $node = node_load($nid);

  if (!fundraiser_is_donation_type($node->type)) {
    return t('Not a valid node type.');
  }

  if (empty($node->gateways)) {
    return t('No gateway information available.');
  }

  if (empty($donation->gateway['payment_method'])) {
    return t('No payment information available.');
  }

  $payment_method = $donation->gateway['payment_method'][0];
  if (!array_key_exists($payment_method, $node->gateways) || !$node->gateways[$payment_method]['status'] || $donation->gateway['id'] !== $node->gateways[$payment_method]['id']) {
    return t('The payment method is not available for the specified node.');
  }

  return NULL;
}

/**
 * Helper function to get a mapping of hmac values for quick donations.
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
