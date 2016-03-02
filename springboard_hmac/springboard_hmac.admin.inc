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
  $form = array();

  $form['hmac'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['hmac']['springboard_hmac_error_message'] = array(
    '#title' => t('Error message'),
    '#description' => t('Message displayed to the user when there is an authentication error.'),
    '#type' => 'textarea',
    '#default_value' => variable_get('springboard_hmac_error_message', ''),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
