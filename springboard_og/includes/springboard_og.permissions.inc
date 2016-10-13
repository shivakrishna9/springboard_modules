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
function springboard_og_og_permission() {
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
function springboard_og_og_permission_alter(&$perms) {
  $perms['subscribe']['default role'] = array();
  $perms['unsubscribe']['default role'] = array();
}

/**
 * Implements hook_og_user_access_alter().
 *
 * Make groups that user is not subscribed to appear in the dropdown selector
 * if they have the correct permission.
 */
function springboard_og_og_user_access_alter(&$perm, $context) {
  if (user_access('Administer all springboard groups')) {
    $perm['update any page content'] = TRUE;
    $perm['administer group'] = TRUE;
  }
}