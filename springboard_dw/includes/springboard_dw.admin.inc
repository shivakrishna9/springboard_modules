<?php

/**
 * @file
 * Admin functions for Springboard data warehouse.
 */

/**
 * Page callback for the Springboard data warehouse admin form.
 */

function springboard_dw_settings_page()
{
  return drupal_get_form('springboard_dw_settings_form');
}

/**
 * Configuration settings form for Springboard data warehousing.
 */
function springboard_dw_settings_form()
{
  $form = array();

  $form['springboard_dw_api_endpoint'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Data warehouse server url: (I.E https://dw.gospringboard.io)'),
    '#default_value' => variable_get('springboard_dw_api_endpoint', NULL),
    '#required'      => true
  );

  $form['springboard_dw_authentication_token'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Data warehouse authentication token'),
    '#default_value' => variable_get('springboard_dw_authentication_token', NULL),
    '#required'      => true
  );

  $form['springboard_dw_watchdog_enabled'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable watchdog logging of queue item processing'),
    '#default_value' => variable_get('springboard_dw_watchdog_enabled', 1)
  );

  return system_settings_form($form);
}

/**
 * Page callback for the Springboard data warehouse reset form.
 */

function springboard_dw_admin_warehouse_reset_page()
{
  return drupal_get_form('springboard_dw_admin_warehouse_reset_form');
}

/**
 * Configuration settings form for Springboard data warehousing.
 */
function springboard_dw_admin_warehouse_reset_form()
{
  $form = array();

  $form['springboard_dw_reset_verify'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Are you really sure you want to reset the data warehouse tracking and warehouse queue? Use with caution, this cannot be undone.'),
    '#default_value' => 0
  );

  $form['springboard_dw_reset_submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit')
  );

  return $form;
}

/**
 * Validation method to make sure verification is made before deleting pending queue items and truncating warehouse table.
 * Implements hook_form_validate()
 * @param $form
 * @param $form_state
 */
function springboard_dw_admin_warehouse_reset_form_validate(&$form, &$form_state)
{
  if (!$form_state['values']['springboard_dw_reset_verify']) {
    form_set_error('name', 'You must verify you want to reset the existing data warehouse tracking and warehouse queue.');
  }
}

/**
 * Form submission handler for resetting the Springboard data warehouse queue and tracking table.
 * Implements hook_form_submit()
 * @param $form
 * @param $form_state
 */
function springboard_dw_admin_warehouse_reset_form_submit(&$form, &$form_state)
{
  // Empty out the queue table for Springboard data warehouse items
  db_query("delete from {queue} where name = :name", array(':name' => 'springboard_dw_export'));

  // Delete all the tracking from the Springboard data warehouse table
  db_query("delete from {springboard_data_warehouse}");

  drupal_set_message('The data warehouse tracking has been reset and the warehouse queue emptied.');
}
