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
  $items['springboard'] = array(
    'link_path' => 'springboard',
    'link_title' => t('Springboard'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
    '_children' => array(),
  );

  // Springboard > Your Website
  $items['springboard']['_children']['website'] = array(
    'link_path' => 'http://drupal.org',
    'link_title' => 'Your Website',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Springboard > Salesforce.com
  $items['springboard']['_children']['salesforce'] = array(
    'link_path' => 'http://login.salesforce.com',
    'link_title' => 'Salesforce.com',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Top Level: Donation Form.
  $items['springboard/forms/donations'] = array(
    'link_path' => 'springboard/forms/donations',
    'link_title' => t('Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
    '_children' => array(),
  );

  // Donation Forms > View All Donation Forms.
  $items['springboard/forms/donations']['_children']['springboard/forms/donations'] = array(
    'link_path' => 'springboard/forms/donations',
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
      $items['springboard/forms/donations']['_children']['springboard/add/' . $type->type] = array(
        'link_path' => 'springboard/add/' . $type->type,
        'link_title' => 'Create a ' . $type->name,
        'menu_name' => 'springboard_admin_menu',
        'expanded' => 1,
        'customized' => 1,
        'weight' => 1,
      );
    }
  }

  // Donation Forms > Donation Reports.
  $items['springboard/forms/donations']['_children']['springboard/reports/dsr'] = array(
    'link_path' => 'springboard/reports/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Donation Forms > Donation Settings.
  /* @TODO Not in the menu mockup.
   * $items['springboard/forms/donations']['_children']['admin/config/salesforce/salesforce-donation'] = array(
    'link_path' => 'admin/config/salesforce/salesforce-donation',
    'link_title' => 'Donation Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  ); */

  // Top Level: Forms.
  $items['springboard/forms/all'] = array(
    'link_path' => 'springboard/forms/all',
    'link_title' => t('Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );

  // Forms > View All Forms.
  $items['springboard/forms/all']['_children']['springboard/forms/all'] = array(
    'link_path' => 'springboard/forms/all',
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
    if ($type && _webform_user_is_webform_user_node_type($type->type)) {
      // Forms > $type->type.
      $items['springboard/forms/all']['_children']['springboard/add/' . $type->type] = array(
        'link_path' => 'springboard/add/' . $type->type,
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
  $items['forms']['_children'][] = array (
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
  $items['springboard/forms/all']['_children']['admin/config/content/webform'] = array(
    'link_path' => 'admin/config/content/webform',
    'link_title' => 'Form Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );
  */

  // Top Level: Asset Library.
  $items['springboard/asset-library'] = array(
    'link_path' => 'springboard/asset-library',
    'link_title' => t('Asset Library'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
    '_children' => array(),
  );

  // Asset Library > Email Templates.
  $items['springboard/asset-library']['_children']['springboard/asset-library/email-templates'] = array(
    'link_path' => 'springboard/asset-library/email-templates',
    'link_title' => 'Email Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Asset Library > Page Templates.
  $items['springboard/asset-library']['_children']['springboard/asset-library/page-templates'] = array(
    'link_path' => 'springboard/asset-library/page-templates',
    'link_title' => 'Page Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Top Level: Marketing & Analytics.
  // @TODO create this page springboard/marketing-analytics
  $items['springboard/marketing-analytics'] = array(
    'link_path' => 'springboard/marketing-analytics',
    'link_title' => t('Marketing & Analytics'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
    '_children' => array(),
  );

  // Marketing & Analytics > Goals.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/webform-goals'] = array(
    'link_path' => 'springboard/settings/webform-goals',
    'link_title' => 'Goals',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Marketing & Analytics > Multivariate Testing.
  $items['springboard/marketing-analytics']['_children']['admin/config/content/webform_ab'] = array(
    'link_path' => 'admin/config/content/webform_ab',
    'link_title' => 'Multivariate Testing',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
  );

  // Marketing & Analytics > Social.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/sb-social'] = array(
    'link_path' => 'springboard/settings/sb-social',
    'link_title' => 'Social Share',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Marketing & Analytics > Source Codes.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/market-source'] = array(
    'link_path' => 'springboard/settings/market-source',
    'link_title' => 'Source Codes',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Marketing & Analytics > Analytics Integraiton.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/ga'] = array(
    'link_path' => 'springboard/settings/ga',
    'link_title' => 'Analytics Integration',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Marketing & Analytics > Metatags
  // @TODO where does this go?

  // Top Level: Reports
  // @TODO Create this page.
  $items['springboard/reports'] = array(
    'link_path' => 'springboard/reports',
    'link_title' => t('Reports'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
    '_children' => array(),
  );

  // Reports > Donations.
  $items['springboard/reports']['_children']['admin/reports/salesforce/donations'] = array(
    'link_path' => 'admin/reports/salesforce/donations',
    'link_title' => 'Donations',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Reports > Contacts.
  $items['springboard/reports']['_children']['springboard/reports/contacts'] = array(
    'link_path' => 'springboard/reports/contacts',
    'link_title' => 'Contacts',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Reports > Integration Reports.
  $items['springboard/reports']['_children']['springboard/reports/integration-reports'] = array(
    'link_path' => 'springboard/reports/integration-reports',
    'link_title' => 'Integration Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Top Level: Settings.
  /* @TODO: Create this page. */
  $items['springboard/settings'] = array(
    'link_path' => 'springboard/settings',
    'link_title' => t('Options'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 6,
    '_children' => array(),
  );

  // Settings > My Profile.
  $items['springboard/settings']['_children']['user'] = array(
    'link_path' => 'user',
    'link_title' => 'My Profile',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Settings > Administrators
  /* @TODO create this page */
  $items['springboard/settings']['_children']['springboard/settings/administrators'] = array(
    'link_path' => 'springboard/settings/administrators',
    'link_title' => 'Administrators',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Settings > Settings.
  $items['springboard/settings']['_children']['springboard/settings/springboard'] = array(
    'link_path' => 'springboard/settings/springboard',
    'link_title' => 'Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Settings > Salesforce Authentication.
  /* @TODO not in menu mockup
   $items['springboard/settings']['_children']['admin/config/salesforce/authorize'] = array(
    'link_path' => 'admin/config/salesforce/authorize',
    'link_title' => 'Salesforce Authorization',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  ); */

  // Settings > Configure Dashboard.
  /*  not in menu mockup
  $items['springboard/settings']['_children']['springboard/settings/dashboard'] = array(
    'link_path' => 'springboard/settings/dashboard',
    'link_title' => t('Configure Dashboard'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );*/

  //Settings > Logout.
  $items['springboard/settings']['_children']['logout'] = array(
    'link_path' => 'user/logout',
    'link_title' => 'Logout',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );
  return $items;
}
