<?php

/**
 * @file
 * Providing extra functionality for the SBA Message UI via views.
 */


/**
 * Implements hook_views_data()
 */
function sba_message_views_data_alter(&$data) { 
  $data['sba_message']['link_sba_message'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the sba_message.'),
      'handler' => 'sba_message_handler_link_field',
    ),
  );
  $data['sba_message']['edit_sba_message'] = array(
    'field' => array(
      'title' => t('Edit Link'),
      'help' => t('Provide a link to the edit form for the sba_message.'),
      'handler' => 'sba_message_handler_edit_link_field',
    ),
  );
  $data['sba_message']['delete_sba_message'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the sba_message.'),
      'handler' => 'sba_message_handler_delete_link_field',
    ),
  );
  // This content of this field are decided based on the menu structure that
  // follows sba_messages/sba_message/%sba_message_id/op
  $data['sba_message']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all operations available for this sba_message.'),
      'handler' => 'sba_message_handler_sba_message_operations_field',
    ),
  );
}


/**
 * Implements hook_views_default_views().
 */
function sba_message_views_default_views() {
  $views = array();

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


/**/

  $view = new view;
  $view->name = 'sba_messages';
  $view->description = 'A list of all sba_messages';
  $view->tag = 'sba_messages';
  $view->base_table = 'sba_message';
  $view->human_name = 'SBA Messages';
  $view->core = 7;
  $view->api_version = '3.0-alpha1';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  
  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'SBA Messages';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'create any sba_message type';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'sba_message_id' => 'sba_message_id',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'sba_message_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'Empty ';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No sba_messages have been created yet';
  /* Field: SBA Message: SBA Message ID */
  $handler->display->display_options['fields']['sba_message_id']['id'] = 'sba_message_id';
  $handler->display->display_options['fields']['sba_message_id']['table'] = 'sba_message';
  $handler->display->display_options['fields']['sba_message_id']['field'] = 'sba_message_id';
  $handler->display->display_options['fields']['sba_message_id']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['external'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['sba_message_id']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['sba_message_id']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['alter']['html'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['sba_message_id']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['sba_message_id']['hide_empty'] = 0;
  $handler->display->display_options['fields']['sba_message_id']['empty_zero'] = 0;
  /* Field: SBA Message: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'sba_message';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['name']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['name']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['name']['empty_zero'] = 0;
  /* Field: SBA Message: Link */
  $handler->display->display_options['fields']['link_sba_message']['id'] = 'link_sba_message';
  $handler->display->display_options['fields']['link_sba_message']['table'] = 'sba_message';
  $handler->display->display_options['fields']['link_sba_message']['field'] = 'link_sba_message';
  $handler->display->display_options['fields']['link_sba_message']['label'] = 'View';
  $handler->display->display_options['fields']['link_sba_message']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['external'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['link_sba_message']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['link_sba_message']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['alter']['html'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['link_sba_message']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['link_sba_message']['hide_empty'] = 0;
  $handler->display->display_options['fields']['link_sba_message']['empty_zero'] = 0;
  /* Field: SBA Message: Operations links */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'sba_message';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';
  $handler->display->display_options['fields']['operations']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['external'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['operations']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['operations']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['operations']['alter']['html'] = 0;
  $handler->display->display_options['fields']['operations']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['operations']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['operations']['hide_empty'] = 0;
  $handler->display->display_options['fields']['operations']['empty_zero'] = 0;
  
  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'sba_messages_admin_page');
  $handler->display->display_options['path'] = 'admin/springboard/sba_messages/list';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'List';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['tab_options']['type'] = 'tab';
  $handler->display->display_options['tab_options']['title'] = 'SBA Messages';
  $handler->display->display_options['tab_options']['description'] = 'Manage sba_messages';
  $handler->display->display_options['tab_options']['weight'] = '0';
  $handler->display->display_options['tab_options']['name'] = 'management';
  $translatables['sba_messages'] = array(
    t('Master'),
    t('SBA Messages'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('Empty '),
    t('No sba_messages have been created yet'),
    t('SBA Message ID'),
    t('.'),
    t(','),
    t('Name'),
    t('View'),
    t('Operations links'),
    t('Page'),
  );
  $views[] = $view;
  return $views;

}
