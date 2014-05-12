<?php
/**
 * @file
 * springboard_p2p_landing.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_p2p_landing_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'p2p_referenced_campaigns';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'P2P Referenced Campaigns';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['css_class'] = 'campaign-landing-view';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['class'] = 'campaign-landing-grid';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'sorry';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Relationship: Entity Reference: Referencing entity */
  $handler->display->display_options['relationships']['reverse_field_p2p_referenced_campaigns_node']['id'] = 'reverse_field_p2p_referenced_campaigns_node';
  $handler->display->display_options['relationships']['reverse_field_p2p_referenced_campaigns_node']['table'] = 'node';
  $handler->display->display_options['relationships']['reverse_field_p2p_referenced_campaigns_node']['field'] = 'reverse_field_p2p_referenced_campaigns_node';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = 'h2';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Upload a Campaign Image */
  $handler->display->display_options['fields']['field_p2p_campaign_images']['id'] = 'field_p2p_campaign_images';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['table'] = 'field_data_field_p2p_campaign_images';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['field'] = 'field_p2p_campaign_images';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['element_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['settings'] = array(
    'image_style' => 'campaign_image_landing',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_p2p_campaign_images']['delta_limit'] = '1';
  $handler->display->display_options['fields']['field_p2p_campaign_images']['delta_offset'] = '0';
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['relationship'] = 'reverse_field_p2p_referenced_campaigns_node';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'p2p_campaign' => 'p2p_campaign',
  );

  /* Display: Content pane */
  $handler = $view->new_display('panel_pane', 'Content pane', 'panel_pane_1');
  $export['p2p_referenced_campaigns'] = $view;

  return $export;
}
