<?php

/**
 * @file
 * Provides default view configurations.
 */


/**
 * Implements hook_views_default_views().
 */
function sba_message_views_default_views() {
  $views = array();
  /* Header: Global: Unfiltered text */
  $view = new view();
  $view->name = 'sba_messages_node';
  $view->description = '';
  $view->tag = 'Advocacy';
  $view->base_table = 'sba_message';
  $view->human_name = 'SBA  Messages';
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
    'name' => 'name',
    'field_sba_subject' => 'field_sba_subject',
    'edit_sba_message' => 'edit_sba_message',
    'nid' => 'nid',
    'delete_sba_message' => 'edit_sba_message',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
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
    'edit_sba_message' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '|',
      'empty_column' => 0,
    ),
    'delete_sba_message' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_sba_alert_id_target_id']['id'] = 'field_sba_alert_id_target_id';
  $handler->display->display_options['relationships']['field_sba_alert_id_target_id']['table'] = 'field_data_field_sba_alert_id';
  $handler->display->display_options['relationships']['field_sba_alert_id_target_id']['field'] = 'field_sba_alert_id_target_id';
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

  /* Display: Embedded node/%/messages Block */
  $handler = $view->new_display('block', 'Embedded node/%/messages Block', 'block_1');
  $handler->display->display_options['display_description'] = 'Embedded block for edit page';
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'field_sba_subject' => 'field_sba_subject',
    'field_sba_alert_id' => 'field_sba_alert_id',
    'edit_sba_message' => 'edit_sba_message',
    'delete_sba_message' => 'edit_sba_message',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_sba_subject' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
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
      'separator' => ' | ',
      'empty_column' => 0,
    ),
    'delete_sba_message' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = 'No messages have been added yet.';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: SBA Message: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'sba_message';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: SBA Message: Subject */
  $handler->display->display_options['fields']['field_sba_subject']['id'] = 'field_sba_subject';
  $handler->display->display_options['fields']['field_sba_subject']['table'] = 'field_data_field_sba_subject';
  $handler->display->display_options['fields']['field_sba_subject']['field'] = 'field_sba_subject';
  $handler->display->display_options['fields']['field_sba_subject']['delta_offset'] = '0';
  /* Field: SBA Message: Edit Link */
  $handler->display->display_options['fields']['edit_sba_message']['id'] = 'edit_sba_message';
  $handler->display->display_options['fields']['edit_sba_message']['table'] = 'sba_message';
  $handler->display->display_options['fields']['edit_sba_message']['field'] = 'edit_sba_message';
  $handler->display->display_options['fields']['edit_sba_message']['label'] = 'Actions';
  /* Field: SBA Message: Delete Link */
  $handler->display->display_options['fields']['delete_sba_message']['id'] = 'delete_sba_message';
  $handler->display->display_options['fields']['delete_sba_message']['table'] = 'sba_message';
  $handler->display->display_options['fields']['delete_sba_message']['field'] = 'delete_sba_message';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: SBA Message: Sba message ID */
  $handler->display->display_options['sorts']['sba_message_id']['id'] = 'sba_message_id';
  $handler->display->display_options['sorts']['sba_message_id']['table'] = 'sba_message';
  $handler->display->display_options['sorts']['sba_message_id']['field'] = 'sba_message_id';
  $handler->display->display_options['sorts']['sba_message_id']['order'] = 'DESC';

  $views[$view->name] = $view;

  return $views;

}