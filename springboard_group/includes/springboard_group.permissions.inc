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
  if (user_access('assign content to any springboard group')) {
    $perm['update any page content'] = TRUE;
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
    $help = array(
      'help_message' => array(
        '#markup' => t("Administrator members inherit all permissions granted to members."),
        '#weight' => -10,
        '#prefix' => '<br /><p>',
      ),
    );
    array_unshift($page['content'], $help);
  }
}