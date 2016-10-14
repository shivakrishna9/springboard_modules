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
function springboard_group_advocacy_og_context_negotiation_info() {
  $providers = array();
  $providers['springboard_group_advocacy'] = array(
    'name' => t('Springboard OG Advocacy'),
    'description' => t("Determine context for API subscriptions"),
    'callback' => 'springboard_group_advocacy_og_context_handler',
    'menu path' => springboard_group_advocacy_context_menu_paths(),
  );

  return $providers;
}

/**
 * OG_context handler.
 *
 * On non-node pages, figure out the current group context, or set it.
 */
function springboard_group_advocacy_og_context_handler($item = NULL) {

  // @todo or not @todo: Alternate menu paths to the advocacy dashboard
  // from group nodes with
  // group unique permissions might be better/simpler than using
  // session negotiation when user has multiple groups.

  $path = current_path();
  unset($_SESSION['og_context']);
  if (strpos($path, '/edit') !== FALSE) {
    // @todo get the group from the target ID.
    //return array('node' => array($gid);
  }
  global $user;
  $ret = array();
  if ($user->uid) {
    $groups = og_get_groups_by_user();
    if (!empty($groups['node'])) {

      // If the user only belongs to one group, return it.
      if (count($groups['node']) === 1) {
        $gid = reset($groups['node']);
        $ret = array('node' => array($gid));
      }
      // If the user has multiple groups and there is not a currently selected
      // working group, set the first group as the working group, and set
      // springboard_group_context_multiple so that the dashboard knows
      // we need a selector.
      elseif (!isset($_SESSION['springboard_group_advocacy_working_group'])) {

        $_SESSION['springboard_group_context_multiple'] = array_values($groups['node']);
        $gid = reset($groups['node']);
        $_SESSION['springboard_group_advocacy_working_group'] = $gid;
        $ret = array('node' => array($gid));
      }
      else {
        // There's already a working group, return it as the context and
        // tell the dashboard we need a selector.
        $_SESSION['springboard_group_context_multiple'] = array_values($groups['node']);
        $ret = array('node' => array($_SESSION['springboard_group_advocacy_working_group']));
      }
    }
  }

  return $ret;
}

/**
 * Iterative getter of group IDs.
 *
 * @return mixed
 */
function springboard_group_advocacy_get_group_context() {

  $gid = NULL;

  // First check if we're on a grouped node.
  // BTW, this only happens (so far) in an administrative context,
  // so no big deal performance-wise.
  $node = menu_get_object();
  if (!empty($node->nid) && !empty($node->vid) && !og_is_group('node', $node->nid)) {
    $wrapper = entity_metadata_wrapper('node', $node);
    foreach ($node as $key => $property) {
      if (strpos($key, 'og_group_ref') !== FALSE) {
        $group = $wrapper->$key->value();
        $gid = $group->nid;
      }
    }
    if (!empty($gid)) {
      return $gid;
    }
  }

  // This would have been set by
  // springboard_group_advocacy_og_context_handler().
  $group_context = og_context('node');
  if (!empty($group_context['gid'])) {
    return $group_context['gid'];
  }

  // @todo Sometimes group context is empty after ajax refresh on
  // advocacy search pages. Figure out why.
  if (!empty($_SESSION['springboard_group_advocacy_working_group'])) {
    return $_SESSION['springboard_group_advocacy_working_group'];
  }

  // If all else fails, try this.
  if (!empty($_SESSION['og_context']['gid']) && $_SESSION['og_context']['group_type'] == 'node') {
    return $_SESSION['og_context']['gid'];
  }

  return $gid;
}


/**
 * Define the context paths for hook_og_context_negotiation_info().
 *
 * Also called from springboard_group_advocacy_preprocess_page(). Which is why
 * it's broken out into a helper, ATM.
 */
function springboard_group_advocacy_context_menu_paths() {
  return array(
    'admin/springboard/advocacy/target',
  );
}