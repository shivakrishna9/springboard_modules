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
  $view->name = 'p2p_campaign_landing_slideshow';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'P2P Campaign Landing Slideshow';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['css_class'] = 'p2p-campaign-landing-slideshow';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '4';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['class'] = 'slides';
  $handler->display->display_options['style_options']['wrapper_class'] = 'flexslider';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Relationship: Entity Reference: Referencing entity */
  $handler->display->display_options['relationships']['reverse_field_p2p_slide_show_node']['id'] = 'reverse_field_p2p_slide_show_node';
  $handler->display->display_options['relationships']['reverse_field_p2p_slide_show_node']['table'] = 'node';
  $handler->display->display_options['relationships']['reverse_field_p2p_slide_show_node']['field'] = 'reverse_field_p2p_slide_show_node';
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_p2p_slide_show_target_id']['id'] = 'field_p2p_slide_show_target_id';
  $handler->display->display_options['relationships']['field_p2p_slide_show_target_id']['table'] = 'field_data_field_p2p_slide_show';
  $handler->display->display_options['relationships']['field_p2p_slide_show_target_id']['field'] = 'field_p2p_slide_show_target_id';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['title']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Landing page slider */
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['id'] = 'field_p2p_campaign_slider';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['table'] = 'field_data_field_p2p_campaign_slider';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['field'] = 'field_p2p_campaign_slider';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['label'] = '';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['element_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['hide_empty'] = TRUE;
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_p2p_campaign_slider']['settings'] = array(
    'image_style' => 'campaign_image_slideshow',
    'image_link' => '',
  );
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = '';
  $handler->display->display_options['fields']['edit_node']['element_type'] = 'span';
  $handler->display->display_options['fields']['edit_node']['element_class'] = 'node-edit';
  $handler->display->display_options['fields']['edit_node']['element_label_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['edit_node']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_default_classes'] = FALSE;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['nid']['alter']['text'] = '<a href="/springboard_p2p/register?p2p_cid=[nid]" >Get Started</a>';
  $handler->display->display_options['fields']['nid']['element_type'] = 'span';
  $handler->display->display_options['fields']['nid']['element_class'] = 'slide-link';
  $handler->display->display_options['fields']['nid']['element_label_type'] = '0';
  $handler->display->display_options['fields']['nid']['element_wrapper_type'] = '0';
  /* Sort criterion: Content: Slide Show (field_p2p_slide_show:delta) */
  $handler->display->display_options['sorts']['delta']['id'] = 'delta';
  $handler->display->display_options['sorts']['delta']['table'] = 'field_data_field_p2p_slide_show';
  $handler->display->display_options['sorts']['delta']['field'] = 'delta';
  $handler->display->display_options['sorts']['delta']['relationship'] = 'field_p2p_slide_show_target_id';
  $handler->display->display_options['sorts']['delta']['order'] = 'DESC';
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['relationship'] = 'reverse_field_p2p_slide_show_node';
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
  /* Filter criterion: Content: Landing page slider (field_p2p_campaign_slider:fid) */
  $handler->display->display_options['filters']['field_p2p_campaign_slider_fid']['id'] = 'field_p2p_campaign_slider_fid';
  $handler->display->display_options['filters']['field_p2p_campaign_slider_fid']['table'] = 'field_data_field_p2p_campaign_slider';
  $handler->display->display_options['filters']['field_p2p_campaign_slider_fid']['field'] = 'field_p2p_campaign_slider_fid';
  $handler->display->display_options['filters']['field_p2p_campaign_slider_fid']['operator'] = 'not empty';

  /* Display: Slideshow */
  $handler = $view->new_display('panel_pane', 'Slideshow', 'panel_pane_1');
  $export['p2p_campaign_landing_slideshow'] = $view;

  $view = new view();
  $view->name = 'p2p_referenced_campaigns';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'P2P Campaign Landing Your P2P';
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
  $handler->display->display_options['pager']['options']['items_per_page'] = '4';
  $handler->display->display_options['pager']['options']['offset'] = '0';
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
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_p2p_referenced_campaigns_target_id']['id'] = 'field_p2p_referenced_campaigns_target_id';
  $handler->display->display_options['relationships']['field_p2p_referenced_campaigns_target_id']['table'] = 'field_data_field_p2p_referenced_campaigns';
  $handler->display->display_options['relationships']['field_p2p_referenced_campaigns_target_id']['field'] = 'field_p2p_referenced_campaigns_target_id';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = 'h2';
  $handler->display->display_options['fields']['title']['element_class'] = 'p2p-thumb-title';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['title']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Campaign thumbnail */
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['id'] = 'field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['table'] = 'field_data_field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['field'] = 'field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['label'] = '';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['settings'] = array(
    'image_style' => 'campaign_image_landing',
    'image_link' => '',
  );
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['max_length'] = '120';
  $handler->display->display_options['fields']['body']['alter']['trim'] = TRUE;
  $handler->display->display_options['fields']['body']['element_type'] = 'p';
  $handler->display->display_options['fields']['body']['element_label_type'] = '0';
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_wrapper_type'] = '0';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['nid']['alter']['text'] = '<a href="/springboard_p2p/register?p2p_cid=[nid]" >Get Started</a>';
  $handler->display->display_options['fields']['nid']['element_type'] = 'span';
  $handler->display->display_options['fields']['nid']['element_class'] = 'campaign-node-link';
  $handler->display->display_options['fields']['nid']['element_label_type'] = '0';
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nid']['element_wrapper_type'] = '0';
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = '';
  $handler->display->display_options['fields']['edit_node']['element_type'] = 'span';
  $handler->display->display_options['fields']['edit_node']['element_class'] = 'node-edit';
  $handler->display->display_options['fields']['edit_node']['element_label_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['edit_node']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_default_classes'] = FALSE;
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

  /* Display: Your P2P (Grid) */
  $handler = $view->new_display('panel_pane', 'Your P2P (Grid)', 'panel_pane_1');
  $handler->display->display_options['defaults']['empty'] = FALSE;
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Start your own campaign (field_p2p_referenced_campaigns) */
  $handler->display->display_options['sorts']['field_p2p_referenced_campaigns_target_id']['id'] = 'field_p2p_referenced_campaigns_target_id';
  $handler->display->display_options['sorts']['field_p2p_referenced_campaigns_target_id']['table'] = 'field_data_field_p2p_referenced_campaigns';
  $handler->display->display_options['sorts']['field_p2p_referenced_campaigns_target_id']['field'] = 'field_p2p_referenced_campaigns_target_id';
  $handler->display->display_options['sorts']['field_p2p_referenced_campaigns_target_id']['relationship'] = 'field_p2p_referenced_campaigns_target_id';
  $handler->display->display_options['sorts']['field_p2p_referenced_campaigns_target_id']['order'] = 'DESC';

  /* Display: Featured Personal */
  $handler = $view->new_display('panel_pane', 'Featured Personal', 'panel_pane_2');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'campaign-landing-featured-view';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '3';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['relationships'] = FALSE;
  /* Relationship: Entity Reference: Referencing entity */
  $handler->display->display_options['relationships']['reverse_field_p2p_feature_pers_campaigns_node']['id'] = 'reverse_field_p2p_feature_pers_campaigns_node';
  $handler->display->display_options['relationships']['reverse_field_p2p_feature_pers_campaigns_node']['table'] = 'node';
  $handler->display->display_options['relationships']['reverse_field_p2p_feature_pers_campaigns_node']['field'] = 'reverse_field_p2p_feature_pers_campaigns_node';
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_p2p_feature_pers_campaigns_target_id']['id'] = 'field_p2p_feature_pers_campaigns_target_id';
  $handler->display->display_options['relationships']['field_p2p_feature_pers_campaigns_target_id']['table'] = 'field_data_field_p2p_feature_pers_campaigns';
  $handler->display->display_options['relationships']['field_p2p_feature_pers_campaigns_target_id']['field'] = 'field_p2p_feature_pers_campaigns_target_id';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = 'h2';
  $handler->display->display_options['fields']['title']['element_class'] = 'p2p-thumb-title';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['title']['element_default_classes'] = FALSE;
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
  /* Field: Content: Campaign thumbnail */
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['id'] = 'field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['table'] = 'field_data_field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['field'] = 'field_p2p_campaign_thumbnail';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['label'] = '';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_label_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_p2p_campaign_thumbnail']['settings'] = array(
    'image_style' => 'campaign_image_landing',
    'image_link' => '',
  );
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['label'] = '';
  $handler->display->display_options['fields']['body']['alter']['max_length'] = '120';
  $handler->display->display_options['fields']['body']['alter']['trim'] = TRUE;
  $handler->display->display_options['fields']['body']['element_type'] = 'p';
  $handler->display->display_options['fields']['body']['element_label_type'] = '0';
  $handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['body']['element_wrapper_type'] = '0';
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = '';
  $handler->display->display_options['fields']['view_node']['element_type'] = 'span';
  $handler->display->display_options['fields']['view_node']['element_class'] = 'campaign-node-link';
  $handler->display->display_options['fields']['view_node']['element_label_type'] = '0';
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['view_node']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'Donate now';
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = '';
  $handler->display->display_options['fields']['edit_node']['element_type'] = 'span';
  $handler->display->display_options['fields']['edit_node']['element_class'] = 'node-edit';
  $handler->display->display_options['fields']['edit_node']['element_label_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['edit_node']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['edit_node']['element_default_classes'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Nid */
  $handler->display->display_options['arguments']['nid']['id'] = 'nid';
  $handler->display->display_options['arguments']['nid']['table'] = 'node';
  $handler->display->display_options['arguments']['nid']['field'] = 'nid';
  $handler->display->display_options['arguments']['nid']['relationship'] = 'reverse_field_p2p_feature_pers_campaigns_node';
  $handler->display->display_options['arguments']['nid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['nid']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
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
    'p2p_personal_campaign' => 'p2p_personal_campaign',
  );
  $export['p2p_referenced_campaigns'] = $view;

  $view = new view();
  $view->name = 'search_for_a_campaign';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Search for a Campaign';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['css_class'] = 'landing-search-view';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'input_required';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['exposed_form']['options']['expose_sort_order'] = FALSE;
  $handler->display->display_options['exposed_form']['options']['text_input_required_format'] = 'filtered_html';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '4';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
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
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );

  /* Display: Search widget exposed form */
  $handler = $view->new_display('panel_pane', 'Search widget exposed form', 'panel_pane_1');
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = TRUE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['exposed_block'] = TRUE;
  $handler->display->display_options['defaults']['footer'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
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
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['link_to_view'] = '0';
  $handler->display->display_options['inherit_panels_path'] = '1';

  /* Display: Search widget */
  $handler = $view->new_display('block', 'Search widget', 'block_1');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = TRUE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['defaults']['footer'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
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
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );

  /* Display: Search Results */
  $handler = $view->new_display('page', 'Search Results', 'page_1');
  $handler->display->display_options['defaults']['css_class'] = FALSE;
  $handler->display->display_options['css_class'] = 'landing-search-view-page';
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = TRUE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = '';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '8';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
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
  $handler->display->display_options['path'] = 'peer-to-peer-campaign-search';
  $export['search_for_a_campaign'] = $view;

  return $export;
}
