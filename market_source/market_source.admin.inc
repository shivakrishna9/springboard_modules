<?php
// $Id$

/**
 * @file
 * Provides an admin interface for the Market Source module.
 */

/**
 * Callback function for the admin settings menu item.
 *
 * @return array
 *   Returns a system_settings_form() formatted Form API array.
 */
function market_source_admin_settings() {
  $form = $campaigns = array();
  $campaigns = array_merge(array('0' => t('- Select -')), salesforce_management_api_get_campaigns());

  $form['market_source_default_campaign'] = array(
    '#type' => 'select',
    '#title' => t('Default Campaign'),
    '#description' => t('Select a default campaign for your donation forms.'),
    '#options' => $campaigns,
    '#default_value' => variable_get('market_source_default_campaign', 0),
  );
  $form['market_source_validity_period'] = array(
    '#title' => 'Validity Period',
    '#description' => 'Determines how long values should be stored in the user\'s session.<br /><strong>Note:</strong> Values do not persist if the user\'s session ends.',
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => array(
      6 => t('6 Hours'),
      12 => t('12 Hours'),
      24 => t('1 Day'),
      48 => t('2 Days'),
      72 => t('3 Days'),
      96 => t('4 Days'),
      120 => t('5 Days'),
      144 => t('6 Days'),
      168 => t('1 Week'),
      336 => t('2 Weeks'),
      504 => t('3 Weeks'),
      672 => t('4 Weeks'),
      0 => t('Always Valid'),
    ),
    '#default_value' => variable_get('market_source_validity_period', 6),
  );
  
  if (module_exists('salesforce_management_api')) {
  	$form['market_source_validate_sf_campaigns'] = array(
  		'#title' => t('Validate Salesforce Campaigns'),
  		'#type' => 'checkbox',
  		'#description' => t('When enabled, campaigns will be validated against a list of Salesforce campaigns before being saved.'),
  		'#default_value' => variable_get('market_source_validate_sf_campaigns', 0),
  	);
  }

  return system_settings_form($form);
}
