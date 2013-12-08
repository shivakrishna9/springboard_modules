<?php

/**
 * @file
 *
 * Test management service plugin.
 *
 * Provides simplistic authentication, tracking, and throttling for use when debugging.
 */

$plugin = array(
  'name' => 'test',
  'title' => t('Test API Management Service'),
  'authentication_callback' => 'springboard_api_test_service_authenticate',
  'tracking_callback' => 'springboard_api_test_service_track',
  'throttling_callback' => 'springboard_api_test_service_throttle',
);


function springboard_api_test_service_authenticate() {

}

function springboard_api_test_service_track() {

}

function springboard_api_test_service_throttle() {

}
