<?php

/**
 * @file
 * Definitions for the Springboard Administration menu.
 */

/**
 * Defines the custom menu "Springboard Administration".
 *
 * @return array
 *   A associative array describing a Drupal custom menu, as defined by
 *   menu_save().
 */
function springboard_admin_admin_menu() {
  return array(
    'menu_name' => 'springboard_admin_menu',
    'title' => 'Springboard Administration',
    'description' => 'Menu for the custom Springboard administration interface'
  );
}

/**
 * Defines menu items for the custom menu "Springboard Administration".
 *
 * @return array
 *   A heirarchical array of menu items for the Springboard Administratio menu.
 *
 *   Each item in the array is a associative array representing a menu link
 *   item, with elements as defined by menu_link_save().
 */
function springboard_admin_admin_menu_items() {
  $items = array();
  // Top Level: Sprinboard (dashboard).
  $items['admin/springboard'] = array(
    'link_path' => 'admin/springboard',
    'link_title' => t('Springboard'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
    '_children' => array(),
  );

  // Springboard > Your Website
  $items['admin/springboard']['_children']['website'] = array(
    'link_path' => 'http://drupal.org',
    'link_title' => 'Your Website',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 0,
    'options' => array('attributes' => array('target' => '_blank')),
  );

  // Springboard > Salesforce.com
  $items['admin/springboard']['_children']['salesforce'] = array(
    'link_path' => 'http://login.salesforce.com',
    'link_title' => 'Salesforce.com',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 1,
    'options' => array('attributes' => array('target' => '_blank')),
  );

  // Top Level: Donation Form.
  $items['admin/springboard/donation-forms/all'] = array(
    'link_path' => 'admin/springboard/donation-forms/all',
    'link_title' => t('Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
    '_children' => array(),
  );

  // Donation Forms > View All Donation Forms.
  $items['admin/springboard/donation-forms/all']['_children']['admin/springboard/donation-forms/all'] = array(
    'link_path' => 'admin/springboard/donation-forms/all',
    'link_title' => t('View All Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Create a menu item for each type of webform.
  // In the future, we will use an intermediate page to choose the form type.
  $types = node_type_get_types();
  foreach ($types as $type) {
    if ($type && fundraiser_is_donation_type($type->type)) {
      // Donation Forms > $type->type
      $items['admin/springboard/donation-forms/all']['_children']['node/add/' . preg_replace('/_/', '-', $type->type)] = array(
        'link_path' => 'node/add/' . preg_replace('/_/', '-', $type->type),
        'link_title' => 'Create a ' . $type->name,
        'menu_name' => 'springboard_admin_menu',
        'expanded' => 1,
        'customized' => 1,
        'weight' => 1,
      );
    }
  }

  // Donation Forms > Donation Reports.
  $items['admin/springboard/donation-forms/all']['_children']['admin/reports/salesforce/donations'] = array(
    'link_path' => 'admin/reports/salesforce/donations',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Donation Forms > Donation Settings.
  /* @TODO Not in the menu mockup.
   * $items['admin/springboard/donation-forms/all']['_children']['admin/config/salesforce/salesforce-donation'] = array(
    'link_path' => 'admin/config/salesforce/salesforce-donation',
    'link_title' => 'Donation Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  ); */

  // Top Level: Forms.
  $items['admin/springboard/forms/all'] = array(
    'link_path' => 'admin/springboard/forms/all',
    'link_title' => t('Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );

  // Forms > View All Forms.
  $items['admin/springboard/forms/all']['_children']['admin/springboard/forms/all'] = array(
    'link_path' => 'admin/springboard/forms/all',
    'link_title' => t('View All Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Create a menu item for each type of webform.
  // In the future, we will use an intermediate page to choose the form type.
  $types = node_type_get_types();
  foreach ($types as $type) {
    // Only create the item if the node type uses Webform User but not Fundraiser.
    if ($type && _webform_user_is_webform_user_node_type($type->type) && !fundraiser_is_donation_type($type->type)) {
      // Forms > $type->type.
      $items['admin/springboard/forms/all']['_children']['node/add/' . preg_replace('/_/', '-', $type->type)] = array(
        'link_path' => 'node/add/' . preg_replace('/_/', '-', $type->type),
        'link_title' => 'Create a ' . $type->name,
        'menu_name' => 'springboard_admin_menu',
        'expanded' => 1,
        'customized' => 1,
        'weight' => 1,
      );
    }
  }

  // Placeholder: Forms >  Form Reports.
  /*
  $items['admin/springboard/forms/all']['_children'][] = array (
    'link_path' => '',
    'link_title' => 'Form Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' =>  3,
  );
  */

  // Forms > Form Settings.
  /* @TODO Not in the menu mockup.
  $items['admin/springboard/forms/all']['_children']['admin/config/content/webform'] = array(
    'link_path' => 'admin/config/content/webform',
    'link_title' => 'Form Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );
  */

  // Top Level: Asset Library.
  $items['admin/springboard/asset-library'] = array(
    'link_path' => 'admin/springboard/asset-library',
    'link_title' => t('Asset Library'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
    '_children' => array(),
  );

  // Asset Library > Email Templates.
  $items['admin/springboard/asset-library']['_children']['admin/springboard/asset-library/email_wrapper'] = array(
    'link_path' => 'admin/springboard/asset-library/email_wrapper',
    'link_title' => 'Email Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Asset Library > Page Wrappers.
  $items['admin/springboard/asset-library']['_children']['admin/springboard/asset-library/page_wrapper'] = array(
    'link_path' => 'admin/springboard/asset-library/page_wrapper',
    'link_title' => 'Page Wrappers',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Top Level: Marketing & Analytics.
  // @TODO create this page springboard/marketing-analytics
  $items['admin/springboard/marketing-analytics'] = array(
    'link_path' => 'admin/springboard/marketing-analytics',
    'link_title' => t('Marketing & Analytics'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
    '_children' => array(),
  );

  // Marketing & Analytics > Goals.
  $items['admin/springboard/marketing-analytics']['_children']['admin/config/content/webform_goals'] = array(
    'link_path' => 'admin/config/content/webform_goals',
    'link_title' => 'Goals',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Marketing & Analytics > Multivariate Testing.
  $items['admin/springboard/marketing-analytics']['_children']['admin/content/webform_ab'] = array(
    'link_path' => 'admin/content/webform_ab',
    'link_title' => 'Multivariate Testing',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
  );

  // Marketing & Analytics > Social.
  $items['admin/springboard/marketing-analytics']['_children']['admin/config/services/springboard_social'] = array(
    'link_path' => 'admin/config/services/springboard_social',
    'link_title' => 'Social Share',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Marketing & Analytics > Source Codes.
  $items['admin/springboard/marketing-analytics']['_children']['admin/config/search/market-source'] = array(
    'link_path' => 'admin/config/search/market-source',
    'link_title' => 'Source Codes',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Marketing & Analytics > Analytics Integraiton.
  $items['admin/springboard/marketing-analytics']['_children']['admin/config/system/googleanalytics'] = array(
    'link_path' => 'admin/config/system/googleanalytics',
    'link_title' => 'Analytics Integration',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );
  
  // Top Level: Reports
  $items['admin/springboard/reports'] = array(
    'link_path' => 'admin/springboard/reports',
    'link_title' => t('Reports'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
    '_children' => array(),
  );

  // Reports > Donations.
  $items['admin/springboard/reports']['_children']['admin/reports/salesforce/donations'] = array(
    'link_path' => 'admin/reports/salesforce/donations',
    'link_title' => 'Donations',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Reports > Contacts.
  $items['admin/springboard/reports']['_children']['admin/springboard/reports/contacts'] = array(
    'link_path' => 'admin/springboard/reports/contacts',
    'link_title' => 'Contacts',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Reports > Integration Reports.
  $items['admin/springboard/reports']['_children']['admin/springboard/reports/integration-reports'] = array(
    'link_path' => 'admin/springboard/reports/integration-reports',
    'link_title' => 'Integration Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );
  // Reports > Integration Reports > Batch Log.
  $items['admin/springboard/reports']['_children']['admin/springboard/reports/integration-reports']['_children']['admin/springboard/reports/integration-reports/batch'] = array(
    'link_path' => 'admin/springboard/reports/integration-reports/batch',
    'link_title' => 'Batch Log',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );
  // Reports > Integration Reports > Batch Item Log.
  $items['admin/springboard/reports']['_children']['admin/springboard/reports/integration-reports']['_children']['admin/springboard/reports/integration-reports/item'] = array(
    'link_path' => 'admin/springboard/reports/integration-reports/item',
    'link_title' => 'Item Log',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 1,
  );
  
  // Reports > Integration Reports > Queue.
  $items['admin/springboard/reports']['_children']['admin/springboard/reports/integration-reports']['_children']['admin/springboard/reports/integration-reports/queue'] = array(
    'link_path' => 'admin/springboard/reports/integration-reports/queue',
    'link_title' => 'Queue',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
  );

  // Top Level: Settings.
  $items['admin/springboard/settings'] = array(
    'link_path' => 'admin/springboard/settings',
    'link_title' => t('Options'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 6,
    '_children' => array(),
  );

  // Settings > My Profile.
  $items['admin/springboard/settings']['_children']['user'] = array(
    'link_path' => 'user',
    'link_title' => 'My Profile',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Settings > Administrators
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/administrators'] = array(
    'link_path' => 'admin/springboard/settings/administrators',
    'link_title' => 'Administrators',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Settings > Configuration.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config'] = array(
    'link_path' => 'admin/springboard/settings/config',
    'link_title' => 'Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );
  
  // Settings > Settings > Sustainer Upsell.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/springboard/options/fundraiser/fundraiser_upsell'] = array(
    'link_path' => 'admin/springboard/options/fundraiser/fundraiser_upsell',
    'link_title' => 'Import salesforce mapping',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );
  
  // Settings > Settings > Sustainer upsell confirmations.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/springboard/options/fundraiser/fundraiser_upsell/thank-you'] = array(
    'link_path' => 'admin/springboard/options/fundraiser/fundraiser_upsell/thank-you',
    'link_title' => 'Sustainer upsell confirmations',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 1,
  );
  
  // Settings > Settings > Multivariate Testing.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/config/content/webform_ab'] = array(
    'link_path' => 'admin/config/content/webform_ab',
    'link_title' => 'Multivariate Testing',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 1,
  );
  
  // Settings > Settings > Fieldmaps.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/structure/salesforce/mappings'] = array(
    'link_path' => 'admin/structure/salesforce/mappings',
    'link_title' => 'Fieldmaps',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
  );
  
  // Settings > Settings > Add salesforce mapping.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/structure/salesforce/mappings/add'] = array(
    'link_path' => 'admin/structure/salesforce/mappings/add',
    'link_title' => 'Add salesforce mapping',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
  );
  
  // Settings > Settings > Import salesforce mapping.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/structure/salesforce/mappings/import'] = array(
    'link_path' => 'admin/structure/salesforce/mappings/import',
    'link_title' => 'Add salesforce mapping',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 3,
  );
  // Settings > Salesforce Authentication.
   $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/config/salesforce/authorize'] = array(
    'link_path' => 'admin/config/salesforce/authorize',
    'link_title' => 'Salesforce Authorization',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 4,
  );
  
  // Settings > Salesforce Authentication.
   $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/springboard/settings/springboard'] = array(
    'link_path' => 'admin/springboard/settings/springboard',
    'link_title' => 'Springboard Configuration',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 5,
  ); 

  // Settings > Configure Dashboard.
  $items['admin/springboard/settings']['_children']['admin/springboard/settings/config']['_children']['admin/springboard/settings/dashboard'] = array(
    'link_path' => 'admin/springboard/settings/dashboard',
    'link_title' => 'Configure Dashboard',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 6,
  );

  //Settings > Logout.
  $items['admin/springboard/settings']['_children']['logout'] = array(
    'link_path' => 'user/logout',
    'link_title' => 'Logout',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );
  return $items;
}
