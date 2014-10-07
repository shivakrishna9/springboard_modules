<?php

/**
 * @file Admin settings for cleaning up the webform_confirmations_submissions table during cron.
 */

/**
 * Admin settings form.
 */
function webform_confirmations_admin_settings() {
  $form['webform_confirmations_submissions_purge_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge old submission records when cron is run.'),
    '#default_value' => variable_get('webform_confirmations_submissions_purge_enabled', FALSE),
  );
  $form['webform_confirmations_submissions_clear_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Clear out submission records after this many days'),
    '#size' => 4,
    '#default_value' => variable_get('webform_confirmations_submissions_clear_date', 90),
  );

  return system_settings_form($form);
}
