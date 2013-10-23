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

  // Top Level: Donation Form
  $items['donation_forms'] = array(
    'link_path' => 'admin/springboard/forms/donation',
    'link_title' => t('Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
    '_children' => array(),
  );

  // Donation Forms > View All Donation Forms
  $items['donation_forms']['_children'][] = array(
    'link_path' => 'admin/springboard/forms/donation',
    'link_title' => t('View All Donation Forms'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
  );

  // Donation Forms > Create Donation Form
  $items['donation_forms']['_children'][] = array(
    'link_path' => 'node/add/donation-form',
    'link_title' => 'Create a Donation Form',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 1,
  );

  // Donation Forms > Confirmation Templates
  if (module_exists('page_wrappers') || module_exists('page_wrappers')) {
    $items['donation_forms']['_children'][] = array(
      'link_path' => 'admin/springboard/templates', //@TODO create this page
      // admin/springboard/templates
      'link_title' => 'Confirmation Templates',
      'menu_name' => 'springboard_admin_menu',
      'weight' => 2,
    );
  }

  // Donation Forms > Donation Reports
  $items['donation_forms']['_children'][] = array(
    'link_path' => 'admin/springboard/repots/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 3,
  );

  // Donation Forms > Donation Settings
  $items['donation_forms']['_children'][] = array(
    'link_path' => 'admin/config/salesforce/salesforce-donation',
    'link_title' => 'Donation Settings',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 4,
  );

  // Top Level: Forms
  $items['forms'] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => t('Forms'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 1,
    '_children' => array(),
  );

  // Forms > View All Forms
  $items['forms']['_children'][] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => t('View All Forms'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
  );

  /**
   * Create a menu item for each type of webform
   * In the future, we will use an intermediate page to choose the form type.
   **/
  $webforms = variable_get('webform_node_types', serialize(array()));
  unserialize($webforms);
  foreach ($webforms as $node_type) {
    if ($node_type && $node_type != 'donation_form') {
      drupal_set_message($node_type);
      // Forms > $node_type
      $items['forms']['_children'][] = array(
        'link_path' => 'node/add/' . $node_type,
        'link_title' => 'Create a ' . $node_type . ' Form',
        'menu_name' => 'springboard_admin_menu',
        'weight' => 1,
      );
    }
  }

  // Forms > Confirmation Templates
  $items['forms']['_children'][] = array(
    'link_path' => 'admin/springboard/templates', //@TODO create this page
    // admin/springboard/templates
    'link_title' => 'Confirmation Templates',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 2,
  );

  // Placeholder: Forms >  Form Reports
  /*$items['forms']['_children'][] = array (
   'link_path' => '',
   'link_title' => 'Form Reports',
   'menu_name' => 'springboard_admin_menu',
   'weight' => 3,
   );*/

  // Forms > Form Settings
  $items['forms']['_children'][] = array(
    'link_path' => 'admin/springboard/forms/webforms',
    'link_title' => 'Form Settings',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 4,
  );

  // Top Level: Templates
  $items['templates'] = array(
    'link_path' => 'admin/springboard/templates', //@TODO create this page
    // admin/springboard/templates
    'link_title' => t('Templates'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 2,
    '_children' => array(),
  );

  // Templates > Page Templates
  $items['templates']['_children'][] = array(
    'link_path' => '/admin/springboard/templates/page_templates', // @TODO create
    // this page /admin/springboard/templates/page_templates
    'link_title' => 'Page Templates',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 1,
  );

  // Templates > Email Templates
  $items['templates']['_children'][] = array(
    'link_path' => '/admin/springboard/templates/email_templates', // @TODO
    // create this page /admin/springboard/templates/email_templates
    'link_title' => 'Email Templates',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 1,
  );

  // Top Level: Marketing & Analytics
  $items['marketing_analytics'] = array(
    'link_path' => 'admin/springboard/marketing_analytics', // @TODO create this
    // page admin/springboard/marketing_analytics
    'link_title' => t('Marketing & Analytics'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 4,
    '_children' => array(),
  );

  // Marketing & Analytics > Source Codes
  $items['marketing_analytics']['_children'][] = array(
    'link_path' => '/admin/springboard/settings/market-source',
    'link_title' => 'Source Codes',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 1,
  );

  // Marketing & Analytics > Analytics Integraiton
  $items['marketing_analytics']['_children'][] = array(
    'link_path' => '/admin/springboard/settings/ga',
    'link_title' => 'Analytics Integration',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 2,
  );

  // Top Level: Results
  $items['results'] = array(
    'link_path' => 'admin/springboard/results', // @TODO create this page
    // admin/springboard/results
    'link_title' => t('Results'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 5,
    '_children' => array(),
  );

  // Marketing & Analytics > Donation Reports
  $items['results']['_children'][] = array(
    'link_path' => 'admin/springboard/repots/dsr',
    'link_title' => 'Donation Reports',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
  );

  // Top Level: Options
  $items['options'] = array(
    'link_path' => 'admin/springboard/settings',
    'link_title' => t('Options'),
    'menu_name' => 'springboard_admin_menu',
    'weight' => 6,
    '_children' => array(),
  );

  // Options > My Profile
  $items['options']['_children'][] = array(
    'link_path' => 'user',
    'link_title' => 'My Profile',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
  );

  // Options > Settings
  $items['options']['_children'][] = array(
    'link_path' => 'admin/springboard/settings', // @TODO redo this page in
    // springboard or make a new page and change path here
    'link_title' => 'Settings',
    'menu_name' => 'springboard_admin_menu',
    'weight' => 0,
  );

  return $items;
}
