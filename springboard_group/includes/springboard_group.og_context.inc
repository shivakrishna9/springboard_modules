<?php
/**
 * @file
 * Determine what group we are working in based on page or node context.
 */

/**
 * Implements hook_og_context_negotiation_info().
 *
 * Fires off a context negotiator on non-node advocacy paths, so we can figure
 * out what group we're currently working in.
 */
function springboard_group_og_context_negotiation_info() {
  $providers = array();
  $providers['springboard_group'] = array(
    'name' => t('Springboard Groups'),
    'description' => t("Determine context for Springboard Groups"),
    'callback' => 'springboard_group_og_context_handler',
    'menu path' => springboard_group_context_menu_paths(),
  );

  return $providers;
}

/**
 * OG_context handler.
 *
 * On non-node pages, figure out the current group context, or set it.
 */
function springboard_group_og_context_handler($item = NULL) {

  $path = current_path();
  unset($_SESSION['og_context']);
  if (strpos($path, '/edit') !== FALSE && strpos($path, 'advocacy/target' !== FALSE)) {
    // @todo get the group from the target ID.
    //return array('node' => array($gid);
  }
  global $user;
  $ret = array();
  if ($user->uid) {
    if (user_access('administer group')) {
      $groups = og_get_all_group();
    }
    else {
      $groups = og_get_groups_by_user(NULL, 'node');
    }
    if (!empty($groups)) {
      // If the user only belongs to one group, return it.
      if (count($groups) === 1) {
        $gid = reset($groups);
        $ret = array('node' => array($gid));
      }

      // If the user has multiple groups and there is not a currently selected
      // working group, set the first group as the working group, and set
      // springboard_group_context_multiple so that the dashboard knows
      // we need a selector.
      elseif (!isset($_SESSION['springboard_group_working_group'])) {
        $_SESSION['springboard_group_context_multiple'] = array_values($groups);
        $gid = reset($groups);
        $_SESSION['springboard_group_working_group'] = $gid;
        $ret = array('node' => array($gid));
      }
      elseif ($_SESSION['springboard_group_working_group'] == '_none') {
        $_SESSION['springboard_group_context_multiple'] = array_values($groups);
        $ret = array('_none' => array('_none'));
      }
      else {
        // There's already a working group, return it as the context and
        // tell the dashboard we need a selector.
        $_SESSION['springboard_group_context_multiple'] = array_values($groups);
        $ret = array('node' => array($_SESSION['springboard_group_working_group']));
      }
    }
  }
  return $ret;
}

/**
 * Iterative getter of group IDs.
 *
 * This should only be called from contexts where you are expecting a single
 * group ID to be returned - i.e., non-node pages and nodes on which only
 * a single group selection is allowed (advocacy).
 *
 * @return mixed
 */
function springboard_group_get_group_context() {

  $gid = NULL;

  // First check if we're on a grouped node.
  // BTW, this only happens (so far) in an administrative context,
  // so no big deal performance-wise.
  if (arg(0) == 'node') {
    $node = menu_get_object();
    if (!empty($node->nid) && !empty($node->vid) && !og_is_group('node', $node->nid)) {
      $wrapper = entity_metadata_wrapper('node', $node);
      foreach ($node as $key => $property) {
        if (strpos($key, 'og_group_ref') !== FALSE) {
          $group = $wrapper->$key->value();
          $gid = !empty($group[0]->nid) ? $group[0]->nid : FALSE;
        }
      }
      // GID can be false if the node is not grouped.
      return $gid;
    }
    // @todo - deal with node/add.
  }


  // This would have been set by
  // springboard_group_og_context_handler().
  $group_context = og_context('node');
  if (!empty($group_context['gid'])) {
    return $group_context['gid'];
  }

  // "_none".
  if (!empty($_SESSION['springboard_group_working_group'])) {
    return $_SESSION['springboard_group_working_group'];
  }

  // If all else fails, try this.
  if (!empty($_SESSION['og_context']['gid']) && $_SESSION['og_context']['group_type'] == 'node') {
    return $_SESSION['og_context']['gid'];
  }
  return $gid;
}

function springboard_group_context_menu_paths() {
  $paths = array(
    'admin/commerce/config/payment-methods',
    'admin/springboard',
    'admin/springboard/asset-library',
    'admin/content/webform_ab',
    'admin/springboard/asset-library/page_wrapper',
    'admin/springboard/asset-library/email_wrapper',
    'admin/springboard/donation-forms/all',
    'admin/springboard/forms/all',
  );
  $paths = array_merge($paths, module_invoke_all('springboard_group_context_menu_paths'));
  return $paths;
}