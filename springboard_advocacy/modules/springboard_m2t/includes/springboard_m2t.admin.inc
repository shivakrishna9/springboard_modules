<?php

/**
 * @file
 * Admin functions for Springboard M2T.
 */

/**
 * System settings form
 */
function springboard_m2t_admin() {
  
  $form = array();

  $form['geo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Geocoding configuration'),
  );

  $options = array(
    //'smartystreets' => 'SmartyStreets Javascript API (Pay, API key is required)',
    'smartystreets_rest' => 'SmartyStreets REST API (Pay, API key is required)',
  );

  $form['geo']['springboard_m2t_geocoder'] = array(
    '#type' => 'radios',
    '#title' => t('Active Geocoding service'),
    '#options' => $options,
    '#description' => t('Choose the geocoding service to use. All but Google Javascript API require an API Key'),
    '#default_value' => variable_get('springboard_m2t_geocoder', ''),
  );
  
  $form['geo']['smarty'] = array(
    '#type' => 'fieldset',
    '#title' => t('Smarty Streets configuration'),
  );

  $form['geo']['smarty']['springboard_m2t_smarty_authid'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth ID'),
    '#description' => t('Required for the REST API'),
    '#default_value' => variable_get('springboard_m2t_smarty_authid', ''),
  );

  $form['geo']['smarty']['springboard_m2t_smarty_authtoken'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth Token'),
    '#description' => t('Required for the REST AP'),
    '#default_value' => variable_get('springboard_m2t_smarty_authtoken', ''),
  );

  $form['geo']['smarty']['springboard_m2t_smarty_htmltoken'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets HTML token'),
    '#description' => t('Required for the Javascript API only'),
    '#default_value' => variable_get('springboard_m2t_smarty_htmltoken', ''),
  );

  return system_settings_form($form);
}

