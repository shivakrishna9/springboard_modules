<?php

/**
 * @file
 * Admin functions for Springboard HMAC.
 */

/**
 * Page callback for the admin form.
 *
 * @return array $build
 *   The render array.
 */
function springboard_hmac_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Springboard HMAC.') . '</p>',
      '#prefix' => '<div class="springboard-hmac-settings-page">',
      '#suffix' => '</div>',
    ),
    'springboard_hmac_settings_form' => drupal_get_form('springboard_hmac_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard advocacy.
 */
function springboard_hmac_settings_form() {

  $key = springboard_hmac_get_key();
  if (empty($key)) {
    drupal_set_message(t('Secure Prepopulate key and initialization vector are not set. Springboard HMAC will not function.'), 'error');
  }

  $form = array();

  $form['hmac'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $format = NULL;
  $message = variable_get('springboard_hmac_error_message', array('value' => 'An error occurred and we could not complete the operation.', 'format' => NULL));
  if (is_array($message)) {
    $text = $message['value'];
    $format = $message['format'];
  }
  else {
    $text = $message;
  }

  $form['hmac']['springboard_hmac_error_message'] = array(
    '#type' => 'text_format',
    '#format' => $format,
    '#title' => t('Error message'),
    '#default_value' => $text,
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
