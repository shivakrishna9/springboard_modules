<?php

/**
 * @file
 * Admin functions for Springboard advocacy.
 */

/**
 * Page callback for the admin form.
 */
function springboard_advocacy_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Springboard advocacy.') . '</p>',
      '#prefix' => '<div class="springboard-advocacy-settings-page">',
      '#suffix' => '</div>',
    ),
    'salesforce_sync_admin_form' => drupal_get_form('springboard_advocacy_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard advocacy.
 */
function springboard_advocacy_settings_form() {
  $form = array();

  $form['springboard_advocacy_server_url'] = array(
    '#title' => t('Springboard advocacy server url'),
    '#description' => t('The url of the Springboard advocacy server.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('springboard_advocacy_server_url', ''),
  );

  $form['transaction']['springboard_advocacy_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client ID.'),
    '#description' => t('Client ID for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_id', ''),
  );

  $form['transaction']['springboard_advocacy_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client Secret.'),
    '#description' => t('Client Secret for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_secret', ''),
  );

   $form['geo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Geocoding configuration'),
  );

  //we may need this commented section if we offer a melissa option

  // $options = array(
  //   //'smartystreets' => 'SmartyStreets Javascript API (Pay, API key is required)',
  //   'smartystreets_rest' => 'SmartyStreets REST API (Pay, API key is required)',
  // );

  // $form['geo']['springboard_advocacy_geocoder'] = array(
  //   '#type' => 'radios',
  //   '#title' => t('Active Geocoding service'),
  //   '#options' => $options,
  //   '#description' => t('Choose the geocoding service to use. All but Google Javascript API require an API Key'),
  //   '#default_value' => variable_get('springboard_advocacy_geocoder', ''),
  // );
  
  $form['geo']['springboard_advocacy_geocoder'] = array(
  '#type' => 'hidden',
  '#value' => 'smartystreets_rest',
  );

  $form['geo']['smarty'] = array(
    '#type' => 'fieldset',
    '#title' => t('Smarty Streets configuration'),
  );

  $form['geo']['smarty']['springboard_advocacy_smarty_authid'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth ID'),
    '#description' => t('Required for the REST API'),
    '#default_value' => variable_get('springboard_advocacy_smarty_authid', ''),
  );

  $form['geo']['smarty']['springboard_advocacy_smarty_authtoken'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth Token'),
    '#description' => t('Required for the REST AP'),
    '#default_value' => variable_get('springboard_advocacy_smarty_authtoken', ''),
  );

  // $form['geo']['smarty']['springboard_advocacy_smarty_htmltoken'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Smarty Streets HTML token'),
  //   '#description' => t('Required for the Javascript API only'),
  //   '#default_value' => variable_get('springboard_advocacy_smarty_htmltoken', ''),
  // );

  return system_settings_form($form);


  return system_settings_form($form);
}
