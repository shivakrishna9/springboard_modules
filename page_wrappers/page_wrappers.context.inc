<?php

/**
 * Implements hook_ctools_plugin_api().
 */
function page_wrappers_ctools_plugin_api($module, $api) {
  if ($module == 'context' && $api == 'plugins') {
    return array('version' => 3);
  }
}

/**
 * Implements hook_context_plugins().
 */
function page_wrappers_context_plugins() {
  $plugins = array();

  $plugins['context_reaction_page_wrappers'] = array(
    'handler' => array(
      'path' => drupal_get_path('module', 'page_wrappers') . '/plugins',
      'file' => 'context_reaction_page_wrappers.inc',
      'class' => 'context_reaction_page_wrappers',
      'parent' => 'context_reaction',
    ),
  );

  return $plugins;
}

/**
 * Implements hook_context_registry().
 */
function page_wrappers_context_registry() {
  $registry = array();

  $registry['reactions'] = array(
    'page_wrappers' => array(
       'title' => t('Page wrappers'),
       'description' => t('Change the page wrapper.'),
       'plugin' => 'context_reaction_page_wrappers'
    )
  );
  return $registry;
}

/**
 * Implements hook_context_page_reaction().
 */
function page_wrappers_context_page_reaction() {
  if ($plugin = context_get_plugin('reaction', 'page_wrappers')) {
    $plugin->execute();
  }
}


/**
 * Find and load the current node
 *
 * @return Object
 *    A fully loaded node if this is a page request for node, else false
 */
function _page_wrappers_context_page_node() {
  // get out of here if we're not on a node page, or if we're on the node edit page
  if (arg(0) != 'node' || (arg(0) == 'node' && arg(2) == 'edit')) {
    return;
  }
  $node = NULL;
  //if this is a node page load it up, also cover webform 'done' page
  if (arg(0) == 'node' && is_numeric(arg(1)) && (arg(2) == 'done' || !arg(2))) {
    $node = node_load(arg(1));
    return $node;
  }

  return FALSE;
}
