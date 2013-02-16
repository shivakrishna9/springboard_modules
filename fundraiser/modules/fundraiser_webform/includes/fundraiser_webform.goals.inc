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
      $metrics['fundraiser_donation_amount'] = array(
        'id' => 'fundraiser_donation_amount',
        'name' => t('Total donation amount'),
        'description' => t('Goal progress will be calculated based on the total donations received.'),
        'settings_callback' => '',
        'threshold_prefix' => t('Do not display the goal progress until a minimum of $'),
        'threshold_suffix' => t(' in donations has been received.'),
        'seed_prefix' => t('Add an initial $'),
        'seed_suffix' => t(' to the donation progress.'),
      );
      return $metrics;
    }
  }
}

/**
 * Implements hook_webform_goals_track_metric().
 */
function fundraiser_webform_webform_goals_track_metric($goal, $params = array()) {
  if ($goal['metric'] === 'fundraiser_donation_amount') {
    $data = array(
      'count' => '',
      'progress' => 0
    );
    // this shouldn't be possible
    if (!empty($goal['extra']['selected_form_id'])) {
      $data['count'] = fundraiser_get_total_donations_by_nid($goal['extra']['selected_form_id']);
      $data['progress'] = ($data['count'] / $goal['target_value']) * 100;
    }
    // Bet the database loves this.
    elseif (!empty($goal['extra']['selected_node_type'])) {
      $nids = _fundraiser_goals_nids_by_type($goal['extra']['selected_node_type']);
      if (is_array($nids)) {
        drupal_set_message('nids is array');
        foreach($nids as $nid) {
          $donations = fundraiser_get_total_donations_by_nid($nid);
          if ($donations) {
            $data['count'] += $donations;
          }
        }
        $data['count'] = number_format($data['count'], 2); // Alligator wrestling to keep format identical with single values.
        $data['progress'] = ($data['count'] / $goal['target_value']) * 100;
      }
    }
    return $data;
  }
}

/**
 * retrieve a list of nids by node type without invoking entity_load().
 */
function _fundraiser_goals_nids_by_type($type) {
  drupal_set_message('type:' . $type);
  static $nids = array();
  if (empty($nids[$type])) {
    $results = db_query('
      SELECT
        nid
      FROM {node}
      WHERE type = :type
    ', array(':type' => $type));
    while ($nid = $results->fetchField()) {
      $nids[$type][] = $nid;
    }
  }
  return $nids[$type];
}