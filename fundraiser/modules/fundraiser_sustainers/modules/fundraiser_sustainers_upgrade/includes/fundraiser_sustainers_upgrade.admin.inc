<?php

/**
 * Page callback for the admin form.
 */


/**
 * Token Generator form.
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
  );
  $form['fundraiser_sustainers_upgrade']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Amount'),
    '#description' => t('In cents. $20 = 2000')
  );
  $form['fundraiser_sustainers_upgrade']['did'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation ID'),
  );
  $form['fundraiser_sustainers_upgrade']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Upgrade Form Node ID'),
  );
  $form['fundraiser_sustainers_upgrade']['rollback'] = array(
    '#type' => 'checkbox',
    '#title' => 'Rollback'
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Upgrade Link'),
  );

  $form['#submit'][] = 'fundraiser_sustainers_upgrade_token_generator_submit';
  return $form;
}


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

  drupal_set_message($link);

  if (!empty($_SESSION['next_donation'])) {
    unset($_SESSION['next_donation']);
  }
}
