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
function webform_goals_single_context_fields(&$form, &$form_state, $goal = array()) {
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

function webform_goals_group_context_fields(&$form, &$form_state, $goal = array()) {
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
 * Settings callback for the form group webform goals context.
 *
 * @param array $form
 *   The form parent element to add additional context settings.
 * @param array $goal
 *   The goal array.
 */
function webform_goals_form_group_context_fields(&$form, &$form_state, $goal = array()) {

  dsm($form_state);

  $types = webform_variable_get('webform_node_types');
  if (empty($form_state['storage']['all_webform_nodes'])) {
    $form_state['storage']['all_webform_nodes'] = _webform_goals_list_webforms();
  }

  $available_options = $form_state['storage']['all_webform_nodes'];
  $selected_options = array();

  if (!$form_state['submitted'] && isset($goal['extra']['selected_form_ids'])) {
    $selected_nids = $goal['extra']['selected_form_ids'];
  }
  elseif (!empty($form_state['values']['selected_form_ids'])) {
    $selected_nids = $form_state['values']['selected_form_ids'];
  }
  else {
    $selected_nids = array();
  }

  foreach ($selected_nids as $selected_nid) {
    $selected_options[$selected_nid] = $selected_nid;
  }

  if (!empty($form_state['values']['selected_form_types'])) {
    $selected_types = array_keys(array_filter($form_state['values']['selected_form_types']));
  }
  else {
    $selected_types = array();
  }

  $filtered_available_options = array();
  foreach($selected_types as $type) {
    $filtered_available_options[$type] = $available_options[$type];
  }

  $form['form_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form group builder'),
    '#collapsible' => FALSE,
    '#prefix' => '<div id="form-group-builder">',
    '#suffix' => '</div>',
  );

  $form['form_group']['available_forms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available forms'),
    '#collapsible' => FALSE,
  );

  $form_types = array();
  foreach ($form_state['storage']['all_webform_nodes'] as $type => $nodes) {
    $form_types[$type] = $type;
  }

  $form['form_group']['available_forms']['selected_form_types'] = array(
    '#type' => 'checkboxes',
    '#options' => $form_types,
    '#title' => t('Filter by type'),
    '#ajax' => webform_goals_form_group_builder_ajax('webform_goals_form_group_builder_available_forms_element'),
  );

  $header = array(
    'nid' => 'NID',
    'title' => 'Title',
    'type' => 'Type',
    'machine_type' => 'Machine type',
  );

  if (!empty($form_state['clicked_button']['#value']) && $form_state['clicked_button']['#value'] == 'Add selected') {
    $checked_available_forms = array_keys(array_filter($form_state['values']['available_forms']));

    foreach ($checked_available_forms as $checked_available_form) {
      // Add the option to the right side.
      $selected_options[$checked_available_form] = '(' . $checked_available_form . ') ' . $checked_available_form;
      // Remove it from the left side.
//      unset($available_options[$checked_available_form]);
    }

  }
  elseif (!empty($form_state['clicked_button']['#value']) && $form_state['clicked_button']['#value'] == 'Remove selected') {
//    $checked_selected_forms = array_keys(array_filter($form_state['values']['selected_forms']));
//
//    // remove checked options from the right side.
//    foreach ($checked_selected_forms as $checked_selected_form) {
//      unset($selected_options[$checked_selected_form]);
//
//      // if the form matches the filter criteria, add it back to the left side.
//      // if the checked selected form type is in the $types array then add it back to the left side.
//      if (in_array($form_state['storage']['all_webform_nodes'][$checked_selected_form]['machine_type'], $types)) {
//        $available_options[$checked_selected_form] = $form_state['storage']['all_webform_nodes'][$checked_selected_form];
//      }
//    }
  }

  // Left side.
  $form['form_group']['available_forms']['available_forms'] = array(
    '#type' => 'select',
    '#size' => 10,
    '#multiple' => TRUE,
    '#title' => 'Available',
    '#options' => $filtered_available_options,
    '#default_value' => array(),
  );

  $form['form_group']['available_forms']['add_selected'] = array(
    '#type' => 'button',
    '#value' => 'Add selected',
    '#ajax' => webform_goals_form_group_builder_ajax('webform_goals_form_group_add_selected'),
  );

  $form['form_group']['selected_forms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Forms in group'),
    '#collapsible' => FALSE,
  );

  $items = array();
  foreach ($selected_options as $id => $option) {
    $items[] = '(' . $id . ') ' . $option;
  }
  // Right side.
  $form['form_group']['selected_forms']['selected_forms'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
//    '#header' => $header,
//    '#multiple' => TRUE,
//    '#options' => $selected_options,
//    '#default_value' => array(),
    '#title' => 'Selected forms.',
  );
