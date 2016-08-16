<?php
/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_p2p_personal_campaign_views_default_views() {
  $views = array();

  // Begin view p2p_personal_campaign_recent_optin_donors
  $view = new view();
  $view->name = 'p2p_personal_campaign_recent_optin_donors';
  $view->description = 'Displays recent donors who opted in for public display of their name, filtered by the current personal campaign page.';
  $view->tag = 'default';
  $view->base_table = 'springboard_p2p_personal_campaign_action';
  $view->human_name = 'P2P Personal Campaign Recent Opt-In Donors';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Recent Donors';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'mini';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'action_type' => 'action_type',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'action_type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: P2P action: Donor Name */
  $handler->display->display_options['fields']['donor_name']['id'] = 'donor_name';
  $handler->display->display_options['fields']['donor_name']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['fields']['donor_name']['field'] = 'donor_name';
  $handler->display->display_options['fields']['donor_name']['label'] = 'Name';
  /* Field: P2P action: Action amount */
  $handler->display->display_options['fields']['amount']['id'] = 'amount';
  $handler->display->display_options['fields']['amount']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['fields']['amount']['field'] = 'amount';
  $handler->display->display_options['fields']['amount']['label'] = 'Amount';
  /* Sort criterion: P2P action: Date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: P2P action: P2P personal campaign */
  $handler->display->display_options['arguments']['personal_campaign_nid']['id'] = 'personal_campaign_nid';
  $handler->display->display_options['arguments']['personal_campaign_nid']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['arguments']['personal_campaign_nid']['field'] = 'personal_campaign_nid';
  $handler->display->display_options['arguments']['personal_campaign_nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['personal_campaign_nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['personal_campaign_nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['personal_campaign_nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['personal_campaign_nid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: P2P action: Action type */
  $handler->display->display_options['filters']['action_type']['id'] = 'action_type';
  $handler->display->display_options['filters']['action_type']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['filters']['action_type']['field'] = 'action_type';
  $handler->display->display_options['filters']['action_type']['value'] = 'fundraiser';
  /* Filter criterion: P2P action: Show Name */
  $handler->display->display_options['filters']['show_name']['id'] = 'show_name';
  $handler->display->display_options['filters']['show_name']['table'] = 'springboard_p2p_personal_campaign_action';
  $handler->display->display_options['filters']['show_name']['field'] = 'show_name';
  $handler->display->display_options['filters']['show_name']['value'] = '1';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');

  $views[$view->name] = $view;
  // End view p2p_personal_campaign_recent_optin_donors


  
  return $views;
}
