<?php


/**
 * Implements hook_og_context_negotiation_info().
 *
 * Fires off a context negotiator on non-node advocacy paths, so we can figure
 * out what group we're currently working in.
 */
function springboard_og_advocacy_og_context_negotiation_info() {
  $providers = array();
  $providers['springboard_og_advocacy'] = array(
    'name' => t('Springboard OG Advocacy'),
    'description' => t("Determine context for API subscriptions"),
    'callback' => 'springboard_og_advocacy_og_context_handler',
    'menu path' => array(
      'admin/springboard/advocacy/targets',
    ),
  );

  return $providers;
}

/**
 * OG_context handler.
 *
 * On non-node pages, figure out the current group context, or set it.
 */
function springboard_og_advocacy_og_context_handler($item = NULL) {

  // @todo Alternate menu paths to the advocacy dashboard from group nodes with
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
      // springboard_og_context_multiple so that the dashboard knows we need a
      // selector.
      elseif (!isset($_SESSION['springboard_og_advocacy_working_group'])) {

        $_SESSION['springboard_og_context_multiple'] = array_values($groups['node']);
        $gid = reset($groups['node']);
        $_SESSION['springboard_og_advocacy_working_group'] = $gid;
        $ret = array('node' => array($gid));
      }
      else {
        // There's already a working group, return it as the context and
        // tell the dashboard we need a selector.
        $_SESSION['springboard_og_context_multiple'] = array_values($groups['node']);
        $ret = array('node' => array($_SESSION['springboard_og_advocacy_working_group']));
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
function springboard_og_advocacy_get_group_context() {

  $gid = NULL;

  // This would have been set by springboard_og_advocacy_og_context_handler().
  $group_context = og_context('node');
  if (!empty($group_context['gid'])) {
    return $group_context['gid'];
  }

  // @todo Sometimes group context is empty after ajax refresh on
  // advocacy search pages.  Figure out why.
  if (!empty($_SESSION['springboard_og_advocacy_working_group'])) {
    return $_SESSION['springboard_og_advocacy_working_group'];
  }

  // If all else fails, try this.
  if (!empty($_SESSION['og_context']['gid'] && $_SESSION['og_context']['group_type'] == 'node')) {
    return $_SESSION['og_context']['gid'];
  }

  return $gid;
}