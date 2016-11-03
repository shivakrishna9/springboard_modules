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
      'description' => t('Access the Springboard Groups administration screens.'),
    ),

    // Access the Springboard Menu items.
    'access springboard groups dashboard' => array(
      'title' => t('Access Springboard Groups Dashboard'),
      'description' => t('View a list of all groups.'),
    ),

    // Grants access to the native OG admin UI, which we have re-permissioned
    // access to using hook_menu_alter, to hide it becasue it's confusing.
    // You need this permission to access it rather than
    // 'administer group'.
    'access og default admin ui' => array(
      'title' => t('Access Organic Groups Default Administration Screens'),
      'description' => t("Springboard Groups hides OG's default admin UI. This permission will unhide it."),
    ),

  );
}

/**
 * Implements hook_og_permission().
 */
function springboard_group_og_permission() {
//  return array(
////    'edit payment method' => array(
////      'title' => t('Edit payment methods'),
////      'description' => t("User must have administer payment method permission in Springboard."),
////      'roles' => array(OG_AUTHENTICATED_ROLE),
////      'default role' => array(),
////    ),
//  );
}

/**
 * Implements hook_og_permission_alter().
 */
function springboard_group_og_permission_alter(&$perms) {
  $perms['subscribe']['default role'] = array();
  $perms['unsubscribe']['default role'] = array();
  $perms['administer group']['default role'] = array();
  $perms['approve and deny subscription']['default role'] = array();
}

/**
 * Implements hook_og_user_access_alter().
 *
 * Make groups that user is not subscribed to appear in the dropdown selector
 * if they have the correct permission.
 */
function springboard_group_og_user_access_alter(&$perm, $context) {
  if (user_access('administer group')) {
    // @todo don't need this probably.
    $perm['administer group'] = TRUE;
  }
}


/**
 * Implements hook_preprocess_TAG().
 *
 * Alter the groups permission page. Remove subscription permissions, rename
 * some items.
 */
function springboard_group_preprocess_og_ui_admin_permissions(&$vars) {

  if ($vars['form']['bundle']['#value'] != 'springboard_group') {
    return;
  }

  // Array of permissions we remove from the UI because we don't need them.
  $omitted_perms = array(
    'manage roles',
    'manage permissions',
    'administer group',
    'subscribe',
    'subscribe without approval',
    'unsubscribe',
    'approve and deny subscription',
  );

  $form = $vars['form'];
  foreach (element_children($form['permission']) as $key) {
    if (is_numeric($key)) {
      // Rename the categories.
      switch ($vars['form']['permission'][$key]['#id']) {
        case 'og':
          $vars['form']['permission'][$key]['#markup'] = t('Springboard Groups Content');
          break;

        case 'og_ui':
          $vars['form']['permission'][$key]['#markup'] = t('Springboard Groups Members');
          break;

        case 'publishcontent':
          $vars['form']['permission'][$key]['#markup'] = t('Content Publication Status');
          break;
      }
    }
    else {
      // Remove unneeded perms.
      if (in_array($key, $omitted_perms)) {
        unset($vars['form']['permission'][$key]);
      }
      foreach (element_children($form['checkboxes']) as $rid) {
        if (in_array($key, $omitted_perms)) {
          unset($vars['form']['checkboxes'][$rid][$key]);
        }
      }
      if ($key == 'update group') {
        $vars['form']['permission'][$key]['#description'] = t('Edit the group title, address, contact info, etc.');
      }
    }
  }
}

/**
 * Add a help message to the top of the permission overview page.
 *
 * Called from springboard_group_page_alter().
 */
function springboard_group_permissions_overview_page(&$page) {
  if (!empty($page['content']['system_main']['#form_id']) &&
    $page['content']['system_main']['#form_id'] == 'og_ui_admin_global_permissions'
  ) {

    $strict = variable_get('og_node_access_strict', TRUE);
    if ($strict) {
      $strict_message = t("Strict access control is enabled.");
    }
    else {
      $strict_message = t("Strict access control is not enabled.");
    }

    $bypass = springboard_group_check_role_conflicts();

    $bypass_message = '';
    if (!empty($bypass)) {
      $bypass = implode(', ', $bypass);
      $bypass_message = t('The following Springboard roles have "bypass content access control" permission and will not be affected by these settings: @bypass', array('@bypass' => $bypass));
    }

    $help_message = t('More information about permissions is available on the help tab');
    $message = t("Administrator members inherit all permissions granted to members.");
    $help = array(
      'help_message' => array(
        '#markup' => $message . '<br />' . $strict_message . '<br />' . $help_message,
        '#weight' => -10,
        '#prefix' => '<br /><p>',
      ),
    );
    array_unshift($page['content'], $help);
  }

  if (!empty($page['content']['system_main']['#form_id']) &&
    $page['content']['system_main']['#form_id'] == 'user_admin_permissions'
  ) {
    springboard_group_check_role_conflicts();
    if (current_path() == 'admin/springboard/groups/settings/springboard-permissions') {
      $help_message = t('More information about permissions is available on the help tab');
      $help = array(
        'help_message' => array(
          '#markup' => t('This is a subset of the global permissions normally found on the !link page which are relevant to Springboard Groups.', array('!link' => l(t('user permissions'), 'admin/people/permissions'))) . '<br />' . $help_message,
          '#weight' => -10,
          '#prefix' => '<br /><p>',
        ),
      );
      array_unshift($page['content'], $help);
    }
  }
}

/**
 * Check for "bypass content access control" conflicts.
 *
 * @return array
 *   Roles with by permission.
 */
function springboard_group_check_role_conflicts() {
  $roles = user_roles();
  $role_permissions = user_role_permissions($roles);
  $bypass = [];
  $administer = [];
  foreach ($role_permissions as $rid => $permissions) {
    foreach ($permissions as $key => $permission) {
      if ($key == 'bypass node access' && $permission == 1) {
        $bypass[$rid] = $roles[$rid];
      }
      if ($key == 'administer group' && $permission == 1) {
        $administer[$rid] = $roles[$rid];
      }
    }
  }

  $problem = array_diff($bypass, $administer);
  if (!empty($problem)) {
    $count = count($problem);
    $word = format_plural($count, 'role', 'roles');
    $bad_roles = implode(', ', $problem);
    drupal_set_message(t('Warning! Some roles have the "bypass content access control" permission, but not the "administer organic groups" permissions. This will cause problems. Please fix the following @word: @bad_roles.', array(
      '@word' => $word,
      '@bad_roles' => $bad_roles,
    )), 'error');
  }
  return $bypass;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add the js which hides non-group permissions.
 */
function springboard_group_form_user_admin_permissions_alter(&$form, $form_state) {
  if (isset($form['permission']['administer group'])) {
    $form['permission']['administer group']['#markup'] = ' Administer Organic groups.';
    $form['permission']['administer group']['#description'] = 'Administer and post in all groups.';
  }

  foreach ($form['permission'] as $key => $perm) {
    if (strpos($key, 'springboard_group') !== FALSE) {
      $form['permission'][$key]['#markup'] = str_replace('content', 'Springboard Group', $form['permission'][$key]['#markup']);
    }
  }
  if (arg(1) == 'springboard' && arg(2) == 'groups') {
    $path = drupal_get_path('module', 'springboard_group') . '/js/springboard-groups-permissions.js';
    $form['#attached']['js'][] = $path;
  }
}