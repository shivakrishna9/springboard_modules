<?php
/**
 * @file
 * Secure prepopulate admin functions
 */

/**
 * System settings form for secure prepopulate.
 */
function secure_prepopulate_admin_settings(&$form_state) {
  $form = array();
  $key = variable_get('secure_prepopulate_key', '') ? decrypt(variable_get('secure_prepopulate_key', '')) : '';
  $vector = variable_get('secure_prepopulate_iv', '') ? decrypt(variable_get('secure_prepopulate_iv', '')) : '';
  $form['secure_prepopulate_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Encryption key'),
    '#description' => t('To use 128 bit encryption enter a 16 byte (128 bit) AES key. 256 bit encryption requires a 32 byte key (256 bits).'),
    '#default_value' => $key,
  );

  $form['secure_prepopulate_iv'] = array(
    '#type' => 'textfield',
    '#title' => t('Initialization vector'),
    '#description' => t('A 16 byte initialization vector. The initialization vector (IV) is a fixed-size input to a cryptographic primitive that is typically required to be random or pseudorandom.'),
    '#default_value' => $vector,
  );

  $form['secure_prepopulate_precedence'] = array(
    '#type' => 'radios',
    '#title' => t('Pre-populate precedence'),
    '#description' => t('Determines which set of values take precedence when the user is already logged in.'),
    '#options' => array(
      'url' => 'Encrypted URL values',
      'drupal' => 'Drupal values',
    ),
    '#default_value' => variable_get('secure_prepopulate_precedence', 'url'),
  );

  return system_settings_form($form);
}

/**
 * Settings validation routine.
 */
function secure_prepopulate_admin_settings_validate(&$form, &$form_state) {
  if (mb_strlen($form_state['values']['secure_prepopulate_key']) != 32 && mb_strlen($form_state['values']['secure_prepopulate_key']) != 16) {
    form_set_error('secure_prepopulate_key', 'Key must be either 128 bits (16 bytes) or 256 bits (32 bytes) in length.');
  }
  else {
    $form_state['values']['secure_prepopulate_key'] = encrypt($form_state['values']['secure_prepopulate_key']);
  }

  if (mb_strlen($form_state['values']['secure_prepopulate_iv']) != 16) {
    form_set_error('secure_prepopulate_iv', 'Initialization vector must be either 128 bits (16 bytes) in length.');
  }
  else {
    $form_state['values']['secure_prepopulate_iv'] = encrypt($form_state['values']['secure_prepopulate_iv']);
  }
}

/**
 * Test string previewer
 */
function secure_prepopulate_preview(&$form_state) {
  if ($encrypted = $form_state['values']['encrypted']) {
    $decrypted = urldecode($encrypted);
    $decrypted = secure_prepopulate_decrypt($decrypted);
  }
  
  $form = array();
  if (!$encrypted) {
    $form['intro'] = array(
      '#type' => 'markup',
      '#value' => t('To verify the contents of a Secure Prepopulate string produced by Salesforce, enter it below exactly as it would be appended to a URL.'),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }
  $form['encrypted'] = array(
    '#type' => 'textarea',
    '#title' => t('Encrypted'),
    '#rows' => 3,
    '#required' => TRUE,
    '#default_value' => $encrypted,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Preview'),
  );
  $form['string_output'] = array(
    '#type' => 'textarea',
    '#title' => t('Decrypted'),
    '#rows' => 6,
    '#attributes' => array('readonly' => '1'),
    '#value' => print_r($decrypted,true),
  );
  return $form;
}

function secure_prepopulate_preview_submit(&$form, &$form_state) {
  $is_expired = secure_prepopulate_is_expired($encrypted);
  drupal_set_message($is_expired ? t('String is expired') : t('String is not expired'));
  $form_state['rebuild'] = TRUE;
}
