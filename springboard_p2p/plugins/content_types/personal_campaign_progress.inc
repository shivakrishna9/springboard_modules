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
  $personal_campaign = springboard_p2p_get_personal_campaign_from_context_or_url($context);

  if (is_object($personal_campaign)) {
    $campaign = springboard_p2p_get_campaign_from_personal_campaign($personal_campaign);
    if (is_object($campaign)) {

      // Build the content type block.
      $block = new stdClass();
      $block->title = 'Personal campaign progress';
      $block->content = springboard_p2p_personal_campaign_progress_box($campaign, $personal_campaign);;

      return $block;

    }
  }

}

/**
 * Settings form.
 */
function springboard_p2p_personal_campaign_progress_edit_form($form, &$form_state) {
  // No settings beyond context, which has already been handled.
  return $form;
}
