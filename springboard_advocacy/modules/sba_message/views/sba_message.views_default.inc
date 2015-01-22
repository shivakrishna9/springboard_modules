<?php

/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function sba_messages_views_default_views() {

$view = new view();
$view->name = 'sba_messages_node';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'sba_message';
$view->human_name = 'SBA  Messges';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'SBA  Messages';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'field_sba_internal_title' => 'field_sba_internal_title',
  'field_sba_subject' => 'field_sba_subject',
  'field_sba_alert_id' => 'field_sba_alert_id',
  'edit_sba_message' => 'edit_sba_message',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'field_sba_internal_title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_sba_subject' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_sba_alert_id' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'edit_sba_message' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Unfiltered text */
$handler->display->display_options['header']['area_text_custom']['id'] = 'area_text_custom';
$handler->display->display_options['header']['area_text_custom']['table'] = 'views';
$handler->display->display_options['header']['area_text_custom']['field'] = 'area_text_custom';
$handler->display->display_options['header']['area_text_custom']['empty'] = TRUE;
$handler->display->display_options['header']['area_text_custom']['content'] = '<a href = "/admin/springboard/sba_messages/add/sba_letter_to_target_message?field_sba_alert_id=%1">Create Message</a>';
$handler->display->display_options['header']['area_text_custom']['tokenize'] = TRUE;
/* Field: SBA Message: Internal Title */
$handler->display->display_options['fields']['field_sba_internal_title']['id'] = 'field_sba_internal_title';
$handler->display->display_options['fields']['field_sba_internal_title']['table'] = 'field_data_field_sba_internal_title';
$handler->display->display_options['fields']['field_sba_internal_title']['field'] = 'field_sba_internal_title';
/* Field: SBA Message: Subject */
$handler->display->display_options['fields']['field_sba_subject']['id'] = 'field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['table'] = 'field_data_field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['field'] = 'field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['delta_offset'] = '0';
/* Field: SBA Message: Alert ID */
$handler->display->display_options['fields']['field_sba_alert_id']['id'] = 'field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['table'] = 'field_data_field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['field'] = 'field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['settings'] = array(
  'link' => 0,
);
/* Field: SBA Message: Edit Link */
$handler->display->display_options['fields']['edit_sba_message']['id'] = 'edit_sba_message';
$handler->display->display_options['fields']['edit_sba_message']['table'] = 'sba_message';
$handler->display->display_options['fields']['edit_sba_message']['field'] = 'edit_sba_message';
/* Contextual filter: SBA Message: Alert ID (field_sba_alert_id) */
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['id'] = 'field_sba_alert_id_target_id';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['table'] = 'field_data_field_sba_alert_id';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['field'] = 'field_sba_alert_id_target_id';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['default_action'] = 'access denied';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_sba_alert_id_target_id']['summary_options']['items_per_page'] = '25';
/* Filter criterion: SBA Message: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'sba_message';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'sba_letter_to_target_message' => 'sba_letter_to_target_message',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'node/%/messages';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Messages';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

/* Display: EVA Field */
$handler = $view->new_display('entity_view', 'EVA Field', 'entity_view_1');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['header'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: SBA Message: Subject */
$handler->display->display_options['fields']['field_sba_subject']['id'] = 'field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['table'] = 'field_data_field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['field'] = 'field_sba_subject';
$handler->display->display_options['fields']['field_sba_subject']['delta_offset'] = '0';
/* Field: SBA Message: Alert ID */
$handler->display->display_options['fields']['field_sba_alert_id']['id'] = 'field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['table'] = 'field_data_field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['field'] = 'field_sba_alert_id';
$handler->display->display_options['fields']['field_sba_alert_id']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_sba_alert_id']['settings'] = array(
  'link' => 0,
);
/* Field: SBA Message: Message */
$handler->display->display_options['fields']['field_sba_message']['id'] = 'field_sba_message';
$handler->display->display_options['fields']['field_sba_message']['table'] = 'field_data_field_sba_message';
$handler->display->display_options['fields']['field_sba_message']['field'] = 'field_sba_message';
$handler->display->display_options['fields']['field_sba_message']['delta_offset'] = '0';
$handler->display->display_options['entity_type'] = 'node';
$handler->display->display_options['bundles'] = array(
  0 => 'springboard_letter',
);
  $views[$view->name] = $view;

  // Return all the views.
  return $views;
}