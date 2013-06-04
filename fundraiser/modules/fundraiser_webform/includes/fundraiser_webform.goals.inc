<?php

/**
 * @file Webform Goals integration hooks and associated helper functions.
 */

/**
 * Implements hook_webform_goals_list_context().
 *
 * Provide custom tracking metrics and UI sugar for donation forms.
 */
function fundraiser_webform_webform_goals_list_metrics($selected_context, $form_state, $goal = array()) {

  // this metric should only be available to donation forms selected in a "single" context
  // or groups if the node type is also a donation node type.
  if ($selected_context['id'] == 'single' || $selected_context['id'] == 'group') {
    $is_fundraiser_goal = fundraiser_webform_is_fundraiser_goal($selected_context, $form_state, $goal);

    if ($is_fundraiser_goal) {
      $metrics['fundraiser_donation_amount'] = array(
        'id' => 'fundraiser_donation_amount',
        'name' => t('Total donation amount'),
        'description' => t('Goal progress will be calculated based on the total donations received.'),
        'settings_callback' => '',
        'form_text' => array(
          'target_value' => array(
            'title' => t('Total donation amount required to meet this goal'),
            'description' => t('Goal progress represents the sum of all successful donations as a percentage of this number.'),
          ),
          'seed' => array(
            'title' => t('Seed goal progress with this amount'),
            'description' => t('Add this number to the current total donations when calculating goal progress.'),
          ),
          'display_threshold' => array(
            'title' => t('Minimum total donations required to show progress'),
            'description' => t('The goal progress bar will be hidden until total donations reach this number.'),
          ),
        ),
      );
      return $metrics;
    }
  }
}

/**
 * Implements hook_webform_goals_track_metric().
 */
function fundraiser_webform_webform_goals_track_metric($goal, $params = array()) {
  $data = array(
    'count' => '',
  );
  if ($goal['metric'] === 'fundraiser_donation_amount') {

    if (!empty($goal['extra']['selected_form_id'])) {
      $data['count'] = fundraiser_get_total_donations_by_nid($goal['extra']['selected_form_id'], $goal['start_date'], $goal['end_date']);
    }
    // Bet the database loves this.
    elseif (!empty($goal['extra']['selected_node_type'])) {
      $nids = _webform_goals_nids_by_type($goal['extra']['selected_node_type']);
      if (is_array($nids)) {
        foreach($nids as $nid) {
          $donations = fundraiser_get_total_donations_by_nid($nid, $goal['start_date'], $goal['end_date']);
          if ($donations) {
            $data['count'] += $donations;
          }
        }
        $data['count'] = number_format((float) $data['count'], 2); // Alligator wrestling to keep format identical with single values.
      }
    }
    webform_goals_calculate_progress($data, $goal);
    return $data;
  }
}

function fundraiser_webform_is_fundraiser_goal($selected_context, $form_state, $goal = array()) {
   if ($selected_context['id'] == 'single' || $selected_context['id'] == 'group') {
    $type = '';
    if (!empty($goal['extra']['selected_form_id'])) {
      $nid = $goal['extra']['selected_form_id'];
    }
    else {
      $nid = !(empty($form_state['values']['selected_form_id'])) ? $form_state['values']['selected_form_id'] : 0;
    }

    if ($selected_context['id'] == 'single' && $nid) {
      $node = node_load($nid);
      $type = $node->type;
    }
    elseif ($selected_context['id'] == 'group') {
      if (isset($form_state['values']['selected_node_type'])) {
        $type = $form_state['values']['selected_node_type'];
      }
      elseif (!empty($goal['extra']['selected_node_type'])) {
        $type = $goal['extra']['selected_node_type'];
      }
    }

    if ($type && fundraiser_is_donation_type($type)) {
      return TRUE;
    }
  }
  return FALSE;
}
