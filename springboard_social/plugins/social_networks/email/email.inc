<?php

$plugin = array(
  'name' => 'email',
  'title' => t('Email'),
  'settings' => 'email_settings',
  'admin_settings' => 'email_admin_settings',
  'process' => 'email_process_settings',
  'share_config' => 'email_share_js_config',
  'js' => 'plugins/social_networks/email/email.share.js',
  'defaults' => 'email_defaults',
  'button_extra' => 'email_button_extra',
  'uninstall' => 'email_uninstall',
);

// TODO: add uninstall hook to clear variables.

function email_defaults() {
  $settings = array(
    'subject' => variable_get('springboard_social_email_subject', '%title'),
    'message' => variable_get('springboard_social_email_message'. ''),
  );
  return $settings;
}

function email_settings(&$form, $enabled_services = array(), $settings = array(), $token_set = array('all')) {
  $form['email_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email Settings'),
    '#access' => in_array('email', $enabled_services),
    '#collapsible' => TRUE,
  );
  $form['email_settings']['email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email subject'),
    '#default_value' => isset($settings['subject']) ? $settings['subject'] : '',
  );
  $form['email_settings']['email_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Email message'),
    '#default_value' => isset($settings['message']) ? $settings['message'] : '',
  );
  $form['email_settings']['tokens']= array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['email_settings']['tokens']['token_help'] = array(
    '#type' => 'item',
    '#title' => t('Drupal tokens'),
    '#description' => theme('token_tree', array('token_types' => $token_set, 'recursion_limit' => 2, 'click_insert' => FALSE)),
  );
}

function email_admin_settings(&$form) {
  $form['services']['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email Settings'),
    '#states' => array(
      // Hide the settings when facebook checkbox is not selected.
      'invisible' => array(
       ':input[name="springboard_social_services[email]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['services']['email']['springboard_social_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email subject'),
    '#default_value' => variable_get('springboard_social_email_subject', '%title'),
  );
  $form['services']['email']['springboard_social_email_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Email message'),
    '#default_value' => variable_get('springboard_social_email_message', ''),
  );
}

function email_process_settings(&$data, $form_state) {
  $data['email'] = array(
    'subject' => !empty($form_state['values']['email_subject']) ? $form_state['values']['email_subject'] : '',
    'message' => !empty($form_state['values']['email_message']) ? $form_state['values']['email_message'] : '',
  );
}

function email_share_js_config($settings) {
  $settings = array(
    'email_subject' => $settings['data']['email']['subject'],
    'email_message' => $settings['data']['email']['message'],
  );
  drupal_add_js(array('sb_social' => $settings), 'setting');  
}

function email_uninstall() {
  variable_del('springboard_social_email_subject');
  variable_del('springboard_social_email_message');
}

// undocumented little hack to set the email subject to a value we control.
// by setting the title attribute in the link we override AddThis' default behavior which
// is pulling in the <title></title> contents as the email subject.
function email_button_extra($settings) {
  return 'addthis:title="' . $settings['subject'] . '"';
}
