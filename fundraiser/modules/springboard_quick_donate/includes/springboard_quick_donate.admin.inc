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
    '#default_value' => 1,
    '#description' => t('The UID of the donator.'),
  );

  $form['springboard_quick_donate']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Amount'),
    '#description' => t('The donation amount in cents. E.g. $20 = 2000'),
    '#default_value' => 1000,
  );

  $default_nid = _springboard_quick_donate_token_generator_get_default_nid();
  if ($default_nid < 0) {
    drupal_set_message(t('There are currently no fundraiser nodes, so the node ID will be set to 0. Note that authorization will fail.'), 'warning', FALSE);
    $default_nid = 0;
  }
  $form['springboard_quick_donate']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Fundraiser Node ID'),
    '#description' => t('The fundraiser node ID, if desired.'),
    '#default_value' => $default_nid,
  );

  $default_did = _springboard_quick_donate_token_generator_get_default_did();
  $form['springboard_quick_donate']['did'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation ID'),
    '#description' => t('The payment ID, if desired.'),
    '#default_value' => $default_did,
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
    $types = array_keys(fundraiser_get_donation_types());
    $query = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->condition('status', 0, '>')
      ->condition('type', $types, 'IN')
      ->orderBy('nid', 'ASC')
      ->range(0, 1)
      ->execute();

    if ($query->rowCount() === 0) {
      $default_nid = -1;
    }
    else {
      $node = $query->fetchObject();
      $default_nid = $node->nid;
    }
  }

  return $default_nid;
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
    global $user;

    $query = db_select('fundraiser_donation', 'fd')
      ->fields('fd', array('did'))
      ->condition('uid', $user->uid, '=')
      ->orderBy('did', 'ASC')
      ->range(0, 1)
      ->execute();

    if ($query->rowCount() === 0) {
      $default_did = -1;
    }
    else {
      $donation = $query->fetchObject();
      $default_did = $donation->did;
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
  );

  $link = springboard_quick_donate_create_link($payload);

  drupal_set_message('Quick donate link generated! ' . l($link, $link));
}