/*
  $form['form_group']['selected_forms']['remove_selected'] = array(
    '#type' => 'button',
    '#value' => 'Remove selected',
    '#ajax' => webform_goals_form_group_builder_ajax('webform_goals_form_group_remove_selected'),
  );
*/
  $form['form_group']['selected_form_ids'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#size' => 10,
    '#title' => 'Forms',
    '#empty_option' => t('-- select --'),
    '#empty_value' => 0,
    '#required' => TRUE,
    '#description' => t('Select the forms that will contribute to this goal.'),
    '#options' => _webform_goals_list_webforms(),
    '#default_value' => !empty($goal['extra']['selected_form_ids']) ? $goal['extra']['selected_form_ids'] : 0,
    '#ajax' => array(
      'callback' => 'webform_goals_extra_callback',
      'wrapper' => 'context-metrics',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

//  $form['mockup'] = array(
//    '#theme' => 'image',
//    '#path' => drupal_get_path('module', 'webform_goals') . '/mockup.png',
//  );
}

function webform_goals_form_group_builder_ajax($callback) {
  return array(
    'callback' => $callback,
    'effect' => 'fade',
    'method' => 'replace',
    'wrapper' => 'form-group-builder',
  );
}

function webform_goals_form_group_remove_selected($form, $form_state) {
  $messages = theme('status_messages');
  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace("#form-group-builder", $messages . render($form['context_container']['options_wrapper']['options']['form_group'])),
    ),
  );
}

function webform_goals_form_group_add_selected($form, $form_state) {
  $messages = theme('status_messages');
  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace("#form-group-builder", $messages . render($form['context_container']['options_wrapper']['options']['form_group'])),
    ),
  );
}

function webform_goals_form_group_builder_available_forms_element($form, $form_state) {
  $messages = theme('status_messages');
  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace("#form-group-builder", $messages . render($form['context_container']['options_wrapper']['options']['form_group'])),
    ),
  );
}

/**
 * Implements hook_webform_goals_list_context().
 *
 * Provide group, form group, and single contexts.
 */
function webform_goals_webform_goals_list_context() {
  $context['group'] = array(
    'id' => 'group',
    'name' => t('Group by type'),
    'settings_callback' => 'webform_goals_group_context_fields',
    'description' => t('Goal progress is calculated by adding metrics for all forms of the given type.'),
  );

  $context['form_group'] = array(
    'id' => 'form_group',
    'name' => t('Form group'),
    'settings_callback' => 'webform_goals_form_group_context_fields',
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
      // fall through
    case 'group':
      // fall through
    case 'form_group':
      // fall through
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

  if ($settings['context'] === 'form_group') {
    $settings['extra']['selected_form_ids'] = $form_state['values']['selected_form_ids'];
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
    elseif ($goal['context'] == 'form_group') {
      $data['count'] = webform_goals_count_submissions_by_form_ids($goal['extra']['selected_form_ids'], $goal['start_date'], $goal['end_date']);
    }
    // TODO: add case for global with provided nid.
    else {
      // TODO: calculate submissions across group.
      $data['count'] = _webform_goals_total_submissions($goal['extra']['selected_node_type'], $goal['start_date'], $goal['end_date']);
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

/**
 * Counts the number of submissions across the given array of form IDs.
 *
 * @param array $nids
 *   An array of nids (form IDs)
 * @param string|bool $start
 *   Start timestamp for form submissions.
 * @param string|bool $end
 *   End timestamp for form submissions.
 *
 * @return int
 *   Number of the submissions.
 */
function webform_goals_count_submissions_by_form_ids($nids, $start = FALSE, $end = FALSE) {
  $query = db_select('webform_submissions', 'w');
  $query->fields('w', array('sid'));
  $query->condition('nid', $nids, 'IN');
  if ($start) {
    $query->condition('submitted', $start, '>=');
  }
  if ($end) {
    $query->condition('submitted', $end, '<=');
  }
  $num_rows = $query->countQuery()->execute()->fetchField();

  return $num_rows;
}

/**
 * Counts form submissions on a single form.
 *
 * @param int $nid
 *   Single form ID
 * @param string|bool $start
 *   Start timestamp for submissions.
 * @param string|bool $end
 *   End timestamp for submissions.
 *
 * @return int
 *   Number of submissions.
 */
function _webform_goals_count_submissions($nid, $start = FALSE, $end = FALSE) {
  return webform_goals_count_submissions_by_form_ids(array($nid), $start, $end);
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
  $result = $query->execute()->fetchField();
  return $result ? $result : 0;
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
