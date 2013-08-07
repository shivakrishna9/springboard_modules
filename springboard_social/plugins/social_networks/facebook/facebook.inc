<?php

$plugin = array(
  'name' => 'facebook',
  'title' => 'Facebook',
  'settings' => 'facebook_settings',
  'process' => 'facebook_process_settings',
);


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