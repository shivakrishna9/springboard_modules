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
 * Configuration settings form for Springboard advocacy.
 */
function springboard_dw_settings_form() {
  $form = array();

  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Data warehouse client id'),
  );

  return $form;
}