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
  if (empty($context) || empty($context->data)) {
    if (isset($_GET['pcid']) && is_numeric($_GET['pcid'])) {
      $node = node_load($_GET['pcid']);
      if ($node->type == 'p2p_personal_campaign' && isset($node->field_p2p_campaign[$node->language][0]['target_id'])) {
        $campaign = node_load($node->field_p2p_campaign[$node->language][0]['target_id']);
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

  if ($node->type == 'p2p_campaign') {
    $campaign = $node;
  }
  elseif ($node->type == 'p2p_personal_campaign' && isset($node->field_p2p_campaign[$node->language][0]['target_id'])) {
    $campaign = node_load($node->field_p2p_campaign[$node->language][0]['target_id']);
  }

  if (!is_null($campaign)) {
    // Build the content type block.
    $block = new stdClass();
    $block->title = 'Campaign header';
    $block->content = springbaord_p2p_format_campaign_form_header($campaign);
    return $block;
  }
}

/**
 * Settings form
 */
function springboard_p2p_personal_campaign_progress_edit_form($form, &$form_state) {
  // No settings beyond context, which has already been handled.
  return $form;
}
