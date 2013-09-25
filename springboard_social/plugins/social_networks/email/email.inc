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
  'token_info' => email_token_info(),
  'token_replace' => 'email_token_replace',
);


function email_token_info() {
  $email_tokens = array();
  $email_tokens['email_share_link'] = array(
    'name' => t('Email share link'),
    'description' => t('AddThis Email link'),
  );
  $email_tokens['email_subject'] = array(
    'name' => t('Email subject'),
    'description' => t('Share email subject'),
  );
  $email_tokens['email_message'] = array(
    'name' => t('Email message'),
    'description' => t('Share email message'),
  );
  $email_tokens['email_share_url'] = array(
    'name' => t('Email share URL'),
    'description' => t('Short URL containing share tracking values for Email.'),
  );


  return $email_tokens;
}

function email_token_replace($tokens, &$replacements, $settings, $node = FALSE, $submission_id = FALSE, $extra = FALSE) {
  foreach ($tokens as $key => $token) {
    switch ($key) {
      case 'email_share_link':
        $vars['service'] = 'email';
        $vars['url'] = sb_social_short_url('twitter', $settings, $submission_id);
        $vars['extra'] = '';

        if ($extra && function_exists($extra)) {
          $vars['extra'] = $extra($settings['data']['email'], $node, $submission_id);
        }
        $button = theme('sb_social_share_button', $vars);
        $replacements[$token] = theme('sb_social_share', array('buttons' => $button));
        break;
      case 'email_subject':
        $replacements[$token] = !empty($settings['data']['email']['subject']) ? _sb_social_replace_tokens($settings['data']['email']['subject'], $node, $submission_id) : '';
        break;
      case 'email_message':
        $replacements[$token] = !empty($settings['data']['email']['message']) ? _sb_social_replace_tokens($settings['data']['email']['message'], $node, $submission_id) : '';
        break;
      case 'email_share_url':
        $replacements[$token] = sb_social_short_url('email', $settings, $submission_id);
        break;
    }
  }
}

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

function email_share_js_config($settings, $node = FALSE, $sid = FALSE) {
  $settings = array(
    'email_subject' => _sb_social_replace_tokens($settings['data']['email']['subject'], $node, $sid),
    'email_message' => _sb_social_replace_tokens($settings['data']['email']['message'], $node, $sid),
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
function email_button_extra($settings, $node = FALSE, $submission_id = FALSE) {
  return 'addthis:title="' . _sb_social_replace_tokens($settings['subject'], $node, $submission_id) . '"';
}
