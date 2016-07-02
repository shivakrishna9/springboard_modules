<?php

/**
 * @file
 * Springboard Quick Donation admin functions.
 */

/**
 * Upgrade link generator form.
 */
function springboard_quick_donation_token_generator($form, $form_state) {
  $key = springboard_hmac_get_key();
  if (empty($key)) {
    drupal_set_message(t('Secure Prepopulate key and initialization vector are not set. Upgrade links will not work.'), 'error');
  }

  $form['springboard_quick_donation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick Donate Token'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('To test quick donation tokens, enter the following field information and press "Generate Token".'),
  );

  $form['springboard_quick_donation']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#size' => 20,
    '#default_value' => 1,
    '#description' => t('The UID of the donator.'),
  );

  $form['springboard_quick_donation']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Amount'),
    '#description' => t('The donation amount in cents. E.g. $20 = 2000'),
    '#default_value' => 1000,
  );

  $form['springboard_quick_donation']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Fundraiser Node ID'),
    '#description' => t('The fundraiser node ID, if desired.'),
    '#default_value' => 0,
  );

  $form['springboard_quick_donation']['pid'] = array(
    '#type' => 'textfield',
    '#title' => t('Payment ID'),
    '#description' => t('The payment ID, if desired.'),
    '#default_value' => 0,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Token'),
  );

  $form['#submit'][] = 'springboard_quick_donation_token_generator_validate';
  $form['#submit'][] = 'springboard_quick_donation_token_generator_submit';

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
function springboard_quick_donation_token_generator_validate($form, $form_state) {

}

/**
 * Create an upgrade link.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function springboard_quick_donation_token_generator_submit($form, $form_state) {
  $payload = array(
    'uid' => $form_state['values']['uid'],
    'amount' => $form_state['values']['amount'],
    'nid' => $form_state['values']['nid'],
    'pid' => $form_state['values']['pid'],
  );

  $link = springboard_quick_donation_create_link($payload, $rollback);

  drupal_set_message('Quick donate link generated! ' . l($link, $link));
}
