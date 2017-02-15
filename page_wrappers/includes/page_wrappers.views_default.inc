<?php
/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_views_views_default_views() {
  $views = array();

  $view = new view();
  $view->name = 'pw_asset_autocomplete';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'file_managed';
  $view->human_name = 'PW Asset Autocomplete';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'PW Asset Autocomplete';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'edit own page_wrapper content';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: File: Name */
  $handler->display->display_options['fields']['filename']['id'] = 'filename';
  $handler->display->display_options['fields']['filename']['table'] = 'file_managed';
  $handler->display->display_options['fields']['filename']['field'] = 'filename';
  $handler->display->display_options['fields']['filename']['label'] = '';
  $handler->display->display_options['fields']['filename']['exclude'] = TRUE;
  $handler->display->display_options['fields']['filename']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['filename']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['filename']['element_label_colon'] = FALSE;
  /* Field: File: Title */
  $handler->display->display_options['fields']['field_pw_asset_title']['id'] = 'field_pw_asset_title';
  $handler->display->display_options['fields']['field_pw_asset_title']['table'] = 'field_data_field_pw_asset_title';
  $handler->display->display_options['fields']['field_pw_asset_title']['field'] = 'field_pw_asset_title';
  $handler->display->display_options['fields']['field_pw_asset_title']['label'] = '';
  $handler->display->display_options['fields']['field_pw_asset_title']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_pw_asset_title']['alter']['text'] = '[field_pw_asset_title] ([filename])';
  $handler->display->display_options['fields']['field_pw_asset_title']['element_type'] = '0';
  $handler->display->display_options['fields']['field_pw_asset_title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_pw_asset_title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['field_pw_asset_title']['element_default_classes'] = FALSE;
  /* Filter criterion: File: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'file_managed';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'page_wrappers_asset_css' => 'page_wrappers_asset_css',
  );

  /* Display: PW CSS Asset - Entity Reference */
  $handler = $view->new_display('entityreference', 'PW CSS Asset - Entity Reference', 'entityreference_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'entityreference_style';
  $handler->display->display_options['style_options']['search_fields'] = array(
    'filename' => 'filename',
    'field_pw_asset_title' => 'field_pw_asset_title',
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'entityreference_fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;

  /* Display: PW JS Asset - Entity Referencer */
  $handler = $view->new_display('entityreference', 'PW JS Asset - Entity Referencer', 'entityreference_2');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'entityreference_style';
  $handler->display->display_options['style_options']['search_fields'] = array(
    'filename' => 'filename',
    'field_pw_asset_title' => 'field_pw_asset_title',
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'entityreference_fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: File: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'file_managed';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'page_wrappers_asset_js' => 'page_wrappers_asset_js',
  );
  $views['pw_asset_autocomplete'] = $view;

  // Return all the views.
  return $views;
}