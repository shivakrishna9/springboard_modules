i<?php

/**
 * @file Admin screens.
 */

function webform_goals_admin($form, $form_state = array()) {
  $goals = webform_goal_load_multiple();
  $rows = array();
  foreach ($goals as $goal) {
    // get progress
    $data = webform_goals_track_metric($goal);

    $row = array();
    $row[] = $goal['name'];
    $row[] = $goal['context'];
    $row[] = $goal['target_value']; //TODO: for single context add link to parent node or node title
    $row[] = $data['count']; // TODO amount: circle back once metrics tracking code is in play.
    $row[] = $data['progress']; // TODO percentage: circle back once metrics tracking code is in play.
    $row[] = _webform_goals_actions($goal);
    $rows[] = $row;
  }

  $form['goal_filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filters'),
  );
  // TODO: add filtering?
  $header = array(
    t('Name'),
    t('Context'),
    t('Goal'),
    t('Amount'),
    t('Progress'),
    t('Actions'),
  );
  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(),
    'colgroups' => array(),
    'caption' => '',
    'sticky' => FALSE,
    'empty' => '',
  );

  $form['goal_list'] = array(
    '#type' => 'markup',
    '#markup' => theme_table($vars),
  );
  $form['add'] = array(
    '#type' => 'button',
    '#value' => t('Add a goal'),
  );
  return $form;
}

function _webform_goals_actions($goal) {
  $output = '';
  $links[] = l(t('Edit'), 'admin/config/content/webform_goals/' . $goal['gid'] . '/edit');
  $links[] = l(t('Delete'), 'admin/config/content/webform_goals/' . $goal['gid'] . '/delete');
  $links[] = l(t('Embed code'), 'admin/config/content/webform_goals/' . $goal['gid'] . '/embed');

  foreach ($links as $link) {
    $output .= $link . '&nbsp;&nbsp;'; // lame
  }
  return $output;
}