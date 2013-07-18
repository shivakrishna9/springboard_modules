<?php

/**
 * Admin settings
 */
function sb_social_admin() {
  // TODO: abstract services. Some kind of plugin architecture would be optimal.
  $services = array(
    'facebook' => t('Facebook'),
    'twitter' => t('Twitter'),
  );
  $form['springboard_social_services'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled services'),
    '#options' => $services,
    '#default_value' => variable_get('springboard_social_services', array()),
  );
  $form['springboard_social_addthis_profile_id'] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis Profile ID'),
    '#default_value' => variable_get('springboard_social_addthis_profile_id', ''),
  );
  $form['services']['facebook'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook Settings'),
    '#states' => array(
      // Hide the settings when facebook checkbox is not selected.
      'invisible' => array(
       ':input[name="springboard_social_services[facebook]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['services']['facebook']['springboard_social_facebook_title'] = array(
    '#type' => 'textfield',
    '#title'=> t('Default Facebook share title'),
    '#default_value' => variable_get('springboard_social_facebook_title', '%title'),
  );
  $form['services']['facebook']['springboard_social_facebook_description'] = array(
    '#type' => 'textfield',
    '#title'=> t('Default Facebook share description'),
    '#default_value' => variable_get('springboard_social_facebook_description', '%teaser'),
  );
  $form['services']['facebook']['springboard_social_facebook_image'] = array(
    '#type' => 'managed_file',
    '#title' => 'Image upload',
    '#description' => t('Upload an image to display in share content'),
    '#default_value' => variable_get('logo', ''),
    '#upload_location' => 'public://social_images',
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
    '#theme' => 'sb_social_share_image_preview',
  );

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

  if (module_exists('market_source')) {
    $form['services']['market_source'] = array(
        '#type' => 'fieldset',
        '#title' => t('Market Source Settings'),
        '#description' => t('Optional Market Source values to include in share links'),
    );
    $fields = market_source_field_info();
    // It doesn't make sense to list certain market source fields, like user agent.
    // the blacklist lets us filter these out cleanly.
    $blacklist = _sb_social_component_blacklist();
    foreach ($fields as $key => $field) {
      if (in_array($key, $blacklist)) {
        continue;
      }
      // special handling for campaign ID
      if ($key == 'cid') {
        $campaigns = market_source_get_campaigns(FALSE);
        $select_options = array();
        foreach ($campaigns as $campaign_id => $campaign) {
          $select_options[$campaign_id] = $campaign['name'] . ' (' . $campaign_id . ')';
        }
    
        $form['services']['market_source']['springoard_social_ms_cid'] = array(
          '#type' => 'select',
          '#title' => t('Campaign ID'),
          '#options' => $select_options,
          '#default_value' => variable_get('springboard_social_ms_cid', ''),
        );
      }
      else {
        $form['services']['market_source']['springboard_social_ms_' . $key] = array(
          '#type' => 'textfield',
          '#title' => $field['#title'],
          '#default_value' => variable_get('springboard_social_ms_' . $key, ''),
        );
      }
    }
  }
  $form['#submit'][] = 'sb_social_admin_settings_submit';
  return system_settings_form($form);
}

function sb_social_admin_settings_submit($form, $form_state) {
  cache_clear_all('*', 'cache_token', TRUE);
  drupal_static_reset('token_get_info');
  drupal_static_reset('token_get_global_token_types');
  drupal_static_reset('token_get_entity_mapping');
  drupal_static_reset('token_build_tree');
  drupal_static_reset('_token_profile_fields');
  drupal_static_reset('token_menu_link_load');
  drupal_static_reset('token_book_link_load');
  drupal_static_reset('token_node_menu_link_load');
  drupal_static_reset('_token_field_info');
}

/**
 * Node level settings
 */
function sb_social_webform_settings($form, $edit, $node) {

  $enabled_services = sb_social_get_enabled_services();
  $settings = sb_social_webform_settings_load($node->nid);
  // setup for token replacement
  $token_set = array('user', 'node');
  if (fundraiser_is_donation_type($node->type)) {
    $token_set[] = 'donation';
  }
  if (empty($enabled_services)) {
    $form['banner'] = array(
      '#prefix' => '<h2>' . t('No services enabled') . '</h2>',
      '#markup' => '<p>' . t('No social media services have been enabled. To enable Social Media services, visit the !admin_screen', array('!admin_screen' => l('module administration page', 'admin/config/services/springboard_social'))) . '</p>',
    );
  }
  // no previous settings available? save global defaults
  if (empty($settings)) {
    sb_social_add_default_settings($node->nid);
    $settings = sb_social_webform_settings_load($node->nid);
  }

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  foreach ($enabled_services as $service) {
    $form['update_' . $service] = array(
      '#type' => 'value',
      '#value' => !empty($settings[$service]),
    );
  }
  $form['fb_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook Settings'),
    '#access' => in_array('facebook', $enabled_services),
  );
  // title
  $form['fb_settings']['fb_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Facebook Share title'),
    '#default_value' => isset($settings['facebook']['title']) ? $settings['facebook']['title'] : '',
  );

  // description
   $form['fb_settings']['fb_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Facebook Share description'),
    '#default_value' => isset($settings['facebook']['description']) ? $settings['facebook']['description'] : '',
  );
  // image
  $form['fb_settings']['fb_image'] = array(
    '#type' => 'managed_file',
    '#title' => 'Image upload',
    '#description' => t('Upload a custom image to display in share content'),
    '#default_value' => variable_get('logo', ''),
    '#upload_location' => 'public://social_images',
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
    ),
  );
  $form['fb_settings']['tokens']= array(
    '#type' => 'fieldset',
    '#title' => t('Available Tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fb_settings']['tokens']['token_help'] = array(
    '#type' => 'item',
    '#title' => t('Drupal tokens'),
    '#description' => theme('token_tree', array('token_types' => $token_set, 'recursion_limit' => 2, 'click_insert' => FALSE)),
  );



  // TODO: implement image preview.

  $form['twitter_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Twitter Settings'),
    '#access' => in_array('twitter', $enabled_services),
  );
  // message
  $form['twitter_settings']['twitter_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Message'),
    '#description' => t('Custom twitter message text.'),
    '#default_value' => isset($settings['twitter']['message']) ? $settings['twitter']['message'] : '',
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

  // Market Source
  if (module_exists('market_source')) {

    $form['market_source'] = array(
      '#type' => 'fieldset',
      '#title' => t('Market Source Settings'),
      '#description' => t('Optional Market Source values to include in share links'),
    );
    $fields = market_source_field_info();
    // It doesn't make sense to list certain market source fields, like user agent.
    // the blacklist lets us filter these out cleanly.
    $blacklist = _sb_social_component_blacklist();
    foreach ($fields as $key => $field) {
      if (in_array($key, $blacklist)) {
        continue;
      }
      if ($key == 'cid') {
        $campaigns = market_source_get_campaigns(FALSE);
        $select_options = array();
        foreach ($campaigns as $campaign_id => $campaign) {
          $select_options[$campaign_id] = $campaign['name'] . ' (' . $campaign_id . ')';
        }
        $form['market_source']['cid'] = array(
          '#type' => 'select',
          '#title' => t('Campaign ID'),
          '#options' => $select_options,
          '#default_value' => (!empty($settings) && !empty($service) && !empty($settings[$service]['market_source'][$key])) ? $settings[$service]['market_source'][$key] : '',
        );
      }
      else {
        $form['market_source'][$key] = array(
          '#type' => 'textfield',
          '#title' => $field['#title'],
          '#default_value' => (!empty($settings) && !empty($service) && !empty($settings[$service]['market_source'][$key])) ? $settings[$service]['market_source'][$key] : '',
        );
      }
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#validate'][] = 'sb_social_webform_settings_validate';
  $form['#submit'][] = 'sb_social_webform_settings_submit';
  return $form;
}

function sb_social_webform_settings_validate(&$form, $form_state) {

}

function sb_social_webform_settings_submit($form, $form_state) {
  $enabled_services = sb_social_get_enabled_services();
  if (is_array($enabled_services)) {
    foreach ($enabled_services as $service) {
      $func = 'sb_social_process_settings_' . $service;
      $func($form_state);
    }
  }
}

function sb_social_process_settings_facebook($form_state) {
  $data = array(
    'title' => !empty($form_state['values']['fb_title']) ? $form_state['values']['fb_title'] : '',
    'description' => !empty($form_state['values']['fb_description']) ? $form_state['values']['fb_description'] : '',
    'image' => '',
  );
  $record = array(
    'nid' => $form_state['values']['nid'],
    'service' => 'facebook',
    'data' => $data,
  );

  if (!empty($form_state['values']['fb_image'])) {
    // apparently the form api only passes the fid when a file has been uploaded
    // so we have to load the full record and fish out the uri to generate
    // a usable image url.
    $file = file_load($form_state['values']['fb_image']);
    $record['data']['image'] = file_create_url($file->uri);
  }
  sb_social_webform_settings_save($record, $form_state['values']);
}

function sb_social_process_settings_twitter($form_state) {
  $data = array(
    'message' => !empty($form_state['values']['twitter_message']) ? $form_state['values']['twitter_message'] : '',
  );
  $record = array(
    'nid' => $form_state['values']['nid'],
    'service' => 'twitter',
    'data' => $data,
  );
  sb_social_webform_settings_save($record, $form_state['values']);
}
