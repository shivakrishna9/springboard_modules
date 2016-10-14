<?php
/**
 * @file
 * Drupal and Springboard Menu hooks for Springboard Groups.
 */

/**
 * Implements hook_menu().
 */
function springboard_group_menu() {
  $items = array();

  $items['admin/springboard/groups/settings'] = array(
    'title' => 'Springboard Groups Settings',
    'description' => 'Configuration settings for Springboard Group features',
    'page callback' => 'springboard_group_settings_page',
    'access arguments' => array('administer springboard groups'),
    'file' => 'includes/springboard_group.admin.inc',
    'type' => MENU_NORMAL_ITEM,
  );

  // OG module doesn't define a default tab. We help it here.
  $items['group/%/%/admin/people/all'] = array(
    'title' => 'People',
    'type' => MENU_DEFAULT_LOCAL_TASK,
  );

  $items['group/%/%/admin/people/bulk-add'] = array(
    'page callback' => 'springboard_group_bulk_add_page',
    'page arguments' => array(1, 2),
    'type' => MENU_LOCAL_TASK,
    'weight' => 100,
    'title' => 'Bulk Add Members',
    'access callback' => 'og_ui_user_access_group',
    'access arguments' => array('add user', 1, 2),
    'file' => 'includes/springboard_group.pages.inc',
  );

  return $items;
}

/**
 * Implements hook_springboard_admin_admin_menu_items_alter().
 *
 * Add the springboard group links to springboard admin.
 */
function springboard_group_springboard_admin_admin_menu_items_alter(&$items) {

  $items['admin/springboard/groups'] = array(
    'link_path' => 'admin/springboard/groups',
    'link_title' => t('Groups'),
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 3,
    '_children' => array(),
  );

  $items['admin/springboard/groups']['_children']['admin/springboard/groups'] = array(
    'link_path' => 'admin/springboard/groups',
    'link_title' => 'View All Groups',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => -5,
  );

  $items['admin/springboard/groups']['_children']['admin/springboard/groups/my'] = array(
    'link_path' => 'admin/springboard/groups/my',
    'link_title' => 'My Groups',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => -4,
  );

  $items['admin/springboard/groups']['_children']['admin/springboard/groups/settings'] = array(
    'link_path' => 'admin/springboard/groups/settings',
    'link_title' => 'Groups Settings',
    'menu_name' => 'springboard_admin_menu',
    'expanded' => 1,
    'customized' => 1,
    'weight' => 0,
  );
}


/**
 * Implements hook_springboard_admin_alias_patterns().
 *
 * Put groups paths under springboard backend theme.
 */
function springboard_group_springboard_admin_alias_patterns() {
  return array(
    'group/node/%/admin/people' => array(
      'path' => array(
        'regex' => '|^/group/node/([0-9]+)/admin/people|',
        'replacement' => 'group/node/$1/admin/people',
      ),
      'alias' => array(
        'regex' => '|^springboard/group/node/([0-9]+)/admin/people|',
        'replacement' => 'springboard/group/node/$1/admin/people',
      ),
    ),
    'group/node/%/admin/people/add-user' => array(
      'path' => array(
        'regex' => '|^/group/node/([0-9]+)/admin/people/add-user|',
        'replacement' => 'group/node/$1/admin/people/add-user',
      ),
      'alias' => array(
        'regex' => '|^springboard/group/node/([0-9]+)/admin/people/add-user|',
        'replacement' => 'springboard/group/node/$1/admin/people/add-user',
      ),
    ),
    'group/node/%/admin/people/bulk-add' => array(
      'path' => array(
        'regex' => '|^/group/node/([0-9]+)/admin/people/bulk-add|',
        'replacement' => 'group/node/$1/admin/people/bulk-add',
      ),
      'alias' => array(
        'regex' => '|^springboard/group/node/([0-9]+)/admin/people/bulk-add|',
        'replacement' => 'springboard/group/node/$1/admin/people/bulk-add',
      ),
    ),
    'group/node/%/admin/roles' => array(
      'path' => array(
        'regex' => '|^/group/node/([0-9]+)/admin/roles|',
        'replacement' => 'group/node/$1/admin/roles',
      ),
      'alias' => array(
        'regex' => '|^springboard/group/node/([0-9]+)/admin/roles|',
        'replacement' => 'springboard/group/node/$1/admin/roles',
      ),
    ),
    'group/node/%/admin/people/permissions' => array(
      'path' => array(
        'regex' => '|^/group/node/([0-9]+)/admin/permissions|',
        'replacement' => 'group/node/$1/admin/permissions',
      ),
      'alias' => array(
        'regex' => '|^springboard/group/node/([0-9]+)/admin/permissions|',
        'replacement' => 'springboard/group/node/$1/admin/permissions',
      ),
    ),
  );
}
