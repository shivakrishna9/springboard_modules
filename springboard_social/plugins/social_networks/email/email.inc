<?php

$plugin = array(
  'name' => 'email',
  'title' => t('Email'),
  'settings' => 'email_settings',
  'process' => 'email_process_settings',
  'share_config' => 'email_share_js_config',
  'js' => 'plugins/social_networks/email/email.share.js',
);


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
