<?php

/**
 * Add quick goals settings form to advocacy node forms.
 *
 * @param array $form
 *    Form array passed from node_form_alter.
 * @param array $form_state
 *    Form array passed from node_form_alter.
 */
function springboard_advocacy_goals_form(array &$form, array $form_state) {
  $goal_config = variable_get('springboard_advocacy_goal_settings', array());
  $gid = isset($form['#node']->nid) && isset($goal_config[$form['#node']->nid]) ? $goal_config[$form['#node']->nid] : FALSE;
  $goal = !empty($gid) ? webform_goals_goal_retrieve($gid) : array();
  $form['goal_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Goal settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['goal_settings']['springboard_advocacy_goal_id'] = array(
    '#type' => 'value',
    '#value' => $gid,
  );
  $form['goal_settings']['springboard_advocacy_goal_target'] = array(
    '#type' => 'textfield',
    '#title' => t('How many signatures are required to meet this goal?'),
    '#default_value' => isset($goal['target_value']) ? $goal['target_value'] : '',
  );
  $form['goal_settings']['springboard_advocacy_goal_minimum'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum number of signatures required to show progress.'),
    '#default_value' => isset($goal['display_threshold']) ? $goal['display_threshold'] : '',
  );
  $form['goal_settings']['springboard_advocacy_starter_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Starter message'),
    '#description' => t('This message is displayed when minimum goal progress has not been met.'),
    '#default_value' => isset($goal['starter_message']) ? $goal['starter_message'] : '',
  );

  $form['#validate'][] = 'springboard_advocacy_goal_save_validate';
  // Actual submission is handled in hook_node_insert and hook_node_update.
  // This ensures that a node id is present whenever we act on a form
  // submission, a condition that is not guaranteed if we rely
  // on hook_form_alter().
}


/**
 * Form validation callback for petition goal settings fieldset.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function springboard_advocacy_goal_save_validate(array $form, array $form_state) {

  // Goal target is NaN.
  if (!empty($form_state['values']['springboard_advocacy_goal_target']) && !is_numeric($form_state['values']['springboard_advocacy_goal_target'])) {
    form_set_error('springboard_advocacy_goal_target', t('Target number of submissions must be a number.'));
  }
  // Goal minimum is NaN.
  if (!empty($form_state['values']['springboard_advocacy_goal_minimum']) && !is_numeric($form_state['values']['springboard_advocacy_goal_minimum'])) {
    form_set_error('springboard_advocacy_goal_minimum', t('Minimum number of submissions must be a number.'));
  }
  // Missing goal target but other settings are present.
  if (empty($form_state['values']['springboard_advocacy_goal_target']) && (!empty($form_state['values']['springboard_advocacy_goal_minimum']) || !empty($form_state['values']['springboard_advocacy_starter_message']))) {
    form_set_error('springboard_advocacy_goal_target', t('Please include number of signatures required to meet this goal.'));
  }
  // Display minimum is larger than the target.
  if (!empty($form_state['values']['springboard_advocacy_goal_target']) && !empty($form_state['values']['springboard_advocacy_goal_minimum'])) {
    if ($form_state['values']['springboard_advocacy_goal_target'] <= $form_state['values']['springboard_advocacy_goal_minimum']) {
      form_set_error('springboard_advocacy_goal_minimum', t('The display minimum supplied would prevent goal progress from ever being displayed. Please supply a value that is less than the target number of submissions.'));
    }
  }
}


/**
 * Save petition goal settings.
 *
 * @param object $node
 *    The node object.
 */
function springboard_advocacy_goal_save_submit($node) {
  if (!empty($node->springboard_advocacy_goal_target)) {
    $settings = array(
      'gid' => isset($node->springboard_advocacy_goal_id) ? $node->springboard_advocacy_goal_id : '',
      'name' => 'Springboard petition goal for node ' . $node->nid,
      'context' => 'single',
      'metric' => 'submission_count',
      'extra' => array('selected_form_id' => $node->nid),
      'display_threshold' => $node->springboard_advocacy_goal_minimum,
      'seed' => 0,
      'target_value' => $node->springboard_advocacy_goal_target,
      'starter_message' => $node->springboard_advocacy_starter_message,
      'starter_message_format' => 'filtered_html',
      'start_date' => 0,
      'end_date' => 0,
      'template' => '[goal:progress-starter]
<p>The current goal is [goal:target]. We have [goal:total] thus far. That’s nearly [goal:progress-percent]% of our goal! Thanks for your support.</p>', // we may need configurable global defaults here.
      'template_format' => 'full_html',
      'progress_bar' => 'flat_ui',
    );
    $goal = $settings['gid'] ? webform_goals_goal_update($settings) : webform_goals_goal_create($settings);
    $goal_settings = variable_get('', array());
    $goal_settings[$node->nid] = $goal['gid'];
    variable_set('springboard_advocacy_goal_settings', $goal_settings);
  }
  elseif (!empty($node->springboard_advocacy_goal_id)) {
    // Node updated with no goal target, delete goal if exists.
    springboard_advocacy_goal_delete($node);
  }
}


/**
 * Delete goal settings.
 *
 * @param object $node
 *    The node object.
 */
function springboard_advocacy_goal_delete($node) {
  $goal_config = variable_get('springboard_advocacy_goal_settings', array());
  $gid = isset($goal_config[$node->nid]) ? $goal_config[$node->nid] : FALSE;
  if ($gid) {
    unset($goal_config[$node->nid]);
    variable_set('springboard_advocacy_goal_settings', $goal_config);
    webform_goals_goal_delete($gid);
  }
}
