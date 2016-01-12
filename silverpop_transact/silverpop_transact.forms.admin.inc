<?php

/**
 * @file
 * silverpop_transact.forms.admin.inc
 *
 * @TODO check login credentials on configuration form submit.
 */

/**
 * Silverpop Sendmail admin form.
 */
function silverpop_transact_admin() {
  $form = array();

  $form['silverpop_transact_model'] = array(
    '#type' => 'radios',
    '#title' => t('Silverpop Transact API mode'),
    '#options' => array(
      'SMTP' => t('SMTP'),
      'XML' => t('XML'),
    ),
    '#default_value' => variable_get('silverpop_transact_model', 'SMTP'),
    '#description' => 'See module documentation for more information about these two options.',
    '#required' => TRUE,
  );

  $form['silverpop_transact_api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Silverpop API Credentials'),
    '#states' => array(
      'visible' => array(
        ':input[name="silverpop_transact_model"]' => array('value' => 'XML'),
      ),
    ),
  );

  $form['silverpop_transact_api']['silverpop_transact_mailing_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Silverpop Mailing ID'),
    '#default_value' => variable_get('silverpop_transact_mailing_id'),
    '#states' => array(
      'required' => array(
        ':input[name="silverpop_transact_model"]' => array('value' => 'XML'),
      ),
    ),
  );

  $form['silverpop_transact_api']['silverpop_transact_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Silverpop Transact username'),
    '#default_value' => variable_get('silverpop_transact_username', ''),
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => 'The username you use to log into the Silverpop
        administrative portal.',
    '#states' => array(
      'required' => array(
        ':input[name="silverpop_transact_model"]' => array('value' => 'XML'),
      ),
    ),
  );

  $form['silverpop_transact_api']['silverpop_transact_password'] = array(
    '#type' => 'password',
    '#title' => t('Silverpop Transact password'),
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => 'The password you use to log into the Silverpop
        administrative portal.',
    '#states' => array(
      'required' => array(
        ':input[name="silverpop_transact_model"]' => array('value' => 'XML'),
      ),
    ),
  );

  $form['silverpop_transact_apihost'] = array(
    '#type' => 'select',
    '#title' => t('Silverpop API host'),
    '#options' => array(
      '1' => '1',
      '2' => '2',
      '3' => '3',
      '4' => '4',
      '5' => '5',
    ),
    '#default_value' => variable_get('silverpop_transact_apihost'),
    '#description' => 'Your API host is typically your engage pod number (e.g.
        Pod 5 would use api5.silverpop.com).',
    '#required' => TRUE,
  );

  $form['#submit'] = array('silverpop_transact_encrypt_password', 'system_settings_form_submit');

  $form = system_settings_form($form);

  return $form;
}

/**
 * Encrypt password (if necessary).
 *
 * Function borrowed from http://drupal.org/project/silverpop.
 */
function silverpop_transact_encrypt_password($form, &$form_state) {
  // Encrypt the API password.
  if (!empty($form_state['values']['silverpop_transact_password'])) {
    $password = $form_state['values']['silverpop_transact_password'];
    $form_state['values']['silverpop_transact_password'] = silverpop_transact_set_password($password);
  }
  else {
    $form_state['values']['silverpop_transact_password'] = silverpop_transact_get_password(FALSE);
  }
}
