<?php


function springboard_group_views_query_alter(&$view, &$query) {

  $alter = array('sbv_forms', 'sbv_pagewrappers', 'sbv_assets', 'webform_ab_tests');
  if (in_array($view->name, $alter)) {

//    $grouped = springboard_group_views_query_check_content_type($query);
//    if ($grouped) {
    $context = springboard_group_get_group_context();
    $user_gids = og_get_groups_by_user(NULL, 'node');
    $condition = db_and();

    if ($context == '_none' || $context == '_all') {
      if ($context == '_none') {
        $condition->isNull('og_membership.gid');
      }
      else {
        $all_groups = og_get_all_group();
        $condition->condition('og_membership.gid', $all_groups, 'IN');
      }
    }

    $query->add_table('og_membership');
    $query->add_where(0, $condition);
  }
  // }
}

function springboard_group_views_query_check_content_type($query) {
  $is_grouped = FALSE;
  foreach ($query->where as $where) {
    foreach ($where['conditions'] as $condition) {
      if ($condition['field'] == 'node.type') {
        if (!is_array($condition['value'])) {
          $is_grouped = og_get_group_type('node', $condition['value'], 'group content');
        }
        elseif (count($condition['value'] == 1)) {
          $is_grouped = og_get_group_type('node', reset($condition['value']), 'group content');
        }
        else {
          if (user_access('administer content')) {
            drupal_set_message('multipe node types in views query', 'error');
          }
        }
        //continue;
      }
    }
  }
  return $is_grouped;
}

function springboard_group_views_pre_execute(&$view) {
  //dpq($view->build_info['query']);
}
