<?php

/**
 * Implements hook_views_query_alter().
 *
 * Add a join for og_membership table on dashboard views when
 * the content type is in a group.
 */
function springboard_group_views_query_alter(&$view, &$query) {
  // Array of alterable views.
  $alter = variable_get('group_restricted_views', []);

  if (in_array($view->name, $alter)) {
    // First check if the query is selecting a groupable entity. If it isn't,
    // we skip the query alteration.
    //$grouped = springboard_group_views_query_check_content_type($query);

    // Get the current active group ID.
    $context = springboard_group_get_group_context();
    if (!empty($context)) {
      // Build the query.
      $condition = db_and();
      if ($context == '_none' || $context == '_all') {
        if ($context == '_none') {
          $condition->isNull('og_membership.gid');
        }
        else {
          if ($context == '_all') {
            $all_groups = og_get_all_group();
            $condition->condition('og_membership.gid', $all_groups, 'IN');
            $condition->condition('og_membership.entity_type', 'node');
          }
        }
      }
      else {
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
//  dpq($view->build_info['query']);
}