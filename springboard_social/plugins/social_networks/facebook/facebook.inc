<?php

$plugin = array(
  'name' => 'facebook', // internal service name
  'title' => t('Facebook'), // Human-readable service name
  'settings' => 'facebook_settings', // additions to path and node settings forms
  'admin_settings' => 'facebook_admin_settings', // additions to admin defaults settings form
  'process' => 'facebook_process_settings', // submit handler for node & path settings fields
  'share_config' => 'facebook_share_opengraph_config', // render share settings required by AddThis
  'js' => FALSE, // include js settings as needed
  'defaults' => 'facebook_defaults', // provide default settings
  'uninstall' => 'facebook_uninstall', // uninstall hook: remove variables, etc.
);

function facebook_defaults() {
  $settings = array(
    'title' => variable_get('springboard_social_facebook_title', '%title'),
    'description' => variable_get('springboard_social_facebook_description', '%teaser'),
    'image' => '', variable_get('springboard_social_facebook_image', ''),
  );
  return $settings;
}

function facebook_admin_settings(&$form) {
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
}

function facebook_settings(&$form, $enabled_services = array(), $settings = array(), $token_set = array('all')) {
  $form['fb_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook Settings'),
    '#access' => in_array('facebook', $enabled_services),
    '#collapsible' => TRUE,
  );
  // title
  $form['fb_settings']['fb_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Facebook Share title'),
    '#default_value' => isset($settings['title']) ? $settings['title'] : '',
  );
  // description
   $form['fb_settings']['fb_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Facebook Share description'),
    '#default_value' => isset($settings['description']) ? $settings['description'] : '',
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
}

function facebook_process_settings(&$data, $form_state) {
  $data['facebook'] = array(
      'title' => !empty($form_state['values']['fb_title']) ? $form_state['values']['fb_title'] : '',
      'description' => !empty($form_state['values']['fb_description']) ? $form_state['values']['fb_description'] : '',
      'image' => '',
  );

  if (!empty($form_state['values']['fb_image'])) {
    // apparently the form api only passes the fid when a file has been uploaded
    // so we have to load the full record and fish out the uri to generate
    // a usable image url.
    $file = file_load($form_state['values']['fb_image']);
    $data['image'] = file_create_url($file->uri);
  }
}

/**
 * Configure AddThis share buttons for Facebook.
 *
 * Add OpenGraph meta tags used by Facebook to define custom title, description, and image content on
 * shares.
 *
 * Facebook and a few other services use opengraph tags to define custom share content instead of the 
 * usual AddThis javascript configuration objects.
 * 
 */ 
function facebook_share_opengraph_config($settings, $node = FALSE, $sid = FALSE) {
  $facebook = $settings['data']['facebook'];
  $meta = array(
    'title' => !empty($facebook['title']) ? $facebook['title'] : '',
    'description' => !empty($facebook['description']) ? $facebook['description'] : '',
    'image' => $facebook['image'],
     //'url' => sb_social_short_url($node, $sid, 'facebook'),
  );
  
  foreach ($meta as $field => $value) {
    $meta[$field] = _sb_social_replace_tokens($value, $node, $sid);	
  }
  
  foreach ($meta as $id => $content) {
     $data = array(
      '#type' => 'html_tag',
      '#tag' => 'meta',
      '#attributes' => array(
        'property' =>  "og:$id",
        'content' => $content,
      ),
    );
    drupal_add_html_head($data, 'opengraph_' . $id);
  }
}

function facebook_uninstall() {
  // remove variables.
  variable_del('springboard_social_facebook_title');
  variable_del('springboard_social_facebook_description');
  variable_del('springboard_social_facebook_image');

}
