<?php

/**
 * @file
 * Admin functions for Springboard data warehouse.
 */

/**
 * Page callback for the admin form.
 */

function springboard_dw_settings_page() {
  return drupal_get_form('springboard_dw_settings_form');
}

/**
 * Configuration settings form for Springboard data warehousing.
 */
function springboard_dw_settings_form() {
  $form = array();

  $form['springboard_dw_api_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse server url: (I.E https://warehouse.gospringboard.io)'),
    '#default_value' => variable_get('springboard_dw_api_endpoint', NULL),
    '#required' => TRUE
  );

  $form['springboard_dw_authentication_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse authentication token'),
    '#default_value' => variable_get('springboard_dw_authentication_token', NULL),
    '#required' => TRUE
  );

  $form['springboard_dw_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse client id'),
    '#default_value' => variable_get('springboard_dw_client_id', NULL),
    '#required' => TRUE
  );

  return system_settings_form($form);
}