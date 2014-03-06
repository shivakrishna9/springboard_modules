<?php

/**
 * @file
 * Test management service plugin.
 * Provides simplistic authentication, tracking, and throttling for use when
 * debugging.
 */

$plugin = array(
  'name' => 'basic',
  'title' => t('Basic API Management Service'),
  'authentication_callback' => 'springboard_api_basic_service_authenticate',
  'tracking_callback' => 'springboard_api_basic_service_track',
  'throttling_callback' => 'springboard_api_basic_service_throttle',
  'admin_settings' => 'springboard_api_basic_service_settings',
  'menu_items' => 'springboard_api_basic_service_menu_items',
);

/**
 * Authentication callback.
 */
function springboard_api_basic_service_authenticate($op, $data) {
  return TRUE;
}

/**
 * API usage tracking callback.
 */
function springboard_api_basic_service_track($endpoint, $data = '') {
  springboard_api_log_service_usage($endpoint, $data);
}

/**
 * API usage throttling callback.
 */
function springboard_api_basic_service_throttle() {
  if (variable_get('springboard_api_basic_service_enable_throttling', 0)) {
    // Throttling enabled, check conditions.
    $limit = variable_get('springboard_api_basic_service_throttle_limit', 0);
    if ($limit > 0) {
      $count = _springboard_api_tracker_count();
      if ($count > $limit) {
        return services_error("API request limits exceeded.", 403);
      }

    }
    return TRUE;
  }
}

/**
 * Admin settings form for plugin.
 */
function springboard_api_basic_service_settings(&$form, $form_state) {
  $form['additional_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings for Basic Service Management plugin'),
    '#states' => array(
      'visible' => array(
        ':input[name="springboard_api_management_service"]' => array('value' => 'basic'),
      ),
    ),
  );
  $form['additional_settings']['springboard_api_basic_service_track_usage'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track API usage'),
    '#default_value' => variable_get('springboard_api_basic_service_track_usage', 0),
  );
  $form['additional_settings']['springboard_api_basic_service_enable_throttling'] = array(
    '#type' => 'textfield',
    '#title' => t('Enable API Throttling'),
    '#description' => t('Throttling attempts to limit the impact of API calls on your web server by limiting the number of calls that can be made over a period of time.'),
    '#default_value' => variable_get('springboard_api_test_service_enable_throttling', 0),
  );
  $form['additional_settings']['springboard_api_basic_service_throttle_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Max API calls per minute.'),
    '#description' => t('Maximum number of API calls per endpoint in 60 seconds.'),
    '#default_value' => variable_get('springboard_api_test_service_throttle_limit', 0),
  );
  $form['api_keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('App Keys'),
    '#states' => array(
      'visible' => array(
        ':input[name="springboard_api_management_service"]' => array('value' => 'basic'),
      ),
    ),
  );
}

/**
 * Get endpoint usage in the last minute.
 */
function _springboard_api_tracker_count() {
  $end = time();
  $start = $end - 60;
  $result = db_query('SELECT COUNT(time) FROM {springboard_api_tracker} WHERE time >= :start AND time <= :end', array(':start' => $start, ':end' => $end));
  return $result->fetchField();
}

/**
 * Add menu paths to add/edit/delete api keys.
 *
 * @param array $items
 *   hook_menu() $items array.
 */
function springboard_api_basic_service_menu_items(&$items) {

}