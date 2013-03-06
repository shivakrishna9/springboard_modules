<?php

/**
 * @file context and metrics hooks and associated helper functions.
 */

/**
 * Add additional settings "webform select field" to context UI when
 * the "single form" context is selected.
 *
 * @param array $form
 * Form parent element to add additional context settings.
 *
 * @param array $goal
 * Goal array.
 */
function webform_goals_single_context_fields(&$form, $goal = array()) {
  $form['selected_form_id'] = array(
    '#type' => 'select',
    '#title' => '',
    '#empty_option' => t('-- select --'),
    '#empty_value' => 0,
    '#required' => TRUE,
    '#description' => t('Select the form that will contribute to this goal.'),
    '#options' => _webform_goals_list_webforms(),
    '#default_value' => !empty($goal['extra']['selected_form_id']) ? $goal['extra']['selected_form_id'] : 0,
    '#ajax' => array(
      'callback' => 'webform_goals_extra_callback',
      'wrapper' => 'context-metrics',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
}

function webform_goals_group_context_fields(&$form, $goal = array()) {
  $form['selected_node_type'] = array(
    '#type' => 'select',
    '#title' => '',
    '#empty_option' => t('-- select --'),
    '#empty_value' => 0,
    '#required' => TRUE,
    '#description' => t('This goal will be assigned to all nodes of the type you select.'),
    '#options' => _webform_goals_get_webform_node_types(),
    '#default_value' => !empty($goal['extra']['selected_node_type']) ? $goal['extra']['selected_node_type'] : 0,
    '#ajax' => array(
      'callback' => 'webform_goals_extra_callback',
      'wrapper' => 'context-metrics',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
}

/**
 * Implements hook_webform_goals_list_context().
 *
 * Provide "global" and "single webform" contexts.
 */
function webform_goals_webform_goals_list_context() {
  $context['group'] = array(
    'id' => 'group',
    'name' => t('Group by type'),
    'settings_callback' => 'webform_goals_group_context_fields',
    'description' => t('Goal progress is calculated by adding metrics for all forms in the group.'),
  );
  $context['single'] = array(
    'id' => 'single',
    'name' => t('Single'),
    'settings_callback' => 'webform_goals_single_context_fields',
    'description' => t('This goal will only be applied to a single webform you select.'),
  );
   return $context;
}

/**
 * Implements hook_webform_goals_list_metrics().
 *
 * Add "# of submissions" and "custom field" metrics.
 */
function webform_goals_webform_goals_list_metrics($selected_context, $form_state, $goal = array()) {

  switch ($selected_context['id']) {
    case 'single':
      $options = array(0 => '- You must select a form for this context to be valid -');
      if (!empty($form_state['values']['selected_form_id'])) {
        $options = _webform_goals_field_list($form_state['values']['selected_form_id']);
      }
      elseif (!empty($goal['extra']['selected_form_id'])) {
        $options = _webform_goals_field_list($goal['extra']['selected_form_id']);
      }
      $elements['custom_field_selection'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => !empty($goal['extra']['custom_field_selection']) ? $goal['extra']['custom_field_selection'] : '',
      );

      $metrics['custom_field'] = array(
        'id' => 'custom_field',
        'name' => t('Custom field '),
        'description' => t('Goal progress will be calculated based on the sum of values submitted by this field.'),
        'settings_callback' => 'webform_goals_single_custom_fields',
        'child_elements' => $elements, // TODO: replace with callback.
        'form_text' => array(
          'target_value' => array(
            'title' => t('What is the total field value required to meet this goal?'),
            'description' => t('Goal progress represents the sum of all values submitted to the selected field as a percentage of this number.'),
          ),
          'seed' => array(
            'title' => t('Seed goal progress with this value'),
            'description' => t('Add this number to the current total when calculating goal progress.'),
          ),
          'display_threshold' => array(
            'title' => t('Minimum total values required to show progress'),
            'description' => t('The goal progress bar will be hidden until the total values in the selected field reach this number.'),
          ),
        ),
      );
    case 'group':
    default:
      $metrics['submission_count'] = array(
        'id' => 'submission_count',
        'name' => t('Number of form submissions'),
        'description' => t('Goal progress will be calculated based on the total number of times this webform has been submitted.'),
        'form_text' => array(
          'target_value' => array(
            'title' => t('How many submissions are required to meet this goal?'),
            'description' => t('Goal progress represents the total number of submissions for the selected form as a percentage of this number.'),
          ),
          'seed' => array(
            'title' => t('Seed goal progress with submissions'),
            'description' => t('Add this number to the current submissions when calculating goal progress.'),
          ),
          'display_threshold' => array(
            'title' => t('Minimum submissions required to show progress.'),
            'description' => t('The goal progress bar will be hidden until this number of submissions have been made to the selected webform.'),
          ),
        ),
      );
  }
  return $metrics;
}

/**
 * Implements hook_webform_goals_save_goal().
 *
 * Add additional required settings for select context and metrics choices.
 */
function webform_goals_webform_goals_save_goal($settings, $form_state) {
  if ($settings['context'] === 'single') {
    $settings['extra']['selected_form_id'] = $form_state['values']['selected_form_id'];
  }

  if ($settings['context'] === 'group') {
    $settings['extra']['selected_node_type'] = $form_state['values']['selected_node_type'];
  }

  if ($settings['metric'] === 'custom_field') {
    $settings['extra']['custom_field_selection'] = $form_state['values']['custom_field_selection'];
  }
  return $settings;
}


/**
 * Implements hook_webform_goals_track_metric().
 */
function webform_goals_webform_goals_track_metric($goal, $params = array()) {
  if ($goal['metric'] === 'submission_count') {
    if ($goal['context'] == 'single') {
      $data['count'] = _webform_goals_count_submissions($goal['extra']['selected_form_id'], $goal['start_date'], $goal['end_date']);

    }
    // TODO: add case for global with provided nid.
    else {
      // TODO: calculate submissions across group.
      $data['count'] = _webform_goals_total_submissions($goal['extra']['selected_node_type']);
    }
    webform_goals_calculate_progress($data, $goal);
    return $data;
  }

  if ($goal['metric'] === 'custom_field') {
    // this shouldn't be possible. custom field should be single webform only.
    if (empty($goal['extra']['selected_form_id'])) {
      $data['count'] = 'N/A';
      $data['progress'] = 'N/A';
    }
    else {
      $data['count'] = _webform_goals_total_component_values($goal['extra']['selected_form_id'], $goal['extra']['custom_field_selection'], $goal['start_date'], $goal['end_date']);
    }
    webform_goals_calculate_progress($data, $goal);
    return $data;
  }
}

function _webform_goals_count_submissions($nid, $start = FALSE, $end = FALSE) {
  $query = db_select('webform_submissions', 'w');
  $query->fields('w', array('sid'));
  $query->condition('nid', $nid, '=');
  if ($start) {
    $query->condition('submitted', $start, '>=');
  }
  if ($end) {
    $query->condition('submitted', $end, '<=');
  }
  $num_rows = $query->countQuery()->execute()->fetchField();

  return $num_rows;
}

function _webform_goals_total_submissions($node_type, $start = FALSE, $end = FALSE) {
  // SELECT COUNT(ws.sid)
  // FROM webform_submissions ws
  // INNER JOIN {node} n
  //   ON ws.nid = n.nid
  // WHERE n.type = :type
  $query = db_select('webform_submissions', 'ws');
  $query->join('node', 'n', 'ws.nid = n.nid');
  $query->addExpression('COUNT(ws.sid)');
  $query->condition('n.type', $node_type, '=');
  if ($start) {
    $query->condition('ws.submitted', $start, '>=');
  }
  if ($end) {
    $query->condition('ws.submitted', $end, '<=');
  }
  return $query->execute()->fetchField();
}

/**
 * Running total of all values submitted to a given webform component.
 */
function _webform_goals_total_component_values($nid, $form_key, $start = FALSE, $end = FALSE) {
  // SELECT SUM(wsd.data)
  // FROM webform_submitted_data wsd
  // JOIN webform_component wc
  //   ON wsd.nid = wc.nid
  //   AND wsd.cid = wc.cid
  // WHERE wsd.nid = 22
  //   AND wc.form_key = 'amount';
  $query = db_select('webform_submitted_data', 'wsd');
  $query->join('webform_component', 'wc', 'wsd.nid = wc.nid AND wsd.cid = wc.cid');
  if ($start || $end) {
    $query->join('webform_submissions', 'ws', 'ws.sid = wsd.sid');
  }
  $query->addExpression('SUM(data)', 'data');
  $query->condition('wsd.nid', $nid, '=');
  $query->condition('wc.form_key', $form_key, '=');
  // add conditionals for start/end date

  if ($start) {
    $query->condition('ws.submitted', $start, '>=');
  }
  if ($end) {
    $query->condition('ws.submitted', $end, '<=');
  }
  return $query->execute()->fetchField();
}

function _webform_goals_get_webform_node_types() {
  $webform_types = variable_get('webform_node_types');
  $result = db_query('
    SELECT
      type,
      name
    FROM {node_type}
    WHERE type IN (:types)
  ', array(':types' => $webform_types));
  while ($type = $result->fetchAssoc()) {
    $types[$type['type']] = $type['name'];
  }
  return $types;
}