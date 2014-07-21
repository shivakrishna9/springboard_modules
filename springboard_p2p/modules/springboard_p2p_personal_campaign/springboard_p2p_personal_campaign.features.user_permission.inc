<?php
/**
 * @file
 * springboard_p2p_personal_campaign.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function springboard_p2p_personal_campaign_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'create p2p_personal_campaign content'.
  $permissions['create p2p_personal_campaign content'] = array(
    'name' => 'create p2p_personal_campaign content',
    'roles' => array(),
    'module' => 'node',
  );

  // Exported permission: 'edit own p2p_personal_campaign content'.
  $permissions['edit own p2p_personal_campaign content'] = array(
    'name' => 'edit own p2p_personal_campaign content',
    'roles' => array(),
    'module' => 'node',
  );

  return $permissions;
}
