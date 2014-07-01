<?php

/**
 * @file
 * Plugin to display the P2P campaign expiration message.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'title' => t('P2P campaign expiration message'),
  'description' => t('Displays the P2P campaign expiration message'),
  'category' => t('Springboard P2P'),
  // Despite having no "settings" we need this function to pass back a form,
  // or we'll loose the context and title settings.
  'render callback' => 'springboard_p2p_campaign_expiration_message_render',
);

/**
 * Render a single content pane.
 */
function springboard_p2p_campaign_expiration_message_render($subtype, $conf, $panel_args, $context) {

  $message = NULL;

  if (isset($_GET['p2p_cid']) && is_numeric($_GET['p2p_cid'])) {
    $node = node_load($_GET['p2p_cid']);
    if ($node->type == 'p2p_campaign') {
      $wrapper = entity_metadata_wrapper('node', $node);
      $message = $wrapper->field_p2p_expiration_message->value();
    }
  }

    // Build the content type block.
    $block = new stdClass();
    $block->title = '';
    $block->content = $message;
    return $block;

}
