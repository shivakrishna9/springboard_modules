<?php

$plugin = array(
  'name' => 'Twitter',
  'title' => t('Facebook'),
  'settings' => 'twitter_settings',
  'admin_settings' => 'twitter_admin_settings',
  'process' => 'twitter_process_settings',
  'share_config' => 'twitter_share_js_config',
  'js' => 'plugins/social_networks/twitter/twitter.share.js',
  'defaults' => 'twitter_defaults',
  'uninstall' => 'twitter_uninstall',
  'token_info' => twitter_token_info(),
  'token_replace' => 'twitter_token_replace',
);

function twitter_token_info() {
  $twitter_tokens['twitter_share_link'] = array(
    'name' => t('Twitter Share link'),
    'description' => t('AddThis Twitter tweet link'),
  );
  // twitter message
  $twitter_tokens['twitter_message'] = array(
    'name' => t('Twitter tweet message'),
    'description' => t('Message content for Twitter tweet'),
  );
  $twitter_tokens['twitter_share_url'] = array(
    'name' => t('Twitter share URL'),
    'description' => t('Short URL containing share tracking values for Twitter.'),
  );
  return $twitter_tokens;
}

function twitter_token_replace($tokens, &$replacements, $settings, $node = FALSE, $submission_id = FALSE, $extra = FALSE) {
  foreach ($tokens as $key => $token) {
    switch ($key) {
      case 'twitter_share':
        $vars['service'] = $service;
        $vars['url'] = sb_social_short_url('twitter', $settings, $submission_id);
        $vars['extra'] = '';

        if ($extra && function_exists($extra)) {
          $vars['extra'] = $extra();
        }
        $button = theme('sb_social_share_button', $vars);
        $replacements[$token] = theme('sb_social_share', array('buttons' => $button);
        break;
      case 'twitter_message':
        $replacements[$token] =  !empty($settings['data']['twitter']['message']) ? _sb_social_replace_tokens($settings['data']['twitter']['message'], $node, $submission_id) : '';
        break;
      case 'twitter_share_url':
        $replacements[$token] = sb_social_short_url('facebook', $settings, $submission_id);
        break;

    }
  }
}

// TODO: add uninstall function to clear variables.

function twitter_defaults() {
  $settings = array(
    'message' => variable_get('springboard_social_twitter_message', '%title'),
  );
  return $settings;
}

function twitter_settings(&$form, $enabled_services = array(), $settings = array(), $token_set = array('all')) {
  $form['twitter_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Twitter Settings'),
    '#access' => in_array('twitter', $enabled_services),
    '#collapsible' => TRUE,
  );
  // message
  $form['twitter_settings']['twitter_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Message'),
    '#description' => t('Custom twitter message text.'),
    '#default_value' => isset($settings['message']) ? $settings['message'] : '',
  );
  $form['twitter_settings']['tokens']= array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['twitter_settings']['tokens']['token_help'] = array(
    '#type' => 'item',
    '#title' => t('Drupal tokens'),
    '#description' => theme('token_tree', array('token_types' => $token_set, 'recursion_limit' => 2, 'click_insert' => FALSE)),
  );
}

function twitter_admin_settings(&$form) {
  $form['services']['twitter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Twitter'),
    '#states' => array(
      // Hide the settings when facebook checkbox is not selected.
      'invisible' => array(
       ':input[name="springboard_social_services[twitter]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['services']['twitter']['springboard_social_twitter_message'] = array(
    "#type" => 'textfield',
    '#title' => t('Default twitter share message'),
    '#default_value' => variable_get('springboard_social_twitter_message', '%title'),
  );
}

function twitter_process_settings(&$data, $form_state) {
  $data['twitter'] = array(
    'message' => !empty($form_state['values']['twitter_message']) ? $form_state['values']['twitter_message'] : '',
  );
}

function twitter_share_js_config($settings, $node = FALSE, $sid = FALSE) {
  $settings = array(
    'twitter_message' => _sb_social_replace_tokens($settings['data']['twitter']['message'], $node, $sid),
  );
  drupal_add_js(array('sb_social' => $settings), 'setting');
}

function twitter_uninstall() {
  variable_del('springboard_social_twitter_message');
}
