<?php

/**
 * Implements hook_views_query_alter().
 *
 * Fix the bulk add view to display only non-members.
 *
 * Add a join for og_membership table on dashboard views when
 * the content type is in a group.
 */
function springboard_group_views_query_alter(&$view, &$query) {

  if ($view->name == 'springboard_group_bulk_add') {
    foreach ($query->where as $key => &$condition_group) {
      foreach ($condition_group['conditions'] as &$condition) {
        if (strpos($condition['field'], 'og_membership.gid') !== FALSE) {
          // Unset the where clause from the context filter, and
          // grab it's value and add it to a join instead.
          $gid = $condition['value'][':og_membership_gid'];
          unset($query->where[$key]);
          $join = new views_join();
          $join->table = 'og_membership';
          $join->field = 'etid';
          $join->left_table = 'users';
          $join->left_field = 'uid';
          $join->type = 'left';
          $join->extra = array(
            array(
              'field' => 'entity_type',
              'value' => 'user',
            ),
            array(
              'field' => 'gid',
              'value' => $gid,
            ),
          );
          $join->construct();
          $join->adjusted = TRUE;
          $query->add_relationship('og_membership', $join, 'user');
          $condition = db_and();
          $condition->isNull('og_membership.etid');
          $query->add_where(0, $condition);
        }
      }
    }
  }

  // Array of alterable views.
  $alter = variable_get('group_restricted_views', []);

  if (in_array($view->name, $alter)) {
    // First check if the query is selecting a groupable entity. If it isn't,
    // we skip the query alteration.
    //$grouped = springboard_group_views_query_check_content_type($query);

    // Get the current active group ID.
    $context = springboard_group_get_group_context();
    if (empty($context)) {
      // This gets sent to the query as the "gid", causing no result.
      $context = '_you_will_see_nothing';
    }
    // Build the query.
    $condition = db_and();
    if (($context == '_none' || $context == '_all') && (user_access('administer group') || user_access('bypass node access'))) {
      if ($context == '_none') {
        $condition->isNull('og_membership.gid');
      }
      else {
        if ($context == '_all') {
          $all_groups = springboard_group_get_all_springboard_groups();
          $condition->condition('og_membership.gid', $all_groups, 'IN');
          $condition->condition('og_membership.entity_type', 'node');
        }
      }
    }
    else {
      if ($context == '_all') {
        $context = $_SESSION['springboard_group_context_multiple'];
      }
      $condition->condition('og_membership.gid', $context);
      $condition->condition('og_membership.entity_type', 'node');
    }

    // Help views understand what we're doing. Adding the table without
    // the join explicitly defined makes the query messy.
    $def = array(
      'left_table' => 'node',
      'left_field' => 'nid',
      'table' => 'og_membership',
      'field' => 'etid',
    );
    $join = new views_join();
    $join->definition = $def;
    $join->extra = array(
      array(
        'field' => 'entity_type',
        'value' => 'node',
      ),
    );

    $join->construct();
    $join->adjusted = TRUE;

    $query->add_table('og_membership', 'node', $join);
    $query->add_where(0, $condition);

  }
}

/**
 * Check a query for groupable content.
 *
 * @param object $query
 *   The views query.
 *
 * @return bool
 *   Is the view querying groupable content, true or false.
 */
function springboard_group_views_query_check_content_type($query) {
  $is_grouped = FALSE;
  foreach ($query->where as $where) {
    foreach ($where['conditions'] as $condition) {
      if ($condition['field'] == 'node.type') {
        if (!is_array($condition['value'])) {
          $is_grouped = og_get_group_type('node', $condition['value'], 'group content');
        }
        elseif (count($condition['value']) == 1) {
          $is_grouped = og_get_group_type('node', reset($condition['value']), 'group content');
        }
        else {
          if (user_access('administer content')) {
            drupal_set_message('multipe node types in views query', 'error');
          }
          return TRUE;
        }
      }
    }
  }
  return $is_grouped;
}


function springboard_group_views_pre_execute(&$view) {
 // dpq($view->build_info['query']);
}

/**
 * Implements hook_views_default_views_alter().
 *
 * Add a group title field to springboard dashboard views.
 */
function springboard_group_views_default_views_alter(&$views) {

  $alter = array_filter(variable_get('group_restricted_views', []));
  foreach ($alter as $view_name => $value) {
    $displays = $views[$view_name]->display;
    foreach ($displays as $display_name => $display) {
      $handler =& $views[$view_name]->display[$display_name]->handler;
      /* Field: Content: Title */
      if (!empty($handler->display->display_options['fields'])) {
        $handler->display->display_options['relationships']['og_membership_related_node_group']['id'] = 'og_membership_related_node_group';
        $handler->display->display_options['relationships']['og_membership_related_node_group']['table'] = 'og_membership';
        $handler->display->display_options['relationships']['og_membership_related_node_group']['field'] = 'og_membership_related_node_group';
        $fields =& $handler->display->display_options['fields'];
        $group_field = [];
        $group_field['title_1']['id'] = 'title_1';
        $group_field['title_1']['table'] = 'node';
        $group_field['title_1']['field'] = 'title';
        $group_field['title_1']['label'] = 'Group';
        $group_field['title_1']['relationship'] = 'og_membership_related_node_group';
        array_splice($fields, 2, 0, $group_field);
      }
    }
  }

}
