<?php

/**
 * @file
 * Builds placeholder replacement tokens for line item related data.
 */

/**
 * Implements hook_token_info_alter().
 */
function fundraiser_designations_token_info_alter(&$data) {
  if (!empty($data['tokens']['donation'])) {
    $data['tokens']['donation']['designations-line-items'] = array(
      'name' => t('Designations'),
      'description' => t('Designations selected.'),
    );
  }
}

/**
 * Implements hook_tokens_alter().
 */
function fundraiser_designations_tokens_alter(&$replacements, $context) {

  if ($context['type'] == 'donation' && !empty($context['data']['donation'])) {
    $donation = $context['data']['donation'];
  }

  if (!empty($donation) && !empty($donation->node) && fundraiser_designations_is_designation_type($donation->node->type)) {
    $order = !empty($donation->order) ? $donation->order : commerce_order_load($donation->did);
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

    $designations = [];
    foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
      $designations[$line_item_wrapper->line_item_label->value()]['quantity'] = $line_item_wrapper->quantity->value();
      $designations[$line_item_wrapper->line_item_label->value()]['amount'] = $line_item_wrapper->commerce_unit_price->value();
      $designations[$line_item_wrapper->line_item_label->value()]['pid'] = $line_item_wrapper->commerce_product->product_id->value();
    }

    $yadda = theme('fundraiser_designations_line_item_token', array('line_items' => $designations, 'donation' => $donation));
    $data['designations-line-items'] = $yadda;

    foreach ($context['tokens'] as $key => $token) {
      if (isset($data[$key])) {
        $replacements[$token] = $data[$key];
      }
    }
  }

}

/**
 * Theme the line item token output.
 */
function theme_fundraiser_designations_line_item_token($vars) {
  $list = [];
  $vars['donation']->node->fd_addon['pid'];
  $symbol = $vars['donation']->donation['currency']['symbol'];
  foreach ($vars['line_items'] as $name => $item) {
    if ($item['pid'] == $vars['donation']->node->fd_addon['pid']) {
      $name = t('One time add-on donation - @name', array('@name' => $name));
    }
    $list[] = $name . ': quantity: ' . (int) $item['quantity'] . ', total: ' . $symbol . commerce_currency_amount_to_decimal($item['quantity'] * $item['amount']['amount'], $item['amount']['currency_code']) . '.';
  }
  return theme('item_list', array('items' => $list, 'title' => t('Designated Funds')));
}