<?php

/**
 * @file
 * Admin functions for Springboard advocacy.
 */

/**
 * Page callback for the admin form.
 */
function springboard_advocacy_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Springboard advocacy.') . '</p>',
      '#prefix' => '<div class="springboard-advocacy-settings-page">',
      '#suffix' => '</div>',
    ),
    'salesforce_sync_admin_form' => drupal_get_form('springboard_advocacy_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard advocacy.
 */
function springboard_advocacy_settings_form() {
  $form = array();

  $form['springboard_advocacy_server_url'] = array(
    '#title' => t('Springboard advocacy server url'),
    '#description' => t('The url of the Springboard advocacy server.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('springboard_advocacy_server_url', ''),
  );

  $form['transaction']['springboard_advocacy_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client ID.'),
    '#description' => t('Client ID for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_id', ''),
  );

  $form['transaction']['springboard_advocacy_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client Secret.'),
    '#description' => t('Client Secret for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_secret', ''),
  );

  return system_settings_form($form);
}
