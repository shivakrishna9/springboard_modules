<?php

/**
 * @file
 *
 * Definitions for the Springboard Administration menu
 */

/**
 * Defines the custom menu "Springboard Administration"
 *
 *  @return
 *  A associative array describing a Drupal custom menu, as defined by
 *  menu_save().
 *
 */
function springboard_admin_admin_menu() {
  return array(
    'menu_name' => 'springboard_admin_menu',
    'title' => t('Springboard Administration'),
    'description' => t('Menu for the custom Springboard administration interface')
  );
}

/**
 * Defines menu items for the custom menu "Springboard Administration"
 *
 *  @return
 *  A heirarchical array of menu items for the Springboard Administratio menu.
 *
 *  Each item in the array is a associative array representing a menu link item,
 *  with elements as defined by menu_link_save().
 *
 */
function springboard_admin_admin_menu_items() {
  $items = array();
  // Top Level: Home (dashboard)
  $items['admin/springboard'] = array(
    'link_path' => 'admin/springboard',
    'link_title' => t('Home'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
    '_children' => array(),
  );
  
  // Top Level: Donation Form
  $items['admin/springboard/forms/donations'] = array(
    'link_path' => 'admin/springboard/forms/donations',
    'link_title' => t('Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
    '_children' => array(),
  );

  // Donation Forms > View All Donation Forms
  $items['admin/springboard/forms/donations']['_children']['admin/springboard/forms/donations'] = array(
    'link_path' => 'admin/springboard/forms/donations',
    'link_title' => t('View All Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Donation Forms > Create Donation Form
  $items['admin/springboard/forms/donations']['_children']['node/add/donation-form'] = array(
    'link_path' => 'node/add/donation-form',
    'link_title' => 'Create a Donation Form',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Donation Forms > Confirmation Templates
    $items['admin/springboard/forms/donations']['_children']['admin/springboard/templates'] = array(
      'link_path' => 'admin/springboard/templates',
      'link_title' => 'Confirmation Templates',
      'menu_name' => 'springboard_admin_menu',
      'expanded' => 1,
      'customized' => 1,
      'weight' => 2,
    );

  // Donation Forms > Donation Reports
  $items['admin/springboard/forms/donations']['_children']['admin/springboard/repots/dsr'] = array(
    'link_path' => 'admin/springboard/repots/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Donation Forms > Donation Settings
  $items['admin/springboard/forms/donations']['_children']['admin/config/salesforce/salesforce-donation'] = array(
    'link_path' => 'admin/config/salesforce/salesforce-donation',
    'link_title' => 'Donation Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Top Level: Forms
  $items['admin/springboard/forms/webforms'] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => t('Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );

  // Forms > View All Forms
  $items['admin/springboard/forms/webforms']['_children']['admin/springboard/forms/webforms'] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => t('View All Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  /**
   * Create a menu item for each type of webform
   * In the future, we will use an intermediate page to choose the form type.
   **/
  $webforms = variable_get('webform_node_types', array());
  foreach ($webforms as $node_type) {
    if ($node_type && $node_type != 'donation_form') {
      // Forms > $node_type
      $items['admin/springboard/forms/webforms']['_children']['node/add/' . $node_type] = array(
        'link_path' => 'node/add/' . $node_type,
        'link_title' => 'Create a ' . $node_type . ' Form',
        'menu_name' => 'springboard_admin_menu',
        'expanded' => 1,
        'customized' => 1,
        'weight' => 1,
      );
    }
  }

  // Forms > Confirmation Templates
  $items['admin/springboard/forms/webforms']['_children']['admin/springboard/templates'] = array(
    'link_path' => 'admin/springboard/templates', 
    'link_title' => 'Confirmation Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
  );

  // Placeholder: Forms >  Form Reports
  /*$items['forms']['_children'][] = array (
   'link_path' => '',
   'link_title' => 'Form Reports',
   'menu_name' => 'springboard_admin_menu',
   'expanded' => 1,
   'customized' => 1,
   'weight' =>  3,
   );*/

  // Forms > Form Settings
  $items['admin/springboard/forms/webforms']['_children']['admin/springboard/forms/webforms'] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => 'Form Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Top Level: Templates
  $items['admin/springboard/templates'] = array(
    'link_path' => 'admin/springboard/templates', 
    'link_title' => t('Templates'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
    '_children' => array(),
  );

  // Templates > Page Templates
  $items['admin/springboard/templates']['_children']['admin/springboard/templates/page-templates'] = array(
    'link_path' => 'admin/springboard/templates/page-templates', 
    'link_title' => 'Page Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Templates > Email Templates
  $items['admin/springboard/templates']['_children']['admin/springboard/templates/email-templates'] = array(
    'link_path' => 'admin/springboard/templates/email-templates', 
    'link_title' => 'Email Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Top Level: Marketing & Analytics
  $items['admin/springboard/marketing-analytics'] = array(
    'link_path' => 'admin/springboard/marketing-analytics', // @TODO create this
    // page admin/springboard/marketing-analytics
    'link_title' => t('Marketing & Analytics'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
    '_children' => array(),
  );

  // Marketing & Analytics > Source Codes
  $items['admin/springboard/marketing-analytics']['_children']['admin/springboard/settings/market-source'] = array(
    'link_path' => 'admin/springboard/settings/market-source',
    'link_title' => 'Source Codes',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Marketing & Analytics > Analytics Integraiton
  $items['admin/springboard/marketing-analytics']['_children']['admin/springboard/settings/ga'] = array(
    'link_path' => 'admin/springboard/settings/ga',
    'link_title' => 'Analytics Integration',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Top Level: Results
  $items['admin/springboard/results'] = array(
    'link_path' => 'admin/springboard/results', // @TODO create this page
    // admin/springboard/results
    'link_title' => t('Results'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
    '_children' => array(),
  );

  // Marketing & Analytics > Donation Reports
  $items['admin/springboard/results']['_children']['admin/springboard/repots/dsr'] = array(
    'link_path' => 'admin/springboard/repots/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Top Level: Options
  $items['admin/springboard/options'] = array(
    'link_path' => 'admin/springboard/options',
    'link_title' => t('Options'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 6,
    '_children' => array(),
  );

  // Options > My Profile
  $items['admin/springboard/options']['_children']['user'] = array(
    'link_path' => 'user',
    'link_title' => 'My Profile',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Options > Settings
  $items['admin/springboard/options']['_children']['admin/springboard/options/springboard'] = array(
    'link_path' => 'admin/springboard/options/springboard', 
    'link_title' => 'Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );
  
  // Options > Salesforce Authentication
  $items['admin/springboard/options']['_children']['admin/config/salesforce/authorize'] = array(
    'link_path' => 'admin/config/salesforce/authorize', 
    'link_title' => 'Salesforce Authorization',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );
  
  // Options > Configure Dashboard
  $items['admin/springboard/options']['_children']['admin/springboard/options/dashboard'] = array(
    'link_path' => 'admin/springboard/options/dashboard',
    'link_title' => t('Configure Dashboard'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );

  $items['tools'] = array(
    'link_path' => '', // @TODO determine a page for this
    'link_title' => 'Other Tools',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 7,
    '_children' => array(),
  );

  $items['tools']['drupal'] = array(
    'link_path' => 'http://drupal.org',
    'link_title' => 'Drupal',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  $items['tools']['salesforce'] = array(
    'link_path' => 'http://salesforce.com',
    'link_title' => 'Salesforce.com',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 1,
  );
  return $items;
}
