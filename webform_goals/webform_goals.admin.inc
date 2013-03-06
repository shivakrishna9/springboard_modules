i<?php

/**
 * @file Admin screens.
 */

function webform_goals_admin_list($form, $form_state = array()) {
  $goals = webform_goal_load_multiple();
  $rows = array();
  foreach ($goals as $goal) {
    // get progress
    $selected_context = _webform_goals_get_selected_context($form_state, $goal);
    $selected_metric = _webform_goals_get_selected_metric($selected_context, $form_state, $goal);
    $data = webform_goals_track_metric($goal);
    if ($selected_context['id'] == 'single') {
      $node = node_load($goal['extra']['selected_form_id']);
      $selected_context['name'] .= ' : ' . l($node->title, 'node/' . $node->nid);
    }
    elseif ($selected_context['id'] == 'group') {
      $selected_context['name'] .= ' : ' . $goal['extra']['selected_node_type'];
    }
    $row = array();
    $row[] = $goal['name'];
    $row[] = $selected_context['name'];
    $row[] = $selected_metric['name'];
    $row[] = $goal['target_value']; //TODO: for single context add link to parent node or node title
    $row[] = $data['count']; // TODO amount: circle back once metrics tracking code is in play.
    $row[] = $data['percent'] . '%';
    $row[] = webform_goals_goal_status($goal);
    $row[] = _webform_goals_actions($goal);
    $rows[] = $row;
  }

  /*
  $form['goal_filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filters'),
  );*/
  // TODO: add filtering?
  $header = array(
    t('Name'),
    t('Context'),
    t('Metric'),
    t('Goal'),
    t('Amount'),
    t('Progress'),
    t('Status'),
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
    '#type' => 'submit',
    '#value' => t('Add a goal'),
  );
  $form['#submit'][] = 'webform_goals_admin_redirect';
  return $form;
}

function webform_goals_admin_redirect($form, $form_state) {
  drupal_goto('admin/config/content/webform_goals/add');
}


function _webform_goals_actions($goal) {
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();
  $output = '';
  $links[] = l(t('Edit'), 'admin/config/content/webform_goals/' . $goal['gid'] . '/edit');
  $links[] = l(t('Delete'), 'admin/config/content/webform_goals/' . $goal['gid'] . '/delete');
  $options = array(
    'attributes' => array(
      'class' => array('ctools-use-modal ctools-modal-modal-popup-small'),
      'html' => TRUE,
    ),
  );
  $links[] = l(t('Embed code'), 'webform_goals/' . $goal['gid'] . '/embed', $options);

  foreach ($links as $link) {
    $output .= $link . '&nbsp;&nbsp;'; // lame
  }
  return $output;
}

function webform_goals_admin_settings($form, $form_state) {
  $default_template = theme('webform_goals_default_template', array());
  $form['cron'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update settings'),
  );
  $form['cron']['webform_goals_widget_update_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Goal update maximum'),
    '#description' => t('Calculating goal progress can be system intensive for large group goals. If goal progress is not updating in a reasonable timeframe try lowering this number.'),
    '#default_value' => variable_get('webform_goals_widget_update_limit', '50'),
  );
  return system_settings_form($form);
}