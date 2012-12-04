<?php

/**
 * @file
 * Capwiz administration pages
 *
 * @author Derek Dunagan <derek.dunagan@jacksonriver.com>
 */


/**
 * Define form: Capwiz settings form
 *
 * @ingroup forms
 */
function capwiz_settings_form(&$form_state) {

  $form = array();

  // Capwiz API account
  $client_id = variable_get('capwiz_client_id', '');
  $authuser  = variable_get('capwiz_authuser', '');
  $authpass  = variable_get('capwiz_authpass', '');

  $account_not_setup = empty($client_id) || empty($authuser) || empty($authpass);

  $form['capwiz_account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Capwiz API account'),
    '#access' => user_access('administer capwiz credentials'),
    '#collapsible' => !$account_not_setup,
    '#collapsed' => !$account_not_setup,
    'client_id' => array(
      '#type' => 'textfield',
      '#title' => t('Capwiz ID'),
      '#default_value' => $client_id,
      '#required' => TRUE,
      '#description' => t('http://www.capwiz.com/<strong>capwizid</strong>/home')),
    'authuser' => array(
      '#type' => 'textfield',
      '#title' => t('Username'),
      '#default_value' => $authuser,
      '#required' => TRUE,
      '#description' => $credentials_helptext = t('For your API account (not your admin account)')),
    'authpass' => array(
      '#type' => 'textfield',
      '#title' => t('Password'),
      '#default_value' => $authpass,
      '#required' => TRUE,
      '#description' => $credentials_helptext),
  );

  // Field mapping
  $mapping = variable_get('capwiz_mapping', array());

  $form['mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field mapping'),
  );

  $custom_fields = variable_get('capwiz_custom_fields', array());

  module_load_include('inc', 'capwiz', 'capwiz.api');
  foreach (array_merge(CapwizApi::getAccountFields(), $custom_fields) as $capwiz_field) {
    $form['mapping'][$capwiz_field] = array(

      "~$capwiz_field~label" => array(
        '#value' => t(ucfirst(str_replace('_', ' ', $capwiz_field)))),

      "~$capwiz_field~profile_fid" => array(
        '#type' => 'select',
        '#title' => '',
        '#default_value' => $mapping[$capwiz_field]['profile_fid'],
        '#options' => array('' => '') + capwiz_get_profile_fields(TRUE)),

      "~$capwiz_field~overwrite_rule" => array(
        '#type' => 'select',
        '#title' => '',
        '#default_value' => $mapping[$capwiz_field]['overwrite_rule'],
        '#options' => array(
          ''                      => '',
          CAPWIZ_OVERWRITE_NEVER  => t('Never overwrite'),
          CAPWIZ_OVERWRITE_BLANK  => t('Overwrite when blank'),
          CAPWIZ_OVERWRITE_ALWAYS => t('Always overwrite'))),
    );
  }

  // Email notifications
  $email_message_options_id_title = capwiz_get_email_message_options('id', 'title');

  $email_message_description = 'The system\'s default new user email message is "<em>!message</em>."';
  $email_message_description = t($email_message_description, array('!message' => $email_message_options_id_title[variable_get('user_register', 1)]));
  if (user_access('administer users')) {
    $email_message_description .= ' ' . t('To review or edit the new user email messages, go to !link.',
      array('!link' => l('User settings', 'admin/user/settings', array('attributes' => array('target' => '_blank')))));
  }

  $form['capwiz_email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email notifications'),
    '#collapsible' => !$account_not_setup,
    '#collapsed' => !$account_not_setup,
    'email_message' => array(
      '#type' => 'select',
      '#title' => t('Email message for newly created Drupal users'),
      '#default_value' => variable_get('capwiz_email_message', 'register_none'), // Default to sending no email.
      '#description' => $email_message_description,
      '#options' => array('' => t('[ System default ]')) + capwiz_get_email_message_options('name', 'title')),
  );

  $form['capwiz_syncing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Syncing'),
    '#access' => user_access('administer capwiz advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => !$account_not_setup,
    'drupal_cron' => array(
      '#type' => 'checkbox',
      '#title' => t('Let Drupal do the syncing'),
      '#default_value' => variable_get('capwiz_cron', 1),
      '#description' => t('If this setting is off, you must create a !symlink in the Drupal root directory that points to !file. ' .
        'Then you must set up a !crontab to hit that file as often as you need Drupal to sync with Capwiz. If syncing is setup to ' .
        'run both by Drupal and an independent crontab, it will not break since they both run the same code, but it would be pretty ' .
        'pointless. Choose one method or the other.', array('!file' => '<em>' . drupal_get_path('module', 'capwiz') . '/</em>' .
        '<strong>capwiz.cron.php</strong>', '!symlink' => l('symlink', 'http://en.wikipedia.org/wiki/Symbolic_link',
        array('attributes' => array('target' => '_blank'))), '!crontab' => l('crontab', 'http://drupal.org/cron',
        array('attributes' => array('target' => '_blank')))))),
    'custom_fields' => array(
      '#type' => 'textarea',
      '#title' => t('Custom fields'),
      '#default_value' => implode("\n", $custom_fields),
      '#description' => t('Write one custom field name per line. Custom field names must be exact or they will not be able to sync up.')),
    'reset_api_timestamp' => array(
      '#type' => 'submit',
      '#value' => t('Reset Capwiz API'),
      '#suffix' => '<span class="description">&nbsp;&nbsp;' . t('Use when first syncing to pull all data from Capwiz.') . '</span>'),
  );

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}


