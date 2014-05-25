<?php

/**
 * @file
 * Plugin to display a personal campaign's progress.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('Personal campaign progress'),
  'description' => t('Displays the P2P personal campaign progress and goal.'),
  'category' => t('Springboard P2P'),
  // Despite having no "settings" we need this function to pass back a form,
  // or we'll loose the context and title settings.
  'edit form' => 'springboard_p2p_personal_campaign_progress_edit_form',
  'render callback' => 'springboard_p2p_personal_campaign_progress_render',
  'required context' => new ctools_context_required(t('Node'), 'node'),
);

/**
 * Render a single content pane.
 */
function springboard_p2p_personal_campaign_progress_render($subtype, $conf, $panel_args, $context) {
  $campaign = NULL;
  $personal_campaign = NULL;
  if (empty($context) || empty($context->data)) {
    if (isset($_GET['p2p_pcid']) && is_numeric($_GET['p2p_pcid'])) {
      $node = node_load($_GET['p2p_pcid']);
      if ($node->type == 'p2p_personal_campaign' && isset($node->field_p2p_campaign[$node->language][0]['target_id'])) {
        $campaign = node_load($node->field_p2p_campaign[$node->language][0]['target_id']);
        $personal_campaign = $node;
      }
      else {
        return;
      }
    }
    else {
      return;
    }
  }

  $node = $context->data;

  if ($node->type == 'p2p_personal_campaign' && isset($node->field_p2p_campaign[$node->language][0]['target_id'])) {
    $campaign = node_load($node->field_p2p_campaign[$node->language][0]['target_id']);
    $personal_campaign = $node;
  }

  if (!is_null($campaign) && !is_null($personal_campaign)) {
    $output = array();
    $language = $personal_campaign->language;
    $goal_type = springboard_p2p_get_goal_type_from_campaign($campaign);

    $progress_field = $personal_campaign->field_p2p_campaign_progress[$language][0];
    $progress_with_units = springboard_p2p_format_progress($progress_field, $goal_type);

    if ($goal_type == 'submissions' && isset($personal_campaign->field_p2p_personal_submit_goal[$language][0]['value'])) {
      $submissions_goal_value = $personal_campaign->field_p2p_personal_submit_goal[$language][0]['value'];
      $percent = springboard_p2p_format_percent($progress_field['submissions'], $submissions_goal_value, FALSE, FALSE);
      $progress_text = t('@progress to date', array('@progress' => $progress_with_units));
    }
    elseif ($goal_type == 'amount' && isset($personal_campaign->field_p2p_personal_campaign_goal[$language][0]['value'])) {
      $amount_goal_value = $personal_campaign->field_p2p_personal_campaign_goal[$language][0]['value'];
      $percent = springboard_p2p_format_percent($progress_field['amount'], $amount_goal_value, FALSE, FALSE);
      $progress_text = t('@progress raised to date', array('@progress' => $progress_with_units));
    }
    else {
      return;
    }

    $output[] = '<i class="fa fa-calendar amount-icon"></i>';

    $output[] = '<div class="personal-campaign-progress-container">';
    $output[] = '<div class="personal-campaign-progress">' . $progress_text . '</div>';
    $output[] = '<div class="progress-bar-wrapper">';
    $output[] = '<div class="progress-bar" style="width:' . $percent . '%"></div>';
    $output[] = '</div>';

    $output[] = '</div>';

    // Build the content type block.
    $block = new stdClass();
    $block->title = 'Personal campaign progress';
    $block->content = implode('', $output);
    return $block;
  }
}

/**
 * Settings form.
 */
function springboard_p2p_personal_campaign_progress_edit_form($form, &$form_state) {
  // No settings beyond context, which has already been handled.
  return $form;
}
