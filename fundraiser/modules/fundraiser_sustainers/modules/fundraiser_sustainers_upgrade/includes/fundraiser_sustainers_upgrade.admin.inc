<?php


/**
 * Upgrade link generator form.
 */
function fundraiser_sustainers_upgrade_token_generator($form, $form_state) {

  $form['fundraiser_sustainers_upgrade'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sustainers Upgrade Token'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['fundraiser_sustainers_upgrade']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#size' => 20,
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => isset($_SESSION['fsu_token_generate']) ? $_SESSION['fsu_token_generate']['uid'] : '',
    '#required' => TRUE,
  );

  $form['fundraiser_sustainers_upgrade']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Amount'),
    '#description' => t('In cents. $20 = 2000'),
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => isset($_SESSION['fsu_token_generate']) ? $_SESSION['fsu_token_generate']['amount'] : '',
    '#required' => TRUE,

  );

  $form['fundraiser_sustainers_upgrade']['did'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation ID'),
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => isset($_SESSION['fsu_token_generate']) ? $_SESSION['fsu_token_generate']['did'] : '',
    '#required' => TRUE,
  );

  $form['fundraiser_sustainers_upgrade']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Form Node ID'),
    '#element_validate' => array('element_validate_integer_positive'),
    '#default_value' => isset($_SESSION['fsu_token_generate']) ? $_SESSION['fsu_token_generate']['nid'] : '',
    '#required' => TRUE,
    '#description' => t('Invalid node IDs will cause a fallback to the default upgrade form.'),
  );

  $form['fundraiser_sustainers_upgrade']['rollback'] = array(
    '#type' => 'checkbox',
    '#title' => 'Rollback',
    '#default_value' => isset($_SESSION['fsu_token_generate']) ? $_SESSION['fsu_token_generate']['rollback'] : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Upgrade Link'),
  );

  $form['#submit'][] = 'fundraiser_sustainers_upgrade_token_generator_validate';
  $form['#submit'][] = 'fundraiser_sustainers_upgrade_token_generator_submit';
  return $form;
}

/**
 * Store temporary upgrade link values in session.
 *
 * @param $form
 * @param $form_state
 */
function fundraiser_sustainers_upgrade_token_generator_validate($form, $form_state) {
  $_SESSION['fsu_token_generate'] = $form_state['values'];
}

/**
 * Create an upgrade link.
 *
 * @param $form
 * @param $form_state
 */
function fundraiser_sustainers_upgrade_token_generator_submit($form, $form_state) {

  $rollback = FALSE;
  $_SESSION['next_donation'] = array();

  $payload = array(
    'uid' => $form_state['values']['uid'],
    'amount' => $form_state['values']['amount'],
    'did' => $form_state['values']['did'],
    'nid' => $form_state['values']['nid'],
  );

  if ($form_state['values']['rollback'] == 1) {
    $rollback = TRUE;
    $_SESSION['next_donation']['master_did'] = $payload['did'];
    $_SESSION['next_donation']['amount'] = $payload['amount'];
  }

  $link = fundraiser_sustainers_upgrade_create_link($payload, $rollback);

  drupal_set_message('<textarea style="width:90%;padding:10px">' . $link . '</textarea>');

  if (!empty($_SESSION['next_donation'])) {
    unset($_SESSION['next_donation']);
  }
}
