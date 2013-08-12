<?php

$plugin = array(
  'name' => 'Twitter',
  'title' => t('Facebook'),
  'settings' => 'twitter_settings',
  'process' => 'twitter_process_settings',
  'share_config' => 'twitter_share_js_config',
  'js' => 'plugins/social_networks/twitter/twitter.share.js',
);


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

function twitter_process_settings(&$data, $form_state) {
  $data['twitter'] = array(
    'message' => !empty($form_state['values']['twitter_message']) ? $form_state['values']['twitter_message'] : '',
  );
}

function twitter_share_js_config($settings) {
  $settings = array(
    'twitter_message' => $settings['data']['twitter']['message'],
  );
  drupal_add_js(array('sb_social' => $settings), 'setting');
}
