<?php



/**
 * Implements hook_permission().
 */
function springboard_og_permission() {
  return array(
    'administer springboard groups' => array(
      'title' => t('Administer Springboard Groups'),
      'description' => t('Perform administration tasks for Springboard Groups.'),
    ),

    'access all springboard groups' => array(
      'title' => t('Access All Springboard Groups'),
      'description' => t('Access all groups'),
    ),
  );
}

/**
 * Functions for when a new group type is created.
 */

/**
 * Implements hook_og_permission().
 */
//function springboard_og_og_permission() {
//  return array(
//    'subscribe' => array(
//      'title' => t('Subscribe user to group'),
//      'description' => t("Allow user to be a member of a group (approval required)."),
//      // Determine to which role to limit the permission. For example the
//      // "subscribe" can't be assigned only to a non-member, as a member doesn't
//      // need it.
//      'roles' => array(OG_ANONYMOUS_ROLE),
//      // Determine to which roles the permissions will be enabled by default.
//      'default role' => array(OG_ANONYMOUS_ROLE),
//    ),
//  );
//}

/**
 * Implements hook_og_permission_alter().
 */
function springboard_og_og_permission_alter(&$perms) {
  // remove non-member subscribe permission.
  $perms['subscribe']['default role'] = array();
  $perms['unsubscribe']['default role'] = array();
}

/**
 * Implements hook_og_default_roles_alter().
 */
function springboard_og_og_default_roles_alter(&$roles) {
 // currently array(0 => 'administrator member');
}
