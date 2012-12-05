<?php
/**
 * @file
 */

/**
 * Implementation of hook_node_info().
 */
function email_confirmation_node_info() {
  $items = array(
    'confirmation_template' => array(
      'name' => t('Confirmation Template'),
      'module' => 'features',
      'description' => t('Create a template for your donation confirmation emails.'),
      'has_title' => '1',
      'title_label' => t('Internal Title'),
      'has_body' => '1',
      'body_label' => t('HTML Template'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
