<?php
/**
 * @file
 * Setup for the "Legacy Table" view display style.
 */

/**
 * Implements hook_views_plugins().
 */
function springboard_p2p_personal_campaign_views_plugins() {
  return array(
    'style' => array(
      'legacy_table' => array(
        'title' => t('Legacy Table'),
        'help' => t('Displays a table using legacy markup.'),
        'handler' => 'views_plugin_style_table',
        'theme' => 'views_view_table',
        'uses row plugin' => FALSE,
        'uses row class' => TRUE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help topic' => 'style-table',
        'name' => 'table',
      ),
    ),
  );
}

/**
 * Implements hook_views_post_render().
 * 
 * Alter output to display legacy markup if the plugin display style is "legacy_table".
 */
function springboard_p2p_views_post_render(&$view, &$output, &$cache) {
  if (!isset($view->style_plugin->plugin_name) || $view->style_plugin->plugin_name != 'legacy_table') {
    return;
  }
  $output = '<i class="fa fa-user donor-icon"></i>' . $output;
  $output = preg_replace('/<table class="views-table.*?>/', '<table class="personal-campaign-roll">', $output);
  $output = preg_replace('/<a.*?sort.*?>(.*?)<\/a>/', '$1', $output);
  $output = preg_replace('/class="views-field.*?"/', 'class="p2p-recent-donors-field"', $output);
  $output = preg_replace('/<th class="p2p-recent-donors-field".*?>(.*?)<\/th>/s', '<td class="p2p-recent-donors-header">$1</td>', $output);
  $output = preg_replace('/<tr class="odd.*?"/', '<tr class="personal-campaign-roll-donor odd"', $output);
  $output = preg_replace('/<tr class="even.*?"/', '<tr class="personal-campaign-roll-donor even"', $output);
}
