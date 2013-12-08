<?php

/**
 * @file
 * Admin settings screens and associated helper functions.
 */

/**
 *
 */
function springboard_api_admin_settings() {
  $available_services = springboard_api_list_management_services();
  $options = array_merge(array('0' => t('- select -')), $available_services);

  $form['springboard_api_management_service'] = array(
    '#type' => 'select',
    '#title' => t('API Management Service'),
    '#description' => t('Select which API management service you would like to use.'),
    '#options' => $options,
    '#default_value' => variable_get('springboard_api_management_service', 0),
  );

  return system_settings_form($form);
}
