<?php
/**
 * @file
 * springboard_p2p.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function springboard_p2p_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'register for springboard p2p'.
  $permissions['register for springboard p2p'] = array(
    'name' => 'register for springboard p2p',
    'roles' => array(
      'anonymous user' => 'anonymous user',
      'authenticated user' => 'authenticated user',
    ),
    'module' => 'springboard_p2p',
  );

  return $permissions;
}
