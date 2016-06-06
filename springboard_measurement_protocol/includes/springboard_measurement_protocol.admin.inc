<?php

/**
 * @file
 * Admin file for the Springboard Measurement Protocol module.
 */

/**
 * Page callback for admin settings.
 */
function springboard_measurement_protocol_admin() {
  if (module_exists('googleanalytics')) {
    $form['paypal_events'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => t('PayPal Events'),
    );
    $form['paypal_events']['springboard_measurement_protocol_paypal_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Send events for PayPal transactions'),
      '#description' => t("Send analytics to Google's Measurement Protocol service when a PayPal transaction occurs."),
      '#default_value' => variable_get('springboard_measurement_protocol_paypal_enable', FALSE),
    );
  }

  return system_settings_form($form);
}
