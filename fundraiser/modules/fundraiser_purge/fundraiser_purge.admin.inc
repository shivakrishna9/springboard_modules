<?php

/**
 * @file
 *
 * Admin functions for the fundraiser purge module.
 */

/**
 * Fundraiser purge settings form.
 */
function fundraiser_purge_settings_form() {
  $form['fundraiser_purge_donation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Donation purge settings'),
    '#description' => t('These settings will be used to determine the purging of old donation orders. The total number of orders that will be purged is the number of orders per loop times the numebr of loops. So if you set to clear 10 orders per loop and loop 5 times, then you will clear 50 orders on each cron run. Please note that we want to keep this to a reasonable level as cron runs often.'),
  );
  $year_options = array(0 => 'Purge disabled');
  for ($years = 0.5; $years < 3.5; $years += 0.5) {
    $days = $years * 365;
    $year_options[$days] = "$years years";
  }
  $form['fundraiser_purge_donation']['fundraiser_purge_age'] = array(
    '#type' => 'select',
    '#title' => t('Storage age'),
    '#default_value' => variable_get('fundraiser_purge_age', 0),
    '#options' => $year_options,
    '#description' => t('Enter the number of years that posted donations should remain in the system. Posted donations older then the number of years configured here will be permanently deleted from the system. Enter "Disable" (0) for never purge.'),
  );
  $limit_options = array(5 => '5 orders per loop');
  for ($i=10; $i < 1001; $i+=10) {
    $limit_options[$i] = "$i orders per loop";
  }
  $form['fundraiser_purge_donation']['fundraiser_purge_limit'] = array(
    '#type' => 'select',
    '#title' => t('Processing limit'),
    '#default_value' => variable_get('fundraiser_purge_limit', 25),
    '#options' => $limit_options,
    '#description' => t('Enter the number of orders to process at a time. The system can process about 10 orders per second.'),
  );
  $loop_options = array();
  for ($i=1; $i < 11; $i++) {
    $loop_options[$i] = "$i loops per cron run";
  }
  $form['fundraiser_purge_donation']['fundraiser_purge_loop'] = array(
    '#type' => 'select',
    '#title' => t('Batch loop'),
    '#default_value' => variable_get('fundraiser_purge_loop', 5),
    '#options' => $loop_options,
    '#description' => t('Enter the number of the number of times you want the batch loop to process. Each loop will process the number of orders set in the limit above.'),
  );

  $form['fundraiser_purge_webform'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webform purge settings'),
    '#description' => t('Select which types of webforms should have their submissions purged also. This will apply the same rules as above, and will occur at the same time. You may need to adjust the limit/loop values if you change this.'),
  );
  $form['fundraiser_purge_webform']['fundraiser_purge_webform_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Webform typess'),
    '#default_value' => variable_get('fundraiser_purge_webform_types', array()),
    '#options' => _fundraiser_purge_settings_types_list(),
    '#description' => t('Select which webform content types to purge old submissions from.'),
  );

  $form['fundraiser_purge_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#default_value' => variable_get('fundraiser_purge_debug', 0),
    '#description' => t('This will allow the purge process to be run, but without actually deleting anything. It will report the purge counts in the log.'),
  );

  return system_settings_form($form);
}

/**
 * Validation function for purge settings form.
 *
 * Ensure purge age is a positive integer.
 */
function fundraiser_purge_settings_form_validate($form, &$form_state) {
  $purge_age = $form_state['values']['fundraiser_purge_age'];

  if (!is_numeric($purge_age) || !((int)($purge_age) == $purge_age) || (int)$purge_age < 0) {
    form_set_error('fundraiser_purge_age', 'Purge age must be a positive integer');
  }
}

/**
 * Helper function to generate the list of webform types
 */
function _fundraiser_purge_settings_types_list() {
  $options = array();
  $donation_type = 'donation_form';
  $webform_types = webform_variable_get('webform_node_types');
  if(($key = array_search($donation_type, $webform_types)) !== false) {
    unset($webform_types[$key]);
  }
  foreach ($webform_types as $type) {
    $options[$type] = ucfirst(str_replace('_', ' ', $type));
  }
  return $options;
}