/**
 * Submit callback: Capwiz settings form
 */
function capwiz_settings_form_submit($form, &$form_state) {

  $form_values = $form_state['values'];

  // Capwiz API account
  variable_set('capwiz_client_id', $form_values['client_id']);
  variable_set('capwiz_authuser',  $form_values['authuser']);
  variable_set('capwiz_authpass',  $form_values['authpass']);

  // Field mapping
  $mapping = array();
  foreach (array_filter($form_values) as $key => $value) {
    if (substr($key, 0, 1) == '~') {
      list(, $capwiz_field, $property) = explode('~', $key);
      $mapping[$capwiz_field][$property] = $value;
    }
  }
  variable_set('capwiz_mapping', $mapping);

  // Custom fields
  $custom_fields = explode("\n", trim($form_values['custom_fields']));
  variable_set('capwiz_custom_fields', array_map('trim', array_filter($custom_fields)));

  // Email notifications
  variable_set('capwiz_email_message', $form_values['email_message']);

  // Syncing
  variable_set('capwiz_cron', $form_values['drupal_cron']);

  // Let the user know the save was successful.
  drupal_set_message(t('The Capwiz settings have been saved.'));

  if ($form_state['clicked_button']['#parents'][0] == 'reset_api_timestamp') {

    require_once 'capwiz.api.inc';
    CapwizApi::resetApiTimestamp();

    drupal_set_message(t('The Capwiz API has been reset. All users and advocacy actions will be synced.'));
  }
}


/**
 * Theme callback: Capwiz settings form
 *
 * @ingroup themeable
 */
function theme_capwiz_settings_form($form) {

  $mapping = array();
  foreach ($form['mapping'] as $capwiz_field => $capwiz_field_settings) {
    if (substr($capwiz_field, 0, 1) != '#') {
      $mapping[] = array(
        drupal_render($form['mapping'][$capwiz_field]["~$capwiz_field~label"]),
        drupal_render($form['mapping'][$capwiz_field]["~$capwiz_field~profile_fid"]),
        drupal_render($form['mapping'][$capwiz_field]["~$capwiz_field~overwrite_rule"]),
      );
    }
  }
  $form['mapping']['table'] = array(
    '#type' => 'markup',
    '#value' => theme('table', array(t('Capwiz field'), t('Profile field'), t('Overwrite rule')), $mapping),
  );

  return drupal_render($form);
}

