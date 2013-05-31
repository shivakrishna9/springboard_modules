<?php

/**
 * Admin settings
 */
function sb_social_admin() {
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

  return system_settings_form($form);
}


/**
 * Node level settings
 */
function sb_social_webform_settings($form, $edit, $node) {

  $enabled_services = variable_get('springboard_social_services', array());
  $settings = sb_social_webform_settings_load($node->nid);


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
  // TODO: implement file upload bs.

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
   $enabled_services = variable_get('springboard_social_services', array());
   foreach ($enabled_services as $service) {
     $func = 'sb_social_process_settings_' . $service;
     $func($form_state);
   }
}

function sb_social_process_settings_facebook($form_state) {

  $data = array(
    'title' => !empty($form_state['values']['fb_title']) ? $form_state['values']['fb_title'] : '',
    'description' => !empty($form_state['values']['fb_description']) ? $form_state['values']['fb_description'] : '',
    'image' => '', // TODO: integrate image field.
  );
  $record = array(
    'nid' => $form_state['values']['nid'],
    'service' => 'facebook',
    'data' => $data,
  );

  $update = $form_state['values']['update_facebook'] ? array('nid', 'service') : array();


  $validators = array();
  if (!empty($form_state['values']['fb_image'])) {
    // apparently the form api only passes the fid when a file has been uploaded
    // so we have to load the full record and fish out the uri to generate
    // a usable image url.
    $file = file_load($form_state['values']['fb_image']);
    $record['data']['image'] = file_create_url($file->uri);
  }
  sb_social_webform_settings_save($record, $update);
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
  $update = $form_state['values']['update_twitter'] ? array('nid', 'service') : array();
  sb_social_webform_settings_save($record, $update);
}
