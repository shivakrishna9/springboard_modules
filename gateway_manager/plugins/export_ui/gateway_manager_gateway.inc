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

  'title singular' => t('payment gateway'),
  'title plural' => t('payment gateways'),
  'title singular proper' => t('Gateway Manager payment gateway'),
  'title plural proper' => t('Gateway Manager payment gateway'),

  'handler' => array(
    'class' => 'gateway_manager_gateway_ui',
    'parent' => 'ctools_export_ui',
  ),
);
