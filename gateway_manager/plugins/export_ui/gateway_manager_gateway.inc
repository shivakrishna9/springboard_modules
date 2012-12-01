<?php

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'gateway_manager_gateways',
  'access' => 'administer site configuration', // TODO

  'menu' => array(
    'menu prefix' => 'admin/settings/fundraiser',
    'menu item' => 'gateways',
    'menu title' => 'Payment gateways',
    'menu description' => 'Administer the available payment gateways.',
  ),

  'title singular' => t('Payment gateway'),
  'title plural' => t('Payment gateways'),
  'title singular proper' => t('Gateway Manager payment gateway'),
  'title plural proper' => t('Gateway Manager payment gateway'),

  /*
  'form' => array(
    'settings' => 'multi_smtp_server_ui_edit_settings_form',
    'validate' => 'multi_smtp_server_ui_edit_settings_form_validate',
    'submit' => 'multi_smtp_server_ui_edit_settings_form_submit',
  ),
  */

  'handler' => array(
    'class' => 'gateway_manager_gateway_ui',
    'parent' => 'ctools_export_ui',
  ),
);
