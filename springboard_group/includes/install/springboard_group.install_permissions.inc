<?php

/**
 * Install the Affilaite editor role and default
 * permissions for groups.
 */
function _springboard_group_install_permissions() {

  $roles = array('Affiliate editor');
  foreach ($roles as $role) {
    if (!user_role_load_by_name($role)) {
      $role = new stdClass();
      $role->name = $role;
      user_role_save($role);
    }
  }

  $permissions = _springboard_group_install_permissions_default();
  $roles = [];
  $roles[] = user_role_load_by_name('Administrator');
  $roles[] = user_role_load_by_name('Springboard administrator');
  $roles[] = user_role_load_by_name('Springboard editor');
  $roles[] = user_role_load_by_name('Affiliate editor');
  foreach ($roles as $role) {
    foreach ($permissions as $name => $perm) {
      if (isset($perm['roles'][$role->name])) {
        user_role_grant_permissions($role->rid, array($name));
      }
    }
  }
}


/**
 * @return array
 */
function _springboard_group_install_permissions_default() {

  $permissions = array();

  $permissions['clone node'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['access contextual links'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['access draggableviews'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['use text format filtered_html'] = array(
    'name' => 'use text format filtered_html',
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['use text format full_html'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['access toolbar'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['create url aliases'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['access springboard dashboard'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['configure webform emails'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['configure webform settings'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
    ),
  );
  $permissions['access springboard groups dashboard'] = array(
    'roles' => array(
      'Affiliate editor' => 'Affiliate editor',
      'Administrator' => 'Administrator',
      'Springboard admin' => 'Springboard admin',
      'Springboard editor' => 'Springboard editor',
    ),
  );
  $permissions['configure springboard groups'] = array(
    'roles' => array(
      'Administrator' => 'Administrator',
      'Springboard admin' => 'Springboard admin',
    ),
  );
  return $permissions;
}
