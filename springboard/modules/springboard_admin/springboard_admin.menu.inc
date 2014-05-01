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
  // Top Level: Home (dashboard).
  $items['springboard'] = array(
    'link_path' => 'springboard',
    'link_title' => t('Home'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
    '_children' => array(),
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

  // Donation Forms > Create Donation Form.
  $items['springboard/forms/donations']['_children']['node/add/donation-form'] = array(
    'link_path' => 'node/add/donation-form',
    'link_title' => 'Create a Donation Form',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Donation Forms > Confirmation Templates.
    $items['springboard/forms/donations']['_children']['springboard/templates'] = array(
      'link_path' => 'springboard/templates',
      'link_title' => 'Confirmation Templates',
      'menu_name' => 'springboard_admin_menu',
      'expanded' => 1,
      'customized' => 1,
      'weight' => 2,
    );

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
  $items['springboard/forms/donations']['_children']['admin/config/salesforce/salesforce-donation'] = array(
    'link_path' => 'admin/config/salesforce/salesforce-donation',
    'link_title' => 'Donation Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Top Level: Forms.
  $items['springboard/forms/webforms'] = array(
    'link_path' => 'springboard/forms/webforms',
    'link_title' => t('Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
    '_children' => array(),
  );

  // Forms > View All Forms.
  $items['springboard/forms/webforms']['_children']['springboard/forms/webforms'] = array(
    'link_path' => 'springboard/forms/webforms',
    'link_title' => t('View All Forms'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Create a menu item for each type of webform.
  // In the future, we will use an intermediate page to choose the form type.
  $webforms = variable_get('webform_node_types', array());
  foreach ($webforms as $node_type) {
    if ($node_type && $node_type != 'donation_form') {
      // Forms > $node_type
      $items['springboard/forms/webforms']['_children']['node/add/' . $node_type] = array(
        'link_path' => 'node/add/' . $node_type,
        'link_title' => 'Create a ' . $node_type . ' Form',
        'menu_name' => 'springboard_admin_menu',
        'expanded' => 1,
        'customized' => 1,
        'weight' => 1,
      );
    }
  }

  // Forms > Confirmation Templates.
  $items['springboard/forms/webforms']['_children']['springboard/templates'] = array(
    'link_path' => 'springboard/templates',
    'link_title' => 'Confirmation Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 2,
  );

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
  $items['springboard/forms/webforms']['_children']['admin/config/content/webform'] = array(
    'link_path' => 'admin/config/content/webform',
    'link_title' => 'Form Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Top Level: Templates.
  $items['springboard/templates'] = array(
    'link_path' => 'springboard/templates',
    'link_title' => t('Templates'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
    '_children' => array(),
  );

  // Templates > Page Templates.
  $items['springboard/templates']['_children']['springboard/templates/page-templates'] = array(
    'link_path' => 'springboard/templates/page-templates',
    'link_title' => 'Page Templates',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Templates > Email Templates.
  $items['springboard/templates']['_children']['springboard/templates/email-templates'] = array(
    'link_path' => 'springboard/templates/email-templates',
    'link_title' => 'Email Templates',
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

  // Marketing & Analytics > Source Codes.
  $items['springboard/marketing-analytics']['_children']['springboard/options/market-source'] = array(
    'link_path' => 'springboard/options/market-source',
    'link_title' => 'Source Codes',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Marketing & Analytics > Analytics Integraiton.
  $items['springboard/marketing-analytics']['_children']['springboard/options/ga'] = array(
    'link_path' => 'springboard/options/ga',
    'link_title' => 'Analytics Integration',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Marketing & Analytics > Goals.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/webform-goals'] = array(
    'link_path' => 'springboard/settings/webform-goals',
    'link_title' => 'Goals',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
  );

  // Marketing & Analytics > Social.
  $items['springboard/marketing-analytics']['_children']['springboard/settings/sb-social'] = array(
    'link_path' => 'springboard/settings/sb-social',
    'link_title' => 'Social',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 4,
  );

  // Marketing & Analytics > A/B Testing.
  $items['springboard/marketing-analytics']['_children']['admin/config/content/webform_ab'] = array(
    'link_path' => 'admin/config/content/webform_ab',
    'link_title' => 'A/B Testing',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
  );

  // Top Level: Results
  // @todo Create this page.
  $items['springboard/results'] = array(
    'link_path' => 'springboard/reports/dsr', 
    'link_title' => t('Results'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 5,
    '_children' => array(),
  );

  // Marketing & Analytics > Donation Reports.
  $items['springboard/results']['_children']['springboard/reports/dsr'] = array(
    'link_path' => 'springboard/reports/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Marketing & Analytics > Donations.
  $items['springboard/results']['_children']['admin/reports/salesforce/donations'] = array(
    'link_path' => 'admin/reports/salesforce/donations',
    'link_title' => 'Donations',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Marketing & Analytics > Donors.
  $items['springboard/results']['_children']['admin/people'] = array(
    'link_path' => 'admin/people',
    'link_title' => 'Donors',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );

  // Top Level: Options.
  $items['springboard/options'] = array(
    'link_path' => 'springboard/options',
    'link_title' => t('Options'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 6,
    '_children' => array(),
  );

  // Options > My Profile.
  $items['springboard/options']['_children']['user'] = array(
    'link_path' => 'user',
    'link_title' => 'My Profile',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  // Options > Settings.
  $items['springboard/options']['_children']['springboard/options/springboard'] = array(
    'link_path' => 'springboard/options/springboard',
    'link_title' => 'Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Options > Salesforce Authentication.
  $items['springboard/options']['_children']['admin/config/salesforce/authorize'] = array(
    'link_path' => 'admin/config/salesforce/authorize',
    'link_title' => 'Salesforce Authorization',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  // Options > Configure Dashboard.
  $items['springboard/options']['_children']['springboard/options/dashboard'] = array(
    'link_path' => 'springboard/options/dashboard',
    'link_title' => t('Configure Dashboard'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 0,
    'customized' => 1,
    'weight' => 0,
  );

  $items['springboard/tools'] = array(
    'link_path' => 'springboard/tools', // @TODO determine a page for this
    'link_title' => 'Other Tools',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 7,
    '_children' => array(),
  );

  $items['springboard/tools']['_children']['website'] = array(
    'link_path' => 'http://drupal.org',
    'link_title' => 'Your Website',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 0,
  );

  $items['springboard/tools']['_children']['salesforce'] = array(
    'link_path' => 'http://login.salesforce.com',
    'link_title' => 'Salesforce.com',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'external' => 1,
    'customized' => 1,
    'weight' => 1,
  );

  $items['springboard/tools']['_children']['logout'] = array(
    'link_path' => 'user/logout',
    'link_title' => 'Logout',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 2,
  );
  return $items;
}
