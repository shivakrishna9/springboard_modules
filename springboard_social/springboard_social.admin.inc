<?php

/**
 * @file
 * Include file for the admin configuration UI
 */

/**
 * Admin settings form.
 */
function springboard_social_admin_settings() {
  $form = array();
  $form['facebook_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['facebook_settings']['springboard_social_share_display'] = array(
    '#type' => 'select',
    '#title' => t('Facebook Share Links'),
    '#options' => array(
      '0' => t('Disable Facebook share links'),
      '1' => t('Enable on all donation forms'),
      '2' => t('Enable on a per form basis'),
    ),
    '#default_value' => variable_get('springboard_social_share_display', 0),
  );

  $form['facebook_settings']['springboard_social_app_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook App ID'),
    '#description' => t('A Facebook app ID is required for share links to work.'),
    '#default_value' => variable_get('springboard_social_app_id', ''),
  );
  $form['facebook_settings']['springboard_social_logo'] = array(
    '#type' => 'textfield',
    '#title' => t('Default logo'),
    '#description' => t('URL of the logo file to use on share popups.'),
    '#default_value' => variable_get('springboard_social_logo', ''),
  );
  $form['facebook_settings']['springboard_social_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Default title'),
    '#description' => t(''),
    '#default_value' => variable_get('springboard_social_title', ''),
  );
  $form['facebook_settings']['springboard_social_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Default description'),
    '#description' => t(''),
    '#default_value' => variable_get('springboard_social_description', ''),
  );

  $form['google_plus_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Google Plus Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['google_plus_settings']['springboard_social_gplus_display'] = array(
    '#type' => 'select',
    '#title' => t('Google Plus links'),
    '#options' => array(
      '0' => t('Disable Google+ links'),
      '1' => t('Enable on all donation forms'),
      '2' => t('Enable on a per form basis'),
    ),
    '#default_value' => variable_get('springboard_social_gplus_display', 0),
  );

  $button_sizes = array(
    'small' => t('Small'),
    'medium' => t('Medium'),
    'standard' => t('Standard'),
    'tall' => t('Tall'),
  );

  $form['google_plus_settings']['springboard_social_gplus_button_size'] = array(
    '#type' => 'select',
    '#title' => t('Plus button size'),
    '#description' => t('Select the default button size for your site.'),
    '#options' => $button_sizes,
    '#default_value' => variable_get('springboard_social_gplus_button_size', ''),
  );
  $form['google_plus_settings']['springboard_social_gplus_annotation'] = array(
    '#type' => 'radios',
    '#title' => t('Annotation'),
    '#description' => t('If annotation is enabled, all +1 buttons will also display the number of times this page has been shared.'),
    '#options' => array(
      'none' => t('None'),
      'bubble' => t('Bubble'),
    ),
    '#default_value' => variable_get('springboard_social_gplus_annotation', 'none'),
  );
  $form['google_plus_settings']['springboard_social_gplus_image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#description' => t(''),
    '#default_value' => variable_get('springboard_social_gplus_image_url', ''),
  );
  $form['google_plus_settings']['springboard_social_gplus_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t(''),
    '#default_value' => variable_get('springboard_social_gplus_description'),
  );
  return system_settings_form($form);
}
