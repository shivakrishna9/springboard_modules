<?php

/**
 * @file
 * Springboard Quick Donation admin functions.
 */

/**
 * Upgrade link generator form.
 */
function springboard_quick_donate_token_generator($form, $form_state) {
  $key = springboard_hmac_get_key();
  if (empty($key)) {
    drupal_set_message(t('Secure Prepopulate key and initialization vector are not set. Upgrade links will not work.'), 'error');
  }

  $defaults = array();
  $default_defaults = array(
    'uid' => 1,
    'amount' => 1000,
    'nid' => 0,
    'did' => 0,
    'ms' => '',
    'cid' => '',
  );
  foreach ($default_defaults as $key => $default) {
    if (isset($_SESSION['qd_token_generate'][$key])) {
      $defaults[$key] = $_SESSION['qd_token_generate'][$key];
    }
    else {
      $defaults[$key] = $default;
    }
  }

  $form['springboard_quick_donate'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick Donate Token'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('To test quick donation tokens, enter the following field information and press "Generate Token".'),
  );

  $form['springboard_quick_donate']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#size' => 20,
    '#default_value' => $defaults['uid'],
    '#description' => t('The UID of the donator.'),
  );

  $form['springboard_quick_donate']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation Amount'),
    '#description' => t('The donation amount in cents. E.g. $20 = 2000'),
    '#default_value' => $defaults['amount'],
  );

  if ($defaults['nid'] < 1) {
    drupal_set_message(t('There are currently no fundraiser nodes, so the node ID will be set to 0. Note that authorization will fail.'), 'warning', FALSE);
  }
  $form['springboard_quick_donate']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Fundraiser Node ID'),
    '#description' => t('The fundraiser node ID, if desired.'),
    '#default_value' => $defaults['nid'],
  );

  $form['springboard_quick_donate']['did'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation ID'),
    '#description' => t('The payment ID, if desired. Note that in order for the authorization to succeed, this must be associated with the node ID.'),
    '#default_value' => $defaults['did'],
  );

  $form['springboard_quick_donate']['ms'] = array(
    '#type' => 'textfield',
    '#title' => t('Market Source'),
    '#description' => t('The market source value.'),
    '#default_value' => $defaults['ms'],
  );

  $form['springboard_quick_donate']['cid'] = array(
    '#type' => 'textfield',
    '#title' => t('Campaign ID'),
    '#description' => t('The campaign ID value.'),
    '#default_value' => $defaults['cid'],
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Token'),
  );

  $form['#submit'][] = 'springboard_quick_donate_token_generator_validate';
  $form['#submit'][] = 'springboard_quick_donate_token_generator_submit';

  return $form;
}

/**
 * Helper function to retrieve a donation node id.
 *
 * @return int
 *   The node ID.
 */
function _springboard_quick_donate_token_generator_get_default_nid() {
  $default_nid = &drupal_static(__FUNCTION__);
  if (!isset($default_nid)) {
    $donation = _springboard_quick_donate_token_generator_get_default_donation();
    if ($donation) {
      $default_nid = $donation->node->nid;
    }
    else {
      $types = array_keys(fundraiser_get_donation_types());
      $query = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('status', 0, '>')
        ->condition('type', $types, 'IN')
        ->orderBy('nid', 'ASC')
        ->range(0, 1)
        ->execute();

      if ($query->rowCount() === 0) {
        $default_nid = 0;
      }
      else {
        $node = $query->fetchObject();
        $default_nid = $node->nid;
      }
    }
  }

  return $default_nid;
}

/**
 * Helper function to retrieve a default donation.
 *
 * @return int
 *   The node ID.
 */
function _springboard_quick_donate_token_generator_get_default_donation() {
  $default_donation = &drupal_static(__FUNCTION__);
  if (!isset($default_donation)) {
    global $user;

    $query = db_select('fundraiser_donation', 'fd')
      ->fields('fd', array('did'))
      ->condition('uid', $user->uid, '=')
      ->orderBy('did', 'ASC')
      ->range(0, 1)
      ->execute();

    if ($query->rowCount() === 0) {
      $donation = FALSE;
    }
    else {
      $donation = $query->fetchObject();
      $donation = fundraiser_donation_get_donation($donation->did);
    }
  }

  return $donation;
}

/**
 * Helper function to retrieve a donation ID.
 *
 * @return int
 *   The node ID.
 */
function _springboard_quick_donate_token_generator_get_default_did() {
  $default_did = &drupal_static(__FUNCTION__);
  if (!isset($default_nid)) {
    $donation = _springboard_quick_donate_token_generator_get_default_donation();
    if ($donation) {
      $default_did = $donation->did;
    }
    else {
      $default_did = 0;
    }
  }

  return $default_did;
}

/**
 * Store temporary upgrade link values in session.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function springboard_quick_donate_token_generator_validate($form, $form_state) {
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
function springboard_quick_donate_token_generator_submit($form, $form_state) {
  $payload = array(
    'uid' => $form_state['values']['uid'],
    'amount' => $form_state['values']['amount'],
    'nid' => $form_state['values']['nid'],
    'did' => $form_state['values']['did'],
    'ms' => $form_state['values']['ms'],
    'cid' => $form_state['values']['cid'],
  );

  $link = springboard_quick_donate_create_link($payload);

  drupal_set_message('Quick donate link generated! ' . l($link, $link));
}
