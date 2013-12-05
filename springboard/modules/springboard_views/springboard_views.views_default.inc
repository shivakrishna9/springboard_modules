<?php
/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_views_views_default_views() {
  /**
   * View sbv_forms
   */
  $view = new view();
  $view->name = 'sbv_forms';
  $view->description = 'Springboard form summaries';
  $view->tag = 'springboard_views';
  $view->base_table = 'node';
  $view->human_name = 'sbv_forms';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Recent Forms';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access springboard dashboard';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'mini';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'nid' => 'nid',
    'status' => 'status',
    'created' => 'created',
    'path' => 'path',
    'clone_node' => 'view_node',
    'view_node' => 'view_node',
    'webform_edit' => 'view_node',
    'webform_submission_count_node' => 'webform_submission_count_node',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'path' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'webform_edit' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'webform_submission_count_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Internal Name */
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['id'] = 'field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['table'] = 'field_data_field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['field'] = 'field_fundraiser_internal_name';
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['label'] = 'Submissions';
  $handler->display->display_options['fields']['webform_submission_count_node']['element_label_colon'] = FALSE;
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  /* Field: Webform: Webform edit link */
  $handler->display->display_options['fields']['webform_edit']['id'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['table'] = 'node';
  $handler->display->display_options['fields']['webform_edit']['field'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['label'] = '';
  $handler->display->display_options['fields']['webform_edit']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['webform_edit']['text'] = 'edit';
  /* Field: Content: Updated date */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'node';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Last updated date';
  $handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['changed']['date_format'] = 'short';
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  /* Sort criterion: Content revision: Updated date */
  $handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['table'] = 'node_revision';
  $handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type_1']['id'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['table'] = 'node';
  $handler->display->display_options['filters']['type_1']['field'] = 'type';
  $handler->display->display_options['filters']['type_1']['value'] = array(
    'donation_form' => 'donation_form',
  );
  $handler->display->display_options['filters']['type_1']['group'] = 1;
  $handler->display->display_options['filters']['type_1']['expose']['operator_id'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_1']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['operator'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['identifier'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Published';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['required'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );

  /* Display: Springboard Recent Forms */
  $handler = $view->new_display('block', 'Springboard Recent Forms', 'block');
  $handler->display->display_options['display_description'] = 'View configuration and submission information about Springboard forms.';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'View All Donation Forms';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'nid' => 'nid',
    'path' => 'path',
    'webform_submission_count_node' => 'webform_submission_count_node',
    'view_node' => 'view_node',
    'webform_edit' => 'webform_edit',
    'changed' => 'changed',
    'clone_node' => 'clone_node',
    'status' => 'status',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'path' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'webform_submission_count_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'webform_edit' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'changed' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['element_label_colon'] = FALSE;
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['label'] = 'View';
  $handler->display->display_options['fields']['clone_node']['element_label_colon'] = FALSE;
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['exclude'] = TRUE;
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  /* Field: Webform: Webform edit link */
  $handler->display->display_options['fields']['webform_edit']['id'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['table'] = 'node';
  $handler->display->display_options['fields']['webform_edit']['field'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['label'] = 'Edit';
  $handler->display->display_options['fields']['webform_edit']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['webform_edit']['text'] = 'Edit';
  /* Field: Content: Updated date */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'node';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Last updated date';
  $handler->display->display_options['fields']['changed']['exclude'] = TRUE;
  $handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['changed']['date_format'] = 'short';
  /* Field: Webform: Webform results link */
  $handler->display->display_options['fields']['webform_results']['id'] = 'webform_results';
  $handler->display->display_options['fields']['webform_results']['table'] = 'node';
  $handler->display->display_options['fields']['webform_results']['field'] = 'webform_results';
  $handler->display->display_options['fields']['webform_results']['label'] = 'Results';
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'webform' => 'webform',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;

  /* Display: Springboard Recent Donation Forms */
  $handler = $view->new_display('block', 'Springboard Recent Donation Forms', 'block_1');
  $handler->display->display_options['display_description'] = 'View configuration and submission information about Springboard forms.';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'View All Donation Forms';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'webform_submission_count_node' => 'webform_submission_count_node',
    'field_fundraiser_internal_name' => 'field_fundraiser_internal_name',
    'nid' => 'nid',
    'value' => 'value',
    'value_1' => 'value_1',
    'path' => 'path',
    'changed' => 'changed',
    'view_node' => 'view_node',
    'edit_node' => 'edit_node',
    'clone_node' => 'clone_node',
    'status' => 'status',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'webform_submission_count_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_fundraiser_internal_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'value' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'value_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'path' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'changed' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['element_label_colon'] = FALSE;
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['label'] = 'View';
  $handler->display->display_options['fields']['clone_node']['element_label_colon'] = FALSE;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = 'Edit';
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  /* Field: Content: Internal Name */
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['id'] = 'field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['table'] = 'field_data_field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['field'] = 'field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['element_label_colon'] = FALSE;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Webform: Webform Hidden Components */
  $handler->display->display_options['fields']['value']['id'] = 'value';
  $handler->display->display_options['fields']['value']['table'] = 'webform_component';
  $handler->display->display_options['fields']['value']['field'] = 'value';
  $handler->display->display_options['fields']['value']['label'] = 'Market Source';
  $handler->display->display_options['fields']['value']['exclude'] = TRUE;
  $handler->display->display_options['fields']['value']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['value']['webform_component_options'] = 'ms';
  /* Field: Webform: Webform Hidden Components */
  $handler->display->display_options['fields']['value_1']['id'] = 'value_1';
  $handler->display->display_options['fields']['value_1']['table'] = 'webform_component';
  $handler->display->display_options['fields']['value_1']['field'] = 'value';
  $handler->display->display_options['fields']['value_1']['label'] = 'Campaign';
  $handler->display->display_options['fields']['value_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['value_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['value_1']['webform_component_options'] = 'cid';
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Content: Updated date */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'node';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Last updated date';
  $handler->display->display_options['fields']['changed']['exclude'] = TRUE;
  $handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['changed']['date_format'] = 'short';
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['exclude'] = TRUE;
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  /* Field: Webform: Webform results link */
  $handler->display->display_options['fields']['webform_results']['id'] = 'webform_results';
  $handler->display->display_options['fields']['webform_results']['table'] = 'node';
  $handler->display->display_options['fields']['webform_results']['field'] = 'webform_results';
  $handler->display->display_options['fields']['webform_results']['label'] = 'Results';
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'donation_form' => 'donation_form',
  );

  /* Display: Springboard Recent Forms Page */
  $handler = $view->new_display('page', 'Springboard Recent Forms Page', 'page_1');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '35';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Date Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'n/j/o';
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['label'] = 'Submissions';
  $handler->display->display_options['fields']['webform_submission_count_node']['element_label_colon'] = FALSE;
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['label'] = 'Action';
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  /* Field: Webform: Webform edit link */
  $handler->display->display_options['fields']['webform_edit']['id'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['table'] = 'node';
  $handler->display->display_options['fields']['webform_edit']['field'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['label'] = '';
  $handler->display->display_options['fields']['webform_edit']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['webform_edit']['text'] = 'edit';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type_1']['id'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['table'] = 'node';
  $handler->display->display_options['filters']['type_1']['field'] = 'type';
  $handler->display->display_options['filters']['type_1']['value'] = array(
    'webform' => 'webform',
  );
  $handler->display->display_options['filters']['type_1']['group'] = 1;
  $handler->display->display_options['filters']['type_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['operator_id'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_1']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['operator'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['identifier'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Published';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['required'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'webform' => 'webform',
  );
  $handler->display->display_options['path'] = 'admin/springboard/forms/webforms';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Webforms';
  $handler->display->display_options['menu']['description'] = 'View and edit all the available webforms on your site.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Springboard Recent Donation Forms Page */
  $handler = $view->new_display('page', 'Springboard Recent Donation Forms Page', 'page_2');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '35';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'nid' => 'nid',
    'status' => 'status',
    'created' => 'created',
    'path' => 'path',
    'webform_submission_count_node' => 'webform_submission_count_node',
    'view_node' => 'view_node',
    'webform_edit' => 'view_node',
    'changed' => 'changed',
    'clone_node' => 'view_node',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'path' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'webform_submission_count_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'webform_edit' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'changed' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Date Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'n/j/o';
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['label'] = 'Submissions';
  $handler->display->display_options['fields']['webform_submission_count_node']['element_label_colon'] = FALSE;
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  /* Field: Webform: Webform edit link */
  $handler->display->display_options['fields']['webform_edit']['id'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['table'] = 'node';
  $handler->display->display_options['fields']['webform_edit']['field'] = 'webform_edit';
  $handler->display->display_options['fields']['webform_edit']['label'] = '';
  $handler->display->display_options['fields']['webform_edit']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['webform_edit']['text'] = 'edit';
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type_1']['id'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['table'] = 'node';
  $handler->display->display_options['filters']['type_1']['field'] = 'type';
  $handler->display->display_options['filters']['type_1']['value'] = array(
    'donation_form' => 'donation_form',
  );
  $handler->display->display_options['filters']['type_1']['group'] = 1;
  $handler->display->display_options['filters']['type_1']['expose']['operator_id'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type_1']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['operator'] = 'type_1_op';
  $handler->display->display_options['filters']['type_1']['expose']['identifier'] = 'type_1';
  $handler->display->display_options['filters']['type_1']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['type_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = '';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Published';
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['required'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  /* Filter criterion: Content: Internal Name (field_fundraiser_internal_name) */
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['id'] = 'field_fundraiser_internal_name_value';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['table'] = 'field_data_field_fundraiser_internal_name';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['field'] = 'field_fundraiser_internal_name_value';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['operator_id'] = 'field_fundraiser_internal_name_value_op';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['label'] = 'Internal Name';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['operator'] = 'field_fundraiser_internal_name_value_op';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['identifier'] = 'field_fundraiser_internal_name_value';
  $handler->display->display_options['filters']['field_fundraiser_internal_name_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    5 => 0,
    6 => 0,
    7 => 0,
    8 => 0,
  );
  $handler->display->display_options['path'] = 'admin/springboard/forms/donations';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Donations';
  $handler->display->display_options['menu']['description'] = 'View and edit all the available donations on your site.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Fundraiser Gateway Switcher */
  $handler = $view->new_display('page', 'Fundraiser Gateway Switcher', 'page_3');
  $handler->display->display_options['defaults']['access'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'use gateway switcher';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Bulk operations: Content */
  $handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['table'] = 'node';
  $handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_operations'] = array(
    'action::_fundraiser_gateway_switch_action' => array(
      'selected' => 1,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
    ),
  );
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Form Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Internal Name */
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['id'] = 'field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['table'] = 'field_data_field_fundraiser_internal_name';
  $handler->display->display_options['fields']['field_fundraiser_internal_name']['field'] = 'field_fundraiser_internal_name';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = 'Form ID';
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Path */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  /* Field: Content: Link */
  $handler->display->display_options['fields']['view_node']['id'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['view_node']['field'] = 'view_node';
  $handler->display->display_options['fields']['view_node']['label'] = 'Action';
  $handler->display->display_options['fields']['view_node']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['view_node']['text'] = 'view';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  $handler->display->display_options['path'] = 'admin/content/donation-forms/gateway-switcher';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Gateway Switcher';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  $views[$view->name] = $view;

  /**
   * View 'sbv_batch'
   */
  $view = new view;
  $view->name = 'sbv_batch';
  $view->description = 'List of items in the batch';
  $view->tag = 'springboard_views';
  $view->base_table = 'sf_batch';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'id' => array(
      'label' => 'ID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'queue_api_batch_id_items_link' => 1,
      'exclude' => 0,
      'id' => 'id',
      'table' => 'sf_batch',
      'field' => 'id',
      'relationship' => 'none',
    ),
    'type' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'type',
      'table' => 'sf_batch',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'created' => array(
      'label' => 'Created',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'created',
      'table' => 'sf_batch',
      'field' => 'created',
      'relationship' => 'none',
    ),
    'record_count' => array(
      'label' => 'Items',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'record_count',
      'table' => 'sf_batch',
      'field' => 'record_count',
      'relationship' => 'none',
    ),
    'success_count' => array(
      'label' => 'Successes',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'success_count',
      'table' => 'sf_batch',
      'field' => 'success_count',
      'relationship' => 'none',
    ),
    'failure_count' => array(
      'label' => 'Failures',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'failure_count',
      'table' => 'sf_batch',
      'field' => 'failure_count',
      'relationship' => 'none',
    ),
    'start_time' => array(
      'label' => 'Start time',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'start_time',
      'table' => 'sf_batch',
      'field' => 'start_time',
      'relationship' => 'none',
    ),
    'end_time' => array(
      'label' => 'End time',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'end_time',
      'table' => 'sf_batch',
      'field' => 'end_time',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'id' => array(
      'order' => 'DESC',
      'id' => 'id',
      'table' => 'sf_batch',
      'field' => 'id',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'id' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'id_op',
        'identifier' => 'id',
        'label' => 'Primary key: ID',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'id',
      'table' => 'sf_batch',
      'field' => 'id',
      'relationship' => 'none',
    ),
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'user' => 'user',
        'webform' => 'webform',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_op',
        'identifier' => 'type',
        'label' => 'Type',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'type',
      'table' => 'sf_batch',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'date_filter_3' => array(
      'operator' => '>',
      'value' => array(
        'value' => NULL,
        'min' => NULL,
        'max' => NULL,
        'default_date' => '',
        'default_to_date' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'date_filter_3_op',
        'identifier' => 'date_filter_3',
        'label' => 'Dates',
        'optional' => 1,
        'remember' => 0,
      ),
      'date_fields' => array(
        'sf_batch.created' => 'sf_batch.created',
        'sf_batch.end_time' => 'sf_batch.end_time',
        'sf_batch.start_time' => 'sf_batch.start_time',
      ),
      'date_method' => 'OR',
      'granularity' => 'minute',
      'form_type' => 'date_select',
      'default_date' => '',
      'default_to_date' => '',
      'year_range' => '-3:+3',
      'id' => 'date_filter_3',
      'table' => 'sf_batch',
      'field' => 'date_filter',
      'relationship' => 'none',
    ),
    'failure_count' => array(
      'operator' => '>',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'failure_count_op',
        'identifier' => 'failure_count',
        'label' => 'Failures',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'failure_count',
      'table' => 'sf_batch',
      'field' => 'failure_count',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view salesforce reports',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Batch history');
  $handler->override_option('header', 'Click on the id of the batch to drill down into more detail.');
  $handler->override_option('header_format', '1');
  $handler->override_option('header_empty', 0);
  $handler->override_option('items_per_page', 20);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'id' => 'id',
      'type' => 'type',
      'created' => 'created',
      'record_count' => 'record_count',
      'start_time' => 'start_time',
      'end_time' => 'end_time',
    ),
    'info' => array(
      'id' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'type' => array(
        'separator' => '',
      ),
      'created' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'record_count' => array(
        'separator' => '',
      ),
      'start_time' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'end_time' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/reports/salesforce/batch');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Batch history',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;

  /**
   * View 'sbv_permanent_failure'
   */
  $view = new view;
  $view->name = 'sbv_permanent_failure';
  $view->description = 'Permanent failures';
  $view->tag = 'springboard_views';
  $view->base_table = 'sf_permanent_failure';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'oid' => array(
      'label' => 'Object ID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'oid',
      'table' => 'sf_permanent_failure',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'type' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'type',
      'table' => 'sf_permanent_failure',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'fieldmap' => array(
      'label' => 'Fieldmap',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'fieldmap',
      'table' => 'sf_permanent_failure',
      'field' => 'fieldmap',
      'relationship' => 'none',
    ),
    'action' => array(
      'label' => 'Action',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'action',
      'table' => 'sf_permanent_failure',
      'field' => 'action',
      'relationship' => 'none',
    ),
    'date_added' => array(
      'label' => 'Date added',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'date_added',
      'table' => 'sf_permanent_failure',
      'field' => 'date_added',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'date_added' => array(
      'order' => 'ASC',
      'granularity' => 'second',
      'id' => 'date_added',
      'table' => 'sf_permanent_failure',
      'field' => 'date_added',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_op',
        'identifier' => 'type',
        'label' => 'Type',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'type',
      'table' => 'sf_permanent_failure',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'oid' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'oid_op',
        'identifier' => 'oid',
        'label' => 'Object ID',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'oid',
      'table' => 'sf_permanent_failure',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'date_filter' => array(
      'operator' => '>',
      'value' => array(
        'value' => '2012-01-01 00:00:00',
        'min' => '2012-01-01 00:00:00',
        'max' => '2012-01-01 00:00:00',
        'default_date' => '',
        'default_to_date' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'date_filter_op',
        'identifier' => 'date_filter',
        'label' => 'Date added',
        'optional' => 1,
        'remember' => 0,
      ),
      'date_fields' => array(
        'sf_permanent_failure.date_added' => 'sf_permanent_failure.date_added',
      ),
      'date_method' => 'OR',
      'granularity' => 'day',
      'form_type' => 'date_select',
      'default_date' => '',
      'default_to_date' => '',
      'year_range' => '-3:+3',
      'id' => 'date_filter',
      'table' => 'sf_permanent_failure',
      'field' => 'date_filter',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view salesforce reports',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Permanent failures');
  $handler->override_option('header', 'The following items have been marked as permanent failures. No more attempts to export them will be made until they have been edited.');
  $handler->override_option('header_format', '1');
  $handler->override_option('header_empty', 1);
  $handler->override_option('empty', 'There are no permanent failures.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 25);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'bulk');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'oid' => 'oid',
      'type' => 'type',
      'fieldmap' => 'fieldmap',
      'action' => 'action',
      'date_added' => 'date_added',
    ),
    'info' => array(
      'oid' => array(
        'separator' => '',
      ),
      'type' => array(
        'separator' => '',
      ),
      'fieldmap' => array(
        'separator' => '',
      ),
      'action' => array(
        'separator' => '',
      ),
      'date_added' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
    'execution_type' => '1',
    'max_performance' => 0,
    'display_type' => '0',
    'hide_selector' => 0,
    'preserve_selection' => 1,
    'display_result' => 1,
    'merge_single_action' => 1,
    'operations' => array(
      'springboard_views_sf_permanent_failure_delete_action' => array(
        'selected' => 1,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_message_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_script_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_argument_selector_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_goto_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'springboard_views_sf_permanent_failure_retry_action' => array(
        'selected' => 1,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_send_email_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
    ),
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/reports/salesforce/permanent-failures');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Permanent failures',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;

  /**
   * View 'sbv_queue'
   */
  $view = new view;
  $view->name = 'sbv_queue';
  $view->description = 'Currently queued items';
  $view->tag = 'springboard_views';
  $view->base_table = 'sf_queue';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'oid' => array(
      'label' => 'Object ID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'oid',
      'table' => 'sf_queue',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'sfid' => array(
      'label' => 'SFID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'sfid',
      'table' => 'sf_queue',
      'field' => 'sfid',
      'relationship' => 'none',
    ),
    'type' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'type',
      'table' => 'sf_queue',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'action' => array(
      'label' => 'Action',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'action',
      'table' => 'sf_queue',
      'field' => 'action',
      'relationship' => 'none',
    ),
    'created' => array(
      'label' => 'Date added',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'created',
      'table' => 'sf_queue',
      'field' => 'created',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
  ));
  $handler->override_option('sorts', array(
    'created' => array(
      'order' => 'ASC',
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'sf_queue',
      'field' => 'created',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_op',
        'identifier' => 'type',
        'label' => 'Type',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'type',
      'table' => 'sf_queue',
      'field' => 'type',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'oid' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'oid_op',
        'identifier' => 'oid',
        'label' => 'Object ID',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'oid',
      'table' => 'sf_queue',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'date_filter' => array(
      'operator' => '>',
      'value' => array(
        'value' => NULL,
        'min' => NULL,
        'max' => NULL,
        'default_date' => '',
        'default_to_date' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'date_filter_op',
        'identifier' => 'date_filter',
        'label' => 'Date added',
        'optional' => 1,
        'remember' => 0,
      ),
      'date_fields' => array(
        'sf_queue.created' => 'sf_queue.created',
      ),
      'date_method' => 'OR',
      'granularity' => 'minute',
      'form_type' => 'date_select',
      'default_date' => '',
      'default_to_date' => '',
      'year_range' => '-3:+3',
      'id' => 'date_filter',
      'table' => 'sf_queue',
      'field' => 'date_filter',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view salesforce reports',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Currently queued items');
  $handler->override_option('header', 'These items are currently queued up and will be exported to Salesforce during the next cron run.');
  $handler->override_option('header_format', '1');
  $handler->override_option('header_empty', 1);
  $handler->override_option('empty', 'No items are queued.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 25);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'oid' => 'oid',
      'sfid' => 'sfid',
      'type' => 'type',
      'action' => 'action',
      'created' => 'created',
    ),
    'info' => array(
      'oid' => array(
        'separator' => '',
      ),
      'sfid' => array(
        'separator' => '',
      ),
      'type' => array(
        'separator' => '',
      ),
      'action' => array(
        'separator' => '',
      ),
      'created' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/reports/salesforce/current');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Currently queued items',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;

  /**
   * View 'sbv_retry_queue'
   */
  $view = new view;
  $view->name = 'sbv_retry_queue';
  $view->description = 'Retry queue';
  $view->tag = 'springboard_views';
  $view->base_table = 'sf_retry_queue';
  $view->core = 6;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'oid' => array(
      'label' => 'Object ID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'exclude' => 0,
      'id' => 'oid',
      'table' => 'sf_retry_queue',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'sfid' => array(
      'label' => 'SFID',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'sfid',
      'table' => 'sf_retry_queue',
      'field' => 'sfid',
      'relationship' => 'none',
    ),
    'type' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'type',
      'table' => 'sf_retry_queue',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'fieldmap' => array(
      'label' => 'Fieldmap',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'fieldmap',
      'table' => 'sf_retry_queue',
      'field' => 'fieldmap',
      'relationship' => 'none',
    ),
    'action' => array(
      'label' => 'Action',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'action',
      'table' => 'sf_retry_queue',
      'field' => 'action',
      'relationship' => 'none',
    ),
    'retry_date' => array(
      'label' => 'Retry date',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'retry_date',
      'table' => 'sf_retry_queue',
      'field' => 'retry_date',
      'relationship' => 'none',
    ),
    'attempts' => array(
      'label' => 'Attempts',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'set_precision' => FALSE,
      'precision' => 0,
      'decimal' => '.',
      'separator' => ',',
      'format_plural' => 0,
      'format_plural_singular' => '1',
      'format_plural_plural' => '@count',
      'prefix' => '',
      'suffix' => '',
      'exclude' => 0,
      'id' => 'attempts',
      'table' => 'sf_retry_queue',
      'field' => 'attempts',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'retry_date' => array(
      'order' => 'ASC',
      'granularity' => 'second',
      'id' => 'retry_date',
      'table' => 'sf_retry_queue',
      'field' => 'retry_date',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_op',
        'identifier' => 'type',
        'label' => 'Type',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'type',
      'table' => 'sf_retry_queue',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'oid' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'oid_op',
        'identifier' => 'oid',
        'label' => 'Object ID',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'oid',
      'table' => 'sf_retry_queue',
      'field' => 'oid',
      'relationship' => 'none',
    ),
    'date_filter' => array(
      'operator' => '>',
      'value' => array(
        'value' => NULL,
        'min' => NULL,
        'max' => NULL,
        'default_date' => '',
        'default_to_date' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 1,
        'operator' => 'date_filter_op',
        'identifier' => 'date_filter',
        'label' => 'Dates',
        'optional' => 1,
        'remember' => 0,
      ),
      'date_fields' => array(
        'sf_retry_queue.created' => 'sf_retry_queue.created',
        'sf_retry_queue.retry_date' => 'sf_retry_queue.retry_date',
      ),
      'date_method' => 'OR',
      'granularity' => 'minute',
      'form_type' => 'date_select',
      'default_date' => '',
      'default_to_date' => '',
      'year_range' => '-3:+3',
      'id' => 'date_filter',
      'table' => 'sf_retry_queue',
      'field' => 'date_filter',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'view salesforce reports',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Retry queue');
  $handler->override_option('header', 'The following items are currently in the retry queue. If not edited beforehand, the next export attempt will be performed on the items\'s retry date.');
  $handler->override_option('header_format', '1');
  $handler->override_option('header_empty', 0);
  $handler->override_option('empty', 'There are no items in the retry queue.');
  $handler->override_option('empty_format', '1');
  $handler->override_option('items_per_page', 25);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'bulk');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'oid' => 'oid',
      'sfid' => 'sfid',
      'type' => 'type',
      'fieldmap' => 'fieldmap',
      'retry_date' => 'retry_date',
      'action' => 'action',
      'attempts' => 'attempts',
    ),
    'info' => array(
      'oid' => array(
        'separator' => '',
      ),
      'sfid' => array(
        'separator' => '',
      ),
      'type' => array(
        'separator' => '',
      ),
      'fieldmap' => array(
        'separator' => '',
      ),
      'retry_date' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'action' => array(
        'separator' => '',
      ),
      'attempts' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
    'execution_type' => '1',
    'max_performance' => 0,
    'display_type' => '0',
    'hide_selector' => 0,
    'preserve_selection' => 1,
    'display_result' => 1,
    'merge_single_action' => 1,
    'operations' => array(
      'springboard_views_sf_retry_queue_delete_action' => array(
        'selected' => 1,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_message_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_script_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'views_bulk_operations_argument_selector_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_goto_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'springboard_views_sf_retry_queue_retry_action' => array(
        'selected' => 1,
        'skip_confirmation' => 0,
        'label' => '',
      ),
      'system_send_email_action' => array(
        'selected' => 0,
        'skip_confirmation' => 0,
        'label' => '',
      ),
    ),
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/reports/salesforce/retries');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Retry queue',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $views[$view->name] = $view;
  /**
   * View 'sbv_donations'
   */
  $view = new view();
  $view->name = 'sbv_donations';
  $view->description = 'Donations currently in the system';
  $view->tag = 'springboard_views';
  $view->base_table = 'fundraiser_donation';
  $view->human_name = 'sbv_donations';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Donations';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer fundraiser';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '35';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'gateway_1' => 'gateway_1',
    'txn_id' => 'txn_id',
    'billing_first_name' => 'billing_first_name',
    'billing_last_name' => 'billing_first_name',
    'billing_street1' => 'billing_street1',
    'billing_street2' => 'billing_street1',
    'billing_city' => 'billing_street1',
    'country_name' => 'billing_street1',
    'zone_name' => 'billing_street1',
    'zone_code' => 'billing_street1',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'gateway_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'txn_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'billing_first_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'billing_last_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'billing_street1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '</br>',
      'empty_column' => 0,
    ),
    'billing_street2' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'billing_city' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'country_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'zone_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'zone_code' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Fundraiser donations: Donating user */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'fundraiser_donation';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  /* Relationship: Fundraiser donations: Fundraiser node */
  $handler->display->display_options['relationships']['nid']['id'] = 'nid';
  $handler->display->display_options['relationships']['nid']['table'] = 'fundraiser_donation';
  $handler->display->display_options['relationships']['nid']['field'] = 'nid';
  /* Relationship: Fundraiser donations: Donation order */
  $handler->display->display_options['relationships']['did']['id'] = 'did';
  $handler->display->display_options['relationships']['did']['table'] = 'fundraiser_donation';
  $handler->display->display_options['relationships']['did']['field'] = 'did';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'nid';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'nid';
  /* Field: Content: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'node';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  $handler->display->display_options['fields']['type']['relationship'] = 'nid';
  /* Field: User: Uid */
  $handler->display->display_options['fields']['uid']['id'] = 'uid';
  $handler->display->display_options['fields']['uid']['table'] = 'users';
  $handler->display->display_options['fields']['uid']['field'] = 'uid';
  $handler->display->display_options['fields']['uid']['relationship'] = 'uid';
  /* Field: User: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'uid';
  /* Field: User: E-mail */
  $handler->display->display_options['fields']['mail']['id'] = 'mail';
  $handler->display->display_options['fields']['mail']['table'] = 'users';
  $handler->display->display_options['fields']['mail']['field'] = 'mail';
  $handler->display->display_options['fields']['mail']['relationship'] = 'uid';
  /* Field: Order: Billing address: First name */
  $handler->display->display_options['fields']['billing_first_name']['id'] = 'billing_first_name';
  $handler->display->display_options['fields']['billing_first_name']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_first_name']['field'] = 'billing_first_name';
  $handler->display->display_options['fields']['billing_first_name']['relationship'] = 'did';
  /* Field: Order: Billing address: Last name */
  $handler->display->display_options['fields']['billing_last_name']['id'] = 'billing_last_name';
  $handler->display->display_options['fields']['billing_last_name']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_last_name']['field'] = 'billing_last_name';
  $handler->display->display_options['fields']['billing_last_name']['relationship'] = 'did';
  /* Field: Order: Billing address: Street address 1 */
  $handler->display->display_options['fields']['billing_street1']['id'] = 'billing_street1';
  $handler->display->display_options['fields']['billing_street1']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_street1']['field'] = 'billing_street1';
  $handler->display->display_options['fields']['billing_street1']['relationship'] = 'did';
  /* Field: Order: Billing address: Street address 2 */
  $handler->display->display_options['fields']['billing_street2']['id'] = 'billing_street2';
  $handler->display->display_options['fields']['billing_street2']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_street2']['field'] = 'billing_street2';
  $handler->display->display_options['fields']['billing_street2']['relationship'] = 'did';
  /* Field: Order: Billing address: City */
  $handler->display->display_options['fields']['billing_city']['id'] = 'billing_city';
  $handler->display->display_options['fields']['billing_city']['table'] = 'uc_orders';
  $handler->display->display_options['fields']['billing_city']['field'] = 'billing_city';
  $handler->display->display_options['fields']['billing_city']['relationship'] = 'did';
  /* Field: Order: Billing address: Country */
  $handler->display->display_options['fields']['country_name']['id'] = 'country_name';
  $handler->display->display_options['fields']['country_name']['table'] = 'billing_countries';
  $handler->display->display_options['fields']['country_name']['field'] = 'country_name';
  $handler->display->display_options['fields']['country_name']['relationship'] = 'did';
  /* Field: Order: Billing address: State/Province */
  $handler->display->display_options['fields']['zone_name']['id'] = 'zone_name';
  $handler->display->display_options['fields']['zone_name']['table'] = 'billing_zones';
  $handler->display->display_options['fields']['zone_name']['field'] = 'zone_name';
  $handler->display->display_options['fields']['zone_name']['relationship'] = 'did';
  /* Field: Order: Billing address: State/Province code */
  $handler->display->display_options['fields']['zone_code']['id'] = 'zone_code';
  $handler->display->display_options['fields']['zone_code']['table'] = 'billing_zones';
  $handler->display->display_options['fields']['zone_code']['field'] = 'zone_code';
  $handler->display->display_options['fields']['zone_code']['relationship'] = 'did';
  /* Field: Fundraiser donations: Transaction ID */
  $handler->display->display_options['fields']['txn_id']['id'] = 'txn_id';
  $handler->display->display_options['fields']['txn_id']['table'] = 'fundraiser_donation';
  $handler->display->display_options['fields']['txn_id']['field'] = 'txn_id';
  /* Field: Fundraiser donations: Gateway */
  $handler->display->display_options['fields']['gateway_1']['id'] = 'gateway_1';
  $handler->display->display_options['fields']['gateway_1']['table'] = 'fundraiser_donation';
  $handler->display->display_options['fields']['gateway_1']['field'] = 'gateway';
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['relationship'] = 'nid';
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
  $handler->display->display_options['filters']['title']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['relationship'] = 'nid';
  $handler->display->display_options['filters']['type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Fundraiser donations: Gateway */
  $handler->display->display_options['filters']['gateway']['id'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['table'] = 'fundraiser_donation';
  $handler->display->display_options['filters']['gateway']['field'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['exposed'] = TRUE;
  $handler->display->display_options['filters']['gateway']['expose']['operator_id'] = 'gateway_op';
  $handler->display->display_options['filters']['gateway']['expose']['label'] = 'Gateway';
  $handler->display->display_options['filters']['gateway']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['gateway']['expose']['operator'] = 'gateway_op';
  $handler->display->display_options['filters']['gateway']['expose']['identifier'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['gateway']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: Fundraiser donations: Transaction ID */
  $handler->display->display_options['filters']['txn_id']['id'] = 'txn_id';
  $handler->display->display_options['filters']['txn_id']['table'] = 'fundraiser_donation';
  $handler->display->display_options['filters']['txn_id']['field'] = 'txn_id';
  $handler->display->display_options['filters']['txn_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['txn_id']['expose']['operator_id'] = 'txn_id_op';
  $handler->display->display_options['filters']['txn_id']['expose']['label'] = 'Transaction ID';
  $handler->display->display_options['filters']['txn_id']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['txn_id']['expose']['operator'] = 'txn_id_op';
  $handler->display->display_options['filters']['txn_id']['expose']['identifier'] = 'txn_id';
  $handler->display->display_options['filters']['txn_id']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['txn_id']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: User: Name */
  $handler->display->display_options['filters']['uid']['id'] = 'uid';
  $handler->display->display_options['filters']['uid']['table'] = 'users';
  $handler->display->display_options['filters']['uid']['field'] = 'uid';
  $handler->display->display_options['filters']['uid']['relationship'] = 'uid';
  $handler->display->display_options['filters']['uid']['value'] = '';
  $handler->display->display_options['filters']['uid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
  $handler->display->display_options['filters']['uid']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['uid']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
  $handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
  $handler->display->display_options['filters']['uid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['path'] = 'admin/reports/salesforce/donations';
  $views[$view->name] = $view;
  /**
   * View sbv_page_templates
   */
  $view = new view();
  $view -> name = 'sbv_page_templates';
  $view -> description = '';
  $view -> tag = 'default';
  $view -> base_table = 'node';
  $view -> human_name = 'Springboard Page Templates';
  $view -> core = 7;
  $view -> api_version = '3.0';
  $view -> disabled = FALSE;
  /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view -> new_display('default', 'Master', 'default');
  $handler -> display -> display_options['title'] = 'Page Templates';
  $handler -> display -> display_options['use_more_always'] = FALSE;
  $handler -> display -> display_options['access']['type'] = 'perm';
  $handler -> display -> display_options['access']['perm'] = 'edit any page_wrapper content';
  $handler -> display -> display_options['cache']['type'] = 'none';
  $handler -> display -> display_options['query']['type'] = 'views_query';
  $handler -> display -> display_options['exposed_form']['type'] = 'basic';
  $handler -> display -> display_options['pager']['type'] = 'full';
  $handler -> display -> display_options['pager']['options']['items_per_page'] = '10';
  $handler -> display -> display_options['style_plugin'] = 'table';
  $handler -> display -> display_options['style_options']['columns'] = array(
    'title' => 'title',
    'status' => 'status',
    'created' => 'created',
    'clone_node' => 'edit_node',
    'delete_node' => 'edit_node',
    'edit_node' => 'edit_node',
    'view_node' => 'edit_node',
  );
  $handler -> display -> display_options['style_options']['default'] = '-1';
  $handler -> display -> display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'delete_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => ' | ',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler -> display -> display_options['style_options']['sticky'] = TRUE;
  /* Header: Global: Text area */
  $handler -> display -> display_options['header']['area']['id'] = 'area';
  $handler -> display -> display_options['header']['area']['table'] = 'views';
  $handler -> display -> display_options['header']['area']['field'] = 'area';
  $handler -> display -> display_options['header']['area']['content'] = '<a href="/node/add/page-wrapper" class="button">Create Page Template</a>';
  $handler -> display -> display_options['header']['area']['format'] = 'full_html';
  /* Field: Content: Title */
  $handler -> display -> display_options['fields']['title']['id'] = 'title';
  $handler -> display -> display_options['fields']['title']['table'] = 'node';
  $handler -> display -> display_options['fields']['title']['field'] = 'title';
  $handler -> display -> display_options['fields']['title']['label'] = 'Template Name';
  $handler -> display -> display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler -> display -> display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Published */
  $handler -> display -> display_options['fields']['status']['id'] = 'status';
  $handler -> display -> display_options['fields']['status']['table'] = 'node';
  $handler -> display -> display_options['fields']['status']['field'] = 'status';
  $handler -> display -> display_options['fields']['status']['label'] = 'Status';
  $handler -> display -> display_options['fields']['status']['type'] = 'published-notpublished';
  $handler -> display -> display_options['fields']['status']['not'] = 0;
  /* Field: Content: Post date */
  $handler -> display -> display_options['fields']['created']['id'] = 'created';
  $handler -> display -> display_options['fields']['created']['table'] = 'node';
  $handler -> display -> display_options['fields']['created']['field'] = 'created';
  $handler -> display -> display_options['fields']['created']['label'] = 'Date Created';
  $handler -> display -> display_options['fields']['created']['date_format'] = 'custom';
  $handler -> display -> display_options['fields']['created']['custom_date_format'] = 'm/d/Y';
  /* Field: Content: Link */
  $handler -> display -> display_options['fields']['view_node']['id'] = 'view_node';
  $handler -> display -> display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['view_node']['field'] = 'view_node';
  $handler -> display -> display_options['fields']['view_node']['label'] = '';
  $handler -> display -> display_options['fields']['view_node']['element_label_colon'] = FALSE;
  /* Field: Content: Edit link */
  $handler -> display -> display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler -> display -> display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler -> display -> display_options['fields']['edit_node']['label'] = 'Action';
  /* Field: Content: Clone link */
  $handler -> display -> display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler -> display -> display_options['fields']['clone_node']['table'] = 'node';
  $handler -> display -> display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler -> display -> display_options['fields']['clone_node']['label'] = '';
  $handler -> display -> display_options['fields']['clone_node']['element_label_colon'] = FALSE;
  /* Field: Content: Delete link */
  $handler -> display -> display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler -> display -> display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler -> display -> display_options['fields']['delete_node']['label'] = '';
  $handler -> display -> display_options['fields']['delete_node']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler -> display -> display_options['sorts']['created']['id'] = 'created';
  $handler -> display -> display_options['sorts']['created']['table'] = 'node';
  $handler -> display -> display_options['sorts']['created']['field'] = 'created';
  $handler -> display -> display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Type */
  $handler -> display -> display_options['filters']['type']['id'] = 'type';
  $handler -> display -> display_options['filters']['type']['table'] = 'node';
  $handler -> display -> display_options['filters']['type']['field'] = 'type';
  $handler -> display -> display_options['filters']['type']['value'] = array('page_wrapper' => 'page_wrapper', );
  $handler -> display -> display_options['filters']['type']['group'] = 1;
  /* Filter criterion: Search: Search Terms */
  $handler -> display -> display_options['filters']['keys']['id'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['table'] = 'search_index';
  $handler -> display -> display_options['filters']['keys']['field'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['group'] = 1;
  $handler -> display -> display_options['filters']['keys']['exposed'] = TRUE;
  $handler -> display -> display_options['filters']['keys']['expose']['operator_id'] = 'keys_op';
  $handler -> display -> display_options['filters']['keys']['expose']['label'] = 'Search templates';
  $handler -> display -> display_options['filters']['keys']['expose']['operator'] = 'keys_op';
  $handler -> display -> display_options['filters']['keys']['expose']['identifier'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view -> new_display('page', 'Page', 'page');
  $handler -> display -> display_options['exposed_block'] = TRUE;
  $handler -> display -> display_options['path'] = 'admin/springboard/templates/page-templates';

  /* Display: Block */
  $handler = $view -> new_display('block', 'Block', 'block_1');
  $handler -> display -> display_options['defaults']['pager'] = FALSE;
  $handler -> display -> display_options['pager']['type'] = 'full';
  $handler -> display -> display_options['pager']['options']['items_per_page'] = '6';
  $handler -> display -> display_options['pager']['options']['offset'] = '0';
  $handler -> display -> display_options['pager']['options']['id'] = '0';
  $handler -> display -> display_options['pager']['options']['quantity'] = '5';
  $handler -> display -> display_options['defaults']['footer'] = FALSE;
  /* Footer: Global: Text area */
  $handler -> display -> display_options['footer']['area']['id'] = 'area';
  $handler -> display -> display_options['footer']['area']['table'] = 'views';
  $handler -> display -> display_options['footer']['area']['field'] = 'area';
  $handler -> display -> display_options['footer']['area']['content'] = '<a href="/admin/springboard/templates/page-templates" class="button">View All Page Templates</a>';
  $handler -> display -> display_options['footer']['area']['format'] = 'filtered_html';
  $views[$view -> name] = $view;

  $view = new view();
  $view -> name = 'sbv_email_templates';
  $view -> description = '';
  $view -> tag = 'default';
  $view -> base_table = 'node';
  $view -> human_name = 'Springboard Email Templates';
  $view -> core = 7;
  $view -> api_version = '3.0';
  $view -> disabled = FALSE;
  /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view -> new_display('default', 'Master', 'default');
  $handler -> display -> display_options['title'] = 'Email Templates';
  $handler -> display -> display_options['use_more_always'] = FALSE;
  $handler -> display -> display_options['access']['type'] = 'perm';
  $handler -> display -> display_options['access']['perm'] = 'edit any email_wrapper content';
  $handler -> display -> display_options['cache']['type'] = 'none';
  $handler -> display -> display_options['query']['type'] = 'views_query';
  $handler -> display -> display_options['exposed_form']['type'] = 'basic';
  $handler -> display -> display_options['pager']['type'] = 'full';
  $handler -> display -> display_options['pager']['options']['items_per_page'] = '10';
  $handler -> display -> display_options['style_plugin'] = 'table';
  $handler -> display -> display_options['style_options']['columns'] = array(
    'title' => 'title',
    'status' => 'status',
    'created' => 'created',
    'clone_node' => 'edit_node',
    'delete_node' => 'edit_node',
    'edit_node' => 'edit_node',
    'view_node' => 'edit_node',
  );
  $handler -> display -> display_options['style_options']['default'] = '-1';
  $handler -> display -> display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'clone_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'delete_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => ' | ',
      'empty_column' => 0,
    ),
    'view_node' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler -> display -> display_options['style_options']['sticky'] = TRUE;
  /* Header: Global: Text area */
  $handler -> display -> display_options['header']['area']['id'] = 'area';
  $handler -> display -> display_options['header']['area']['table'] = 'views';
  $handler -> display -> display_options['header']['area']['field'] = 'area';
  $handler -> display -> display_options['header']['area']['content'] = '<a href="/node/add/email-wrapper" class="button">Create Email Template</a>';
  $handler -> display -> display_options['header']['area']['format'] = 'full_html';
  /* Field: Content: Title */
  $handler -> display -> display_options['fields']['title']['id'] = 'title';
  $handler -> display -> display_options['fields']['title']['table'] = 'node';
  $handler -> display -> display_options['fields']['title']['field'] = 'title';
  $handler -> display -> display_options['fields']['title']['label'] = 'Template Name';
  $handler -> display -> display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler -> display -> display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Published */
  $handler -> display -> display_options['fields']['status']['id'] = 'status';
  $handler -> display -> display_options['fields']['status']['table'] = 'node';
  $handler -> display -> display_options['fields']['status']['field'] = 'status';
  $handler -> display -> display_options['fields']['status']['label'] = 'Status';
  $handler -> display -> display_options['fields']['status']['type'] = 'published-notpublished';
  $handler -> display -> display_options['fields']['status']['not'] = 0;
  /* Field: Content: Post date */
  $handler -> display -> display_options['fields']['created']['id'] = 'created';
  $handler -> display -> display_options['fields']['created']['table'] = 'node';
  $handler -> display -> display_options['fields']['created']['field'] = 'created';
  $handler -> display -> display_options['fields']['created']['label'] = 'Date Created';
  $handler -> display -> display_options['fields']['created']['date_format'] = 'custom';
  $handler -> display -> display_options['fields']['created']['custom_date_format'] = 'm/d/Y';
  /* Field: Content: Link */
  $handler -> display -> display_options['fields']['view_node']['id'] = 'view_node';
  $handler -> display -> display_options['fields']['view_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['view_node']['field'] = 'view_node';
  $handler -> display -> display_options['fields']['view_node']['label'] = '';
  $handler -> display -> display_options['fields']['view_node']['element_label_colon'] = FALSE;
  /* Field: Content: Edit link */
  $handler -> display -> display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler -> display -> display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler -> display -> display_options['fields']['edit_node']['label'] = 'Action';
  /* Field: Content: Clone link */
  $handler -> display -> display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler -> display -> display_options['fields']['clone_node']['table'] = 'node';
  $handler -> display -> display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler -> display -> display_options['fields']['clone_node']['label'] = '';
  $handler -> display -> display_options['fields']['clone_node']['element_label_colon'] = FALSE;
  /* Field: Content: Delete link */
  $handler -> display -> display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler -> display -> display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler -> display -> display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler -> display -> display_options['fields']['delete_node']['label'] = '';
  $handler -> display -> display_options['fields']['delete_node']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler -> display -> display_options['sorts']['created']['id'] = 'created';
  $handler -> display -> display_options['sorts']['created']['table'] = 'node';
  $handler -> display -> display_options['sorts']['created']['field'] = 'created';
  $handler -> display -> display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Type */
  $handler -> display -> display_options['filters']['type']['id'] = 'type';
  $handler -> display -> display_options['filters']['type']['table'] = 'node';
  $handler -> display -> display_options['filters']['type']['field'] = 'type';
  $handler -> display -> display_options['filters']['type']['value'] = array('email_wrapper' => 'email_wrapper', );
  $handler -> display -> display_options['filters']['type']['group'] = 1;
  /* Filter criterion: Search: Search Terms */
  $handler -> display -> display_options['filters']['keys']['id'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['table'] = 'search_index';
  $handler -> display -> display_options['filters']['keys']['field'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['group'] = 1;
  $handler -> display -> display_options['filters']['keys']['exposed'] = TRUE;
  $handler -> display -> display_options['filters']['keys']['expose']['operator_id'] = 'keys_op';
  $handler -> display -> display_options['filters']['keys']['expose']['label'] = 'Search templates';
  $handler -> display -> display_options['filters']['keys']['expose']['operator'] = 'keys_op';
  $handler -> display -> display_options['filters']['keys']['expose']['identifier'] = 'keys';
  $handler -> display -> display_options['filters']['keys']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view -> new_display('page', 'Page', 'page');
  $handler -> display -> display_options['exposed_block'] = TRUE;
  $handler -> display -> display_options['path'] = 'admin/springboard/templates/email-templates';

  /* Display: Block */
  $handler = $view -> new_display('block', 'Block', 'block_1');
  $handler -> display -> display_options['defaults']['pager'] = FALSE;
  $handler -> display -> display_options['pager']['type'] = 'full';
  $handler -> display -> display_options['pager']['options']['items_per_page'] = '6';
  $handler -> display -> display_options['pager']['options']['offset'] = '0';
  $handler -> display -> display_options['pager']['options']['id'] = '0';
  $handler -> display -> display_options['pager']['options']['quantity'] = '5';
  $handler -> display -> display_options['defaults']['footer'] = FALSE;
  /* Footer: Global: Text area */
  $handler -> display -> display_options['footer']['area']['id'] = 'area';
  $handler -> display -> display_options['footer']['area']['table'] = 'views';
  $handler -> display -> display_options['footer']['area']['field'] = 'area';
  $handler -> display -> display_options['footer']['area']['content'] = '<a href="/admin/springboard/templates/email-templates" class="button">View All Email Templates</a>';
  $handler -> display -> display_options['footer']['area']['format'] = 'filtered_html';
  $views[$view -> name] = $view;

  /**
   * View sbv_dblog
   */

  $view = new view();
  $view->name = 'sbv_dblog';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'watchdog';
  $view->human_name = 'Recent log messages extended';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Springboard log messages';
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access site reports';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'watchdog_table';
  $handler->display->display_options['style_options']['columns'] = array(
    'severity' => 'severity',
    'timestamp' => 'timestamp',
    'type' => 'type',
    'message' => 'message',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'severity' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'timestamp' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'message' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['style_options']['watchdog_table_type'] = 1;
  $handler->display->display_options['style_options']['watchdog_table_severity'] = 1;
  /* Relationship: Watchdog: User */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'watchdog';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  /* Field: Watchdog: Severity */
  $handler->display->display_options['fields']['severity']['id'] = 'severity';
  $handler->display->display_options['fields']['severity']['table'] = 'watchdog';
  $handler->display->display_options['fields']['severity']['field'] = 'severity';
  $handler->display->display_options['fields']['severity']['watchdog_severity_icon'] = 0;
  /* Field: Watchdog: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'watchdog';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  /* Field: Watchdog: Message */
  $handler->display->display_options['fields']['message']['id'] = 'message';
  $handler->display->display_options['fields']['message']['table'] = 'watchdog';
  $handler->display->display_options['fields']['message']['field'] = 'message';
  $handler->display->display_options['fields']['message']['watchdog_message_format'] = 1;
  $handler->display->display_options['fields']['message']['watchdog_message_link'] = 0;
  /* Field: Watchdog: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'watchdog';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'long';
  /* Sort criterion: Watchdog: Timestamp */
  $handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['table'] = 'watchdog';
  $handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';
  /* Filter criterion: Watchdog: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'watchdog';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'fundraiser_cron' => 'fundraiser_cron',
    'fundraiser_sustainers' => 'fundraiser_sustainers',
    'springboard_config' => 'springboard_config',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  $handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['expose']['multiple'] = TRUE;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/springboard/reports/dblog';

  /* Display: Springboard Dashboard Log */
  $handler = $view->new_display('block', 'Springboard Dashboard Log', 'block_1');
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'More Log Entries';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '5';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'severity',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Watchdog: Severity */
  $handler->display->display_options['fields']['severity']['id'] = 'severity';
  $handler->display->display_options['fields']['severity']['table'] = 'watchdog';
  $handler->display->display_options['fields']['severity']['field'] = 'severity';
  $handler->display->display_options['fields']['severity']['label'] = '';
  $handler->display->display_options['fields']['severity']['exclude'] = TRUE;
  $handler->display->display_options['fields']['severity']['element_type'] = '0';
  $handler->display->display_options['fields']['severity']['element_label_type'] = '0';
  $handler->display->display_options['fields']['severity']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['severity']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['severity']['watchdog_severity_icon'] = 0;
  /* Field: Watchdog: Timestamp */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'watchdog';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['label'] = '';
  $handler->display->display_options['fields']['timestamp']['exclude'] = TRUE;
  $handler->display->display_options['fields']['timestamp']['element_type'] = '0';
  $handler->display->display_options['fields']['timestamp']['element_label_type'] = '0';
  $handler->display->display_options['fields']['timestamp']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['timestamp']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'time ago';
  /* Field: Watchdog: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'watchdog';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  $handler->display->display_options['fields']['type']['label'] = '';
  $handler->display->display_options['fields']['type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['type']['element_type'] = '0';
  $handler->display->display_options['fields']['type']['element_label_type'] = '0';
  $handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['type']['element_wrapper_type'] = '0';
  /* Field: Watchdog: Message */
  $handler->display->display_options['fields']['message']['id'] = 'message';
  $handler->display->display_options['fields']['message']['table'] = 'watchdog';
  $handler->display->display_options['fields']['message']['field'] = 'message';
  $handler->display->display_options['fields']['message']['label'] = '';
  $handler->display->display_options['fields']['message']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['message']['alter']['text'] = '[type]: [message] <em>[timestamp]</em>';
  $handler->display->display_options['fields']['message']['alter']['max_length'] = '240';
  $handler->display->display_options['fields']['message']['element_type'] = '0';
  $handler->display->display_options['fields']['message']['element_label_type'] = '0';
  $handler->display->display_options['fields']['message']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['message']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['message']['watchdog_message_format'] = 0;
  $handler->display->display_options['fields']['message']['watchdog_message_link'] = 1;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Watchdog: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'watchdog';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'fundraiser_cron' => 'fundraiser_cron',
    'fundraiser_sustainers' => 'fundraiser_sustainers',
    'springboard_config' => 'springboard_config',
    'capwiz_connect' => 'capwiz_connect',
    'commerce_psi' => 'commerce_psi',
    'commerce_litle' => 'commerce_litle',
    'dynamic gift strings' => 'dynamic gift strings',
    'fundraiser' => 'fundraiser',
    'fundraiser_commerce' => 'fundraiser_commerce',
    'fundraiser_webform' => 'fundraiser_webform',
    'fundraiser_comment' => 'fundraiser_comment',
    'Market Source' => 'Market Source',
    'salesforce' => 'salesforce',
    'Salesforce Pull' => 'Salesforce Pull',
    'salesforce_push' => 'salesforce_push',
    'salesforce_soap' => 'salesforce_soap',
    'salesforce_sync' => 'salesforce_sync',
    'sb_social' => 'sb_social',
    'Secure Prepopulate' => 'Secure Prepopulate',
    'webform_dupe_cop' => 'webform_dupe_cop',
    'webform_goals' => 'webform_goals',
    'webform_user' => 'webform_user',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  $handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['expose']['multiple'] = TRUE;

  $views[$view -> name] = $view;

  $view = new view();
  $view->name = 'salesforce_soap_errors';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'salesforce_log_item';
  $view->human_name = 'salesforce_soap_errors';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Errors for Unsynched Content';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view salesforce batch logs';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'drupal_id' => 'drupal_id',
    'error_code' => 'error_code',
    'object_type' => 'object_type',
    'message' => 'message',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'drupal_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'error_code' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_type' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'message' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Salesforce item log: Drupal ID */
  $handler->display->display_options['fields']['drupal_id']['id'] = 'drupal_id';
  $handler->display->display_options['fields']['drupal_id']['table'] = 'salesforce_log_item';
  $handler->display->display_options['fields']['drupal_id']['field'] = 'drupal_id';
  /* Field: Salesforce item log: Object type */
  $handler->display->display_options['fields']['object_type']['id'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['table'] = 'salesforce_log_item';
  $handler->display->display_options['fields']['object_type']['field'] = 'object_type';
  /* Field: Salesforce item log: Message */
  $handler->display->display_options['fields']['message']['id'] = 'message';
  $handler->display->display_options['fields']['message']['table'] = 'salesforce_log_item';
  $handler->display->display_options['fields']['message']['field'] = 'message';
  $handler->display->display_options['fields']['message']['label'] = 'Explanation';
  /* Sort criterion: Salesforce item log: Log Item ID */
  $handler->display->display_options['sorts']['lid']['id'] = 'lid';
  $handler->display->display_options['sorts']['lid']['table'] = 'salesforce_log_item';
  $handler->display->display_options['sorts']['lid']['field'] = 'lid';
  $handler->display->display_options['sorts']['lid']['order'] = 'DESC';
  /* Filter criterion: Salesforce item log: Error code */
  $handler->display->display_options['filters']['error_code']['id'] = 'error_code';
  $handler->display->display_options['filters']['error_code']['table'] = 'salesforce_log_item';
  $handler->display->display_options['filters']['error_code']['field'] = 'error_code';
  $handler->display->display_options['filters']['error_code']['value'] = 'Client';
  /* Filter criterion: Salesforce item log: Message */
  $handler->display->display_options['filters']['message']['id'] = 'message';
  $handler->display->display_options['filters']['message']['table'] = 'salesforce_log_item';
  $handler->display->display_options['filters']['message']['field'] = 'message';
  $handler->display->display_options['filters']['message']['operator'] = 'contains';
  $handler->display->display_options['filters']['message']['value'] = 'SOAP-ERROR';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/springboard/reports/salesforce-soap-errors';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Unsynched Errors';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  /* Display: Unsynched Errors Block */
  $handler = $view->new_display('block', 'Unsynched Errors Block', 'block');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Errors for Unsynched Content';
  $handler->display->display_options['defaults']['use_more'] = FALSE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['defaults']['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['defaults']['use_more_text'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'See the full Error Message Guide';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '3';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['block_description'] = 'Unsynched Errors';

  return $views;
}
