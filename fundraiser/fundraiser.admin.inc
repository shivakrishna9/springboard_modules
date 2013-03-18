<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Form builder. Configure fundraiser module.
 */
function fundraiser_admin_settings($form, &$form_state) {
  $form['fundraiser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fundraiser general settings'),
    '#description' => t('Configure fundraiser settings.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['fundraiser']['fundraiser_development_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Development mode'),
    '#default_value' => variable_get('fundraiser_development_mode', 0),
    '#description' => t('Development mode allows you to access donation forms over HTTP.'),
  );
  $form['fundraiser']['fundraiser_http_error_message'] = array(
    '#type' => 'textarea',
    '#title' => t('HTTPS error message'),
    '#default_value' => variable_get('fundraiser_http_error_message', t('We\'re sorry. The donation form you are trying ' .
        'to access has not been secured properly. Safeguarding your personal information is extremely important ' .
        'to us, therefore we cannot accept donations from this form at this time.')),
    '#description' => t('The message to display to the user when a donation page is not secure and the site is in ' .
      'NOT in development mode.'),
  );
  $form['fundraiser']['fundraiser_default_minimum'] = array(
    '#type' => 'textfield',
    '#title' => t('Default minimum donation amount'),
    '#default_value' => variable_get('fundraiser_default_minimum', 10.00),
    '#description' => t('Enter the default minimum donation amount that will be used on new donation forms.'),
  );
  $form['fundraiser']['fundraiser_default_amounts'] = array(
    '#type' => 'textarea',
    '#title' => t('Default amounts'),
    '#default_value' => variable_get('fundraiser_default_amounts', '10|$10' . "\n" . '20|$20' . "\n" . '50|$50' . "\n" . '100|$100'),
    '#description' => t('Enter the default donation amounts that will be used on new donation forms.'),
  );
  $form['fundraiser']['fundraiser_default_other'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use other amount by default'),
    '#default_value' => variable_get('fundraiser_default_other', TRUE),
    '#description' => t('Check this box to automatically configure donation forms to use the other amount.'),
  );
  $form['fundraiser']['fundraiser_receipt_bcc'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation receipt bcc email address'),
    '#default_value' => variable_get('fundraiser_receipt_bcc', ''),
    '#description' => t('The email address of the user that will receive a bcc copy of all donation receipt emails.'),
  );
  $form['fundraiser']['fundraiser_gateway_messages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display gateway messages'),
    '#default_value' => variable_get('fundraiser_gateway_messages', 1),
    '#description' => t('When checked, the message that is returned from the payment gateway will be displayed in the Drupal status message area of the theme.'),
  );
  $form['fundraiser']['fundraiser_log_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log comments'),
    '#default_value' => variable_get('fundraiser_log_comments', 0),
    '#description' => t('Log comments made on donations to watchdog.'),
  );
  return system_settings_form($form);
}
