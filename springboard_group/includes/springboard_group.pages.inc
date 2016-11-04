<?php
/**
 * @file
 * Menu callback functions.
 */

/**
 * Bulk add users to a group.
 *
 * @param string $group_type
 *   Always node.
 * @param int $group_id
 *   The group ID.
 *
 * @return mixed
 *   An empty string if permissions are lacking, the bulk add view
 *   if permissions exist.
 */
function springboard_group_bulk_add_page($group_type, $group_id) {

  if (!user_access('administer group')) {
    drupal_set_message(t('You need to have access to administer group in
      order to add users to groups. Please configure the needed permission'),
      'error');
    return '';
  }
  else {
    $content = '';
    if (is_numeric($group_id)) {
      //og_context($group_type, node_load($group_id));
      og_set_breadcrumb($group_type, $group_id, array(l(t('Group'), "$group_type/$group_id/group")));
      $view = views_get_view('springboard_group_bulk_add');
      $view->set_arguments(array($group_id));
      $view->set_display('block_1');
      $view->override_url = $_GET['q'];
      $content = $view->preview();
    }
  }
  return $content;
}