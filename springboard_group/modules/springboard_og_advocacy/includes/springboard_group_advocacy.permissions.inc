<?php

/**
 * @file
 * Permissions related functions for group adovacy.
 */

/**
 * Implements hook_og_permission().
 */
function springboard_group_advocacy_og_permission() {
  return array(
    'administer advocacy group subscription' => array(
      'title' => t('Administer advocacy subscription'),
      'description' => t("Change the advocacy subscription level of a group."),
      'roles' => array(OG_AUTHENTICATED_ROLE),
      'default role' => array(),
    ),
    'view targets' => array(
      'title' => t('View Targets'),
      'description' => t('View Custom targets.'),
    ),
    'create targets' => array(
      'title' => t('Create Targets'),
      'description' => t('Create Custom targets.'),
    ),
    'edit targets' => array(
      'title' => t('Edit Targets'),
      'description' => t('Edit Custom my targets.'),
    ),
    'delete targets' => array(
      'title' => t('Delete Targets'),
      'description' => t('Custom targets.'),
    ),
    'view groups' => array(
      'title' => t('View Groups'),
      'description' => t('View Custom groups.'),
    ),
    'create groups' => array(
      'title' => t('Create Groups'),
      'description' => t('Create Custom groups.'),
    ),
    'edit groups' => array(
      'title' => t('Edit Groups'),
      'description' => t('Edit Custom groups.'),
    ),
    'delete groups' => array(
      'title' => t('Delete Groups'),
      'description' => t('Custom groups.'),
    ),
    'add target to action' => array(
      'title' => t('Add target to action.'),
      'description' => t('Add target to action.'),
    ),
    'add group to action' => array(
      'title' => t('Add group to action.'),
      'description' => t('Add group to action.'),
    ),
  );
}

/**
 * Check if user has target CRUD permissions in any Group.
 */
function springboard_group_advocacy_get_advocacy_group_perms($account) {

  $all_user_group_rids = array();
  $in_group_perms = array();

  // Get the user's groups.
  $groups = og_get_entity_groups('user', $account);
  // Get the user's roles in every group.
  if (!empty($groups['node'])) {
    foreach ($groups['node'] as $gid) {
      $all_user_group_rids += og_get_user_roles('node', $gid, $account->uid, TRUE, TRUE);
    }

    $advo_perms = array(
      'view targets',
      'create targets',
      'edit targets',
      'delete targets',
      'view groups',
      'create groups',
      'edit groups',
      'delete groups',
      'add target to action',
      'add group to action',
    );

    // Check the role permissions for target CRUD access.
    $perms = og_role_permissions($all_user_group_rids);
    foreach ($perms as $group_perm) {
      foreach ($group_perm as $key => $status) {
        if (in_array($key, $advo_perms)) {
          $in_group_perms[$key] = $status;
        }
      }
    }
  }

  return $in_group_perms;
}
