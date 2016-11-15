<?php

/**
 * Implements hook_ctools_plugin_api().
 */
function springboard_group_ctools_plugin_api($module = NULL, $api = NULL) {
  // Needed for automatic field group installation on Springboard Group
  // content type.
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
  // Needed for automatic panels installation on Springboard Group
  // content type.
  if ($module == "page_manager" && $api == "pages_default") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_ctools_plugin_directory().
 *
 * Extend Class OgSelectionHandler, so that admins can get groups
 * they aren't a member of displayed in the default group selector instead
 * of the "groups you aren't a member of" selector that is default with OG.
 */
function springboard_group_ctools_plugin_directory($module, $plugin) {
  $ret = NULL;
  if ($module == 'entityreference') {
    $ret = "plugins/entityreference/$plugin";
  }
  return $ret;
}

/**
 * Implements hook_ctools_plugin_post_alter().
 *
 * Allow entity reference fields to reference unpublished items.
 */
function springboard_group_ctools_plugin_post_alter(&$plugin, &$info) {
  if (
    $plugin['plugin module'] == 'entityreference'
    && $plugin['plugin type'] == 'selection'
    && $plugin['class'] == 'EntityReference_SelectionHandler_Generic'
  ) {
    $plugin = array(
      'class' => 'SpringboardGroup_EntityReference_SelectionHandler_Generic',
      'module' => 'springboard_group',
      'path' => drupal_get_path('module', 'springboard_group') . '/plugins/entityreference/selection',
      'file' => 'SpringboardGroup_EntityReference_SelectionHandler_Generic.class.php',
    ) + $plugin;
  }

  // Make the create content links pane recognize sb groups field name.
  if ($info['type'] == 'content_types' && $plugin['render callback'] == 'og_node_create_links_content_type_render') {
    $plugin['render callback'] = 'springboard_group_og_node_create_links_content_type_render';
  }
}

/**
 * Make the create content links pane recognize sb groups field name.
 *
 * Overrides og_node_create_links_content_type_render().
 *
 */
function springboard_group_og_node_create_links_content_type_render($subtype, $conf, $args, $context) {
  if (empty($context->data)) {
    return FALSE;
  }
  $node = $context->data;
  // This is where we alter og version.
  $types = !empty($conf['types']) ? $conf['types'] : array_keys(node_type_get_types());
  $links = [];
  foreach ($types as $type) {
    if ($context->data->type == 'springboard_group') {
      // Use our custom field name if springboard group.
      $links[] = og_node_create_links('node', $node->nid, 'og_group_ref_' . substr($type, 0, 19));
    }
    else {
      $links[] = og_node_create_links('node', $node->nid, $conf['field_name'], NULL, !empty($conf['types']) ? $conf['types'] : NULL);
    }
  }
  if (empty($links)) {
    return FALSE;
  }
  // Alteration end.
  $module = 'og';
  $block = new stdClass();
  $block->module = $module;
  $block->title = t('Content create links');

  $block->content = $links;
  return $block;
}
