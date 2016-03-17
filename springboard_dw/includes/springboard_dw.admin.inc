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

  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse client id'),
    '#default_value' => variable_get('client_id', NULL),
    '#required' => TRUE
  );

  $form['authentication_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse authentication token'),
    '#default_value' => variable_get('authentication_token', NULL),
    '#required' => TRUE
  );

  return system_settings_form($form);
}