<?php
/**
 * @file
 * Drupal and OG permission hooks.
 */

/**
 * Implements hook_permission().
 */
function springboard_group_permission() {
  return array(
    // Grants access to our custom groups admin ui.
    'configure springboard groups' => array(
      'title' => t('Configure Springboard Groups'),
      'description' => t('Allows a user to designate entire content types as groupable.'),
    ),
    // Grants access to the native OG admin UI, which we have re-permissioned
    // access to using hook_menu_alter, to hide it becasue it's confusing.
    // You need this permission to access it rather than
    // 'administer group'.
    'access og default admin ui' => array(
      'title' => t('Access Organic Groups Default Administration Screens'),
      'description' => t('Access organic groups default configuration pages which are normally simplified and overridden by the Springboard Groups administration pages.'),
    ),

    // Access the Springboard Menu items.
    'access springboard groups dashboard' => array(
      'title' => t('Access Springboard Groups Dashboard'),
      'description' => t('Access Springboard Groups Dashboard. Group Admins and Group members who are not admins should have, at least, this permission.'),
    ),

    // @todo remove this.
    'assign content to any springboard group' => array(
      'title' => t('Assign content to any springboard group'),
      'description' => t('Assign content to any springboard group, even if not a group member.'),
    ),
  );
}

/**
 * Functions for when a new group type is created.
 */

/**
 * Implements hook_og_permission().
function springboard_group_og_permission() {
  return array(
    'subscribe' => array(
      'title' => t('Subscribe user to group'),
      'description' => t("Allow user to be a member of a group (approval required)."),
      // Determine to which role to limit the permission. For example the
      // "subscribe" can't be assigned only to a non-member, as a member doesn't
      // need it.
      'roles' => array(OG_ANONYMOUS_ROLE),
      // Determine to which roles the permissions will be enabled by default.
      'default role' => array(OG_ANONYMOUS_ROLE),
    ),
  );
}
 */

/**
 * Implements hook_og_permission_alter().
 *
 * Remove the option for a non-member to request a subscription on the
 * group node page.
 */
function springboard_group_og_permission_alter(&$perms) {
  $perms['subscribe']['default role'] = array();
  $perms['unsubscribe']['default role'] = array();
}

/**
 * Implements hook_og_user_access_alter().
 *
 * Make groups that user is not subscribed to appear in the dropdown selector
 * if they have the correct permission.
 */
function springboard_group_og_user_access_alter(&$perm, $context) {
  if (user_access('assign content to any springboard group')) {
    $perm['update any page content'] = TRUE;
    $perm['administer group'] = TRUE;
  }
}
