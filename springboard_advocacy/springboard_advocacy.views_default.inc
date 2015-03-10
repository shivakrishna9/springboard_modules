<?php

/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_advocacy_views_default_views() {


/**
 * Alerts Administration
 */
  $view = new view();
  $view->name = 'sba_alerts';
  $view->description = 'Action management screens';
  $view->tag = 'Advocacy';
  $view->base_table = 'node';
  $view->human_name = 'SBA Actions';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Site Actions';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer springboard advocacy';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'type',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'field_webform_user_internal_name' => 'field_webform_user_internal_name',
    'created' => 'created',
    'type' => 'type',
    'webform_submission_count_node' => 'webform_submission_count_node',
    'status' => 'status',
    'edit_node' => 'edit_node',
    'delete_node' => 'edit_node',
    'clone_node' => 'edit_node',
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
    'field_webform_user_internal_name' => array(
      'sortable' => 1,
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
    'type' => array(
      'sortable' => 0,
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
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'edit_node' => array(
      'align' => '',
      'separator' => ' | ',
      'empty_column' => 0,
    ),
    'delete_node' => array(
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
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Internal Name */
  $handler->display->display_options['fields']['field_webform_user_internal_name']['id'] = 'field_webform_user_internal_name';
  $handler->display->display_options['fields']['field_webform_user_internal_name']['table'] = 'field_data_field_webform_user_internal_name';
  $handler->display->display_options['fields']['field_webform_user_internal_name']['field'] = 'field_webform_user_internal_name';
  /* Field: Content: Post date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'node';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Date';
  $handler->display->display_options['fields']['created']['date_format'] = 'custom';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'm/d/Y';
  /* Field: Content: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'node';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  $handler->display->display_options['fields']['type']['label'] = '';
  $handler->display->display_options['fields']['type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['type']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['type']['alter']['text'] = '<h3>[type]</h3>';
  $handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
  /* Field: Webform: Webform submission count */
  $handler->display->display_options['fields']['webform_submission_count_node']['id'] = 'webform_submission_count_node';
  $handler->display->display_options['fields']['webform_submission_count_node']['table'] = 'node';
  $handler->display->display_options['fields']['webform_submission_count_node']['field'] = 'webform_submission_count_node';
  /* Field: Content: Published */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'node';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  $handler->display->display_options['fields']['status']['type'] = 'published-notpublished';
  $handler->display->display_options['fields']['status']['not'] = 0;
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = 'Actions';
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['label'] = '';
  $handler->display->display_options['fields']['delete_node']['element_label_colon'] = FALSE;
  /* Field: Content: Clone link */
  $handler->display->display_options['fields']['clone_node']['id'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['table'] = 'node';
  $handler->display->display_options['fields']['clone_node']['field'] = 'clone_node';
  $handler->display->display_options['fields']['clone_node']['label'] = '';
  $handler->display->display_options['fields']['clone_node']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Type */
  $handler->display->display_options['arguments']['type']['id'] = 'type';
  $handler->display->display_options['arguments']['type']['table'] = 'node';
  $handler->display->display_options['arguments']['type']['field'] = 'type';
  $handler->display->display_options['arguments']['type']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['type']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['type']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['type']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['type']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['type']['limit'] = '0';
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
    'sba_message_action' => 'sba_message_action',
    'springboard_petition' => 'springboard_petition',
  );

  /* Display: Actions Paged Block */
  $handler = $view->new_display('block', 'Actions Paged Block', 'block_1');
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['pager']['options']['offset'] = '0';

  /* Display: Actions Show All Block */
  $handler = $view->new_display('block', 'Actions Show All Block', 'block_2');
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $views[$view->name] = $view;
  /*******************************************************************
 * Target Admin
 */
  $view = new view();
  $view->name = 'targets';
  $view->description = 'Advocacy Custom Target List';
  $view->tag = 'Advocacy';
  $view->base_table = 'advocacy';
  $view->human_name = 'Sba Targets';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Targets';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer targets';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['pager']['type'] = 'springboard_advocacy_views_plugin_pager';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'id' => 'id',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'id' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Advocacy: Salutation */
  $handler->display->display_options['fields']['salutation']['id'] = 'salutation';
  $handler->display->display_options['fields']['salutation']['table'] = 'advocacy';
  $handler->display->display_options['fields']['salutation']['field'] = 'salutation';
  /* Field: Advocacy: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Advocacy: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Advocacy: Email */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'advocacy';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  /* Field: Advocacy: Politcal Party */
  $handler->display->display_options['fields']['party']['id'] = 'party';
  $handler->display->display_options['fields']['party']['table'] = 'advocacy';
  $handler->display->display_options['fields']['party']['field'] = 'party';
  /* Field: Advocacy: Role */
  $handler->display->display_options['fields']['role']['id'] = 'role';
  $handler->display->display_options['fields']['role']['table'] = 'advocacy';
  $handler->display->display_options['fields']['role']['field'] = 'role';
  /* Field: Advocacy: State */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'advocacy';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  /* Filter criterion: Advocacy: Search By Type */
  $handler->display->display_options['filters']['search_class_name']['id'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_class_name']['field'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['value'] = array(
    'Target' => 'Target',
  );
  $handler->display->display_options['filters']['search_class_name']['expose']['operator_id'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['label'] = 'Search By Type';
  $handler->display->display_options['filters']['search_class_name']['expose']['operator'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['identifier'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Last Name */
  $handler->display->display_options['filters']['last_name']['id'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['last_name']['field'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['last_name']['expose']['operator_id'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['label'] = 'Last Name';
  $handler->display->display_options['filters']['last_name']['expose']['operator'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['identifier'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Search By Gender */
  $handler->display->display_options['filters']['search_gender']['id'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_gender']['field'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['value'] = array(
    'M' => 'M',
    'F' => 'F',
  );
  $handler->display->display_options['filters']['search_gender']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['operator_id'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['expose']['operator'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_gender']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Male',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
      ),
    ),
    2 => array(
      'title' => 'Female',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By State */
  $handler->display->display_options['filters']['search_state']['id'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_state']['field'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['value'] = array(
    'AL' => 'AL',
    'AK' => 'AK',
    'AZ' => 'AZ',
    'AR' => 'AR',
    'CA' => 'CA',
    'CO' => 'CO',
    'CT' => 'CT',
    'DE' => 'DE',
    'DC' => 'DC',
    'FL' => 'FL',
    'GA' => 'GA',
    'HI' => 'HI',
    'ID' => 'ID',
    'IL' => 'IL',
    'IN' => 'IN',
    'IA' => 'IA',
    'KS' => 'KS',
    'KY' => 'KY',
    'LA' => 'LA',
    'ME' => 'ME',
    'MD' => 'MD',
    'MA' => 'MA',
    'MI' => 'MI',
    'MN' => 'MN',
    'MS' => 'MS',
    'MO' => 'MO',
    'MT' => 'MT',
    'NE' => 'NE',
    'NV' => 'NV',
    'NH' => 'NH',
    'NJ' => 'NJ',
    'NM' => 'NM',
    'NY' => 'NY',
    'NC' => 'NC',
    'ND' => 'ND',
    'OH' => 'OH',
    'OK' => 'OK',
    'OR' => 'OR',
    'PA' => 'PA',
    'RI' => 'RI',
    'SC' => 'SC',
    'SD' => 'SD',
    'TN' => 'TN',
    'TX' => 'TX',
    'UT' => 'UT',
    'VT' => 'VT',
    'VA' => 'VA',
    'WA' => 'WA',
    'WV' => 'WV',
    'WI' => 'WI',
    'WY' => 'WY',
  );
  $handler->display->display_options['filters']['search_state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_state']['expose']['operator_id'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['label'] = 'Search By State';
  $handler->display->display_options['filters']['search_state']['expose']['operator'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['identifier'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_state']['expose']['reduce'] = TRUE;
  /* Filter criterion: Advocacy: Search By Branch */
  $handler->display->display_options['filters']['search_role']['id'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_role']['field'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['value'] = array(
    'SR|SS' => 'SR|SS',
    'FR|FS' => 'FR|FS',
  );
  $handler->display->display_options['filters']['search_role']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_role']['expose']['operator_id'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['expose']['operator'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_role']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_role']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['group_info']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_role']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'State',
      'operator' => 'in',
      'value' => array(
        'SR|SS' => 'SR|SS',
        'all' => 0,
        'FR|FS' => 0,
      ),
    ),
    2 => array(
      'title' => 'Federal',
      'operator' => 'in',
      'value' => array(
        'FR|FS' => 'FR|FS',
        'all' => 0,
        'SR|SS' => 0,
      ),
    ),
  );

  /* Display: Search Legislators */
  $handler = $view->new_display('page', 'Search Legislators', 'page_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Search Legislators';
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'salutation' => 'salutation',
    'first_name' => 'first_name',
    'last_name' => 'last_name',
    'email' => 'email',
    'party' => 'party',
    'role' => 'role',
    'state' => 'state',
  );
  $handler->display->display_options['style_options']['default'] = 'last_name';
  $handler->display->display_options['style_options']['info'] = array(
    'salutation' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'first_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'last_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'email' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'party' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'role' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'state' => array(
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
  /* Field: Advocacy: Salutation */
  $handler->display->display_options['fields']['salutation']['id'] = 'salutation';
  $handler->display->display_options['fields']['salutation']['table'] = 'advocacy';
  $handler->display->display_options['fields']['salutation']['field'] = 'salutation';
  /* Field: Advocacy: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Advocacy: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Advocacy: Email */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'advocacy';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  /* Field: Advocacy: Politcal Party */
  $handler->display->display_options['fields']['party']['id'] = 'party';
  $handler->display->display_options['fields']['party']['table'] = 'advocacy';
  $handler->display->display_options['fields']['party']['field'] = 'party';
  /* Field: Advocacy: Role */
  $handler->display->display_options['fields']['role']['id'] = 'role';
  $handler->display->display_options['fields']['role']['table'] = 'advocacy';
  $handler->display->display_options['fields']['role']['field'] = 'role';
  /* Field: Advocacy: State */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'advocacy';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Advocacy: Search By Type */
  $handler->display->display_options['filters']['search_class_name']['id'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_class_name']['field'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['value'] = array(
    'Legislator' => 'Legislator',
  );
  $handler->display->display_options['filters']['search_class_name']['group'] = 1;
  $handler->display->display_options['filters']['search_class_name']['expose']['operator_id'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['label'] = 'Search By Type';
  $handler->display->display_options['filters']['search_class_name']['expose']['operator'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['identifier'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Last Name */
  $handler->display->display_options['filters']['last_name']['id'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['last_name']['field'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['group'] = 1;
  $handler->display->display_options['filters']['last_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['last_name']['expose']['operator_id'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['label'] = 'Last Name';
  $handler->display->display_options['filters']['last_name']['expose']['operator'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['identifier'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Search By Gender */
  $handler->display->display_options['filters']['search_gender']['id'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_gender']['field'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['value'] = array(
    'M' => 'M',
    'F' => 'F',
  );
  $handler->display->display_options['filters']['search_gender']['group'] = 1;
  $handler->display->display_options['filters']['search_gender']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['operator_id'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['expose']['operator'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['search_gender']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_gender']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Male',
      'operator' => 'in',
      'value' => array(
        'M' => 'M',
        'all' => 0,
        'F' => 0,
      ),
    ),
    2 => array(
      'title' => 'Female',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By State */
  $handler->display->display_options['filters']['search_state']['id'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_state']['field'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['value'] = array(
    'AL' => 'AL',
    'AK' => 'AK',
    'AZ' => 'AZ',
    'AR' => 'AR',
    'CA' => 'CA',
    'CO' => 'CO',
    'CT' => 'CT',
    'DE' => 'DE',
    'DC' => 'DC',
    'FL' => 'FL',
    'GA' => 'GA',
    'HI' => 'HI',
    'ID' => 'ID',
    'IL' => 'IL',
    'IN' => 'IN',
    'IA' => 'IA',
    'KS' => 'KS',
    'KY' => 'KY',
    'LA' => 'LA',
    'ME' => 'ME',
    'MD' => 'MD',
    'MA' => 'MA',
    'MI' => 'MI',
    'MN' => 'MN',
    'MS' => 'MS',
    'MO' => 'MO',
    'MT' => 'MT',
    'NE' => 'NE',
    'NV' => 'NV',
    'NH' => 'NH',
    'NJ' => 'NJ',
    'NM' => 'NM',
    'NY' => 'NY',
    'NC' => 'NC',
    'ND' => 'ND',
    'OH' => 'OH',
    'OK' => 'OK',
    'OR' => 'OR',
    'PA' => 'PA',
    'RI' => 'RI',
    'SC' => 'SC',
    'SD' => 'SD',
    'TN' => 'TN',
    'TX' => 'TX',
    'UT' => 'UT',
    'VT' => 'VT',
    'VA' => 'VA',
    'WA' => 'WA',
    'WV' => 'WV',
    'WI' => 'WI',
    'WY' => 'WY',
  );
  $handler->display->display_options['filters']['search_state']['group'] = 1;
  $handler->display->display_options['filters']['search_state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_state']['expose']['operator_id'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['label'] = 'Search By State';
  $handler->display->display_options['filters']['search_state']['expose']['operator'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['identifier'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_state']['expose']['reduce'] = TRUE;
  /* Filter criterion: Advocacy: Search By District */
  $handler->display->display_options['filters']['search_district_name']['id'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_district_name']['field'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['group'] = 1;
  $handler->display->display_options['filters']['search_district_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_district_name']['expose']['operator_id'] = 'search_district_name_op';
  $handler->display->display_options['filters']['search_district_name']['expose']['label'] = 'Search By District';
  $handler->display->display_options['filters']['search_district_name']['expose']['operator'] = 'search_district_name_op';
  $handler->display->display_options['filters']['search_district_name']['expose']['identifier'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Advocacy: Search By Branch */
  $handler->display->display_options['filters']['search_role']['id'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_role']['field'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['group'] = 1;
  $handler->display->display_options['filters']['search_role']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_role']['expose']['operator_id'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['expose']['operator'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['group_info']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_role']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'State',
      'operator' => 'in',
      'value' => array(
        'SR|SS' => 'SR|SS',
        'all' => 0,
        'FR|FS' => 0,
      ),
    ),
    2 => array(
      'title' => 'Federal',
      'operator' => 'in',
      'value' => array(
        'FR|FS' => 'FR|FS',
        'all' => 0,
        'SR|SS' => 0,
      ),
    ),
  );
  $handler->display->display_options['path'] = 'admin/springboard/advocacy/legislators';

  /* Display: Search All Targets */
  $handler = $view->new_display('page', 'Search All Targets', 'page_2');
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Advocacy: Search By Type */
  $handler->display->display_options['filters']['search_class_name']['id'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_class_name']['field'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['value'] = array(
    'Legislator' => 'Legislator',
  );
  $handler->display->display_options['filters']['search_class_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['expose']['operator_id'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['label'] = 'Search By Type';
  $handler->display->display_options['filters']['search_class_name']['expose']['operator'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['identifier'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['group_info']['label'] = 'Search By Type';
  $handler->display->display_options['filters']['search_class_name']['group_info']['identifier'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_class_name']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Custom Target',
      'operator' => 'in',
      'value' => array(
        'Target' => 'Target',
        'all' => 0,
        'Legislator' => 0,
        'group' => 0,
      ),
    ),
    2 => array(
      'title' => 'Legislator',
      'operator' => 'in',
      'value' => array(
        'Legislator' => 'Legislator',
        'all' => 0,
        'Target' => 0,
        'group' => 0,
      ),
    ),
    3 => array(
      'title' => 'Group',
      'operator' => 'in',
      'value' => array(
        'group' => 'group',
        'all' => 0,
        'Target' => 0,
        'Legislator' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Last Name */
  $handler->display->display_options['filters']['last_name']['id'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['last_name']['field'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['last_name']['expose']['operator_id'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['label'] = 'Last Name';
  $handler->display->display_options['filters']['last_name']['expose']['operator'] = 'last_name_op';
  $handler->display->display_options['filters']['last_name']['expose']['identifier'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Search By Gender */
  $handler->display->display_options['filters']['search_gender']['id'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_gender']['field'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['value'] = array(
    'M' => 'M',
    'F' => 'F',
  );
  $handler->display->display_options['filters']['search_gender']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['operator_id'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['expose']['operator'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_gender']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_gender']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Male',
      'operator' => 'in',
      'value' => array(
        'M' => 'M',
        'all' => 0,
        'F' => 0,
      ),
    ),
    2 => array(
      'title' => 'Female',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By State */
  $handler->display->display_options['filters']['search_state']['id'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_state']['field'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['value'] = array(
    'AL' => 'AL',
    'AK' => 'AK',
    'AZ' => 'AZ',
    'AR' => 'AR',
    'CA' => 'CA',
    'CO' => 'CO',
    'CT' => 'CT',
    'DE' => 'DE',
    'DC' => 'DC',
    'FL' => 'FL',
    'GA' => 'GA',
    'HI' => 'HI',
    'ID' => 'ID',
    'IL' => 'IL',
    'IN' => 'IN',
    'IA' => 'IA',
    'KS' => 'KS',
    'KY' => 'KY',
    'LA' => 'LA',
    'ME' => 'ME',
    'MD' => 'MD',
    'MA' => 'MA',
    'MI' => 'MI',
    'MN' => 'MN',
    'MS' => 'MS',
    'MO' => 'MO',
    'MT' => 'MT',
    'NE' => 'NE',
    'NV' => 'NV',
    'NH' => 'NH',
    'NJ' => 'NJ',
    'NM' => 'NM',
    'NY' => 'NY',
    'NC' => 'NC',
    'ND' => 'ND',
    'OH' => 'OH',
    'OK' => 'OK',
    'OR' => 'OR',
    'PA' => 'PA',
    'RI' => 'RI',
    'SC' => 'SC',
    'SD' => 'SD',
    'TN' => 'TN',
    'TX' => 'TX',
    'UT' => 'UT',
    'VT' => 'VT',
    'VA' => 'VA',
    'WA' => 'WA',
    'WV' => 'WV',
    'WI' => 'WI',
    'WY' => 'WY',
  );
  $handler->display->display_options['filters']['search_state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_state']['expose']['operator_id'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['label'] = 'Search By State';
  $handler->display->display_options['filters']['search_state']['expose']['operator'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['identifier'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_state']['expose']['reduce'] = TRUE;
  /* Filter criterion: Advocacy: Search By Branch */
  $handler->display->display_options['filters']['search_role']['id'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_role']['field'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_role']['expose']['operator_id'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['expose']['operator'] = 'search_role_op';
  $handler->display->display_options['filters']['search_role']['expose']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['label'] = 'Search By Level';
  $handler->display->display_options['filters']['search_role']['group_info']['identifier'] = 'search_role';
  $handler->display->display_options['filters']['search_role']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_role']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'State',
      'operator' => 'in',
      'value' => array(
        'SR|SS' => 'SR|SS',
        'all' => 0,
        'FR|FS' => 0,
      ),
    ),
    2 => array(
      'title' => 'Federal',
      'operator' => 'in',
      'value' => array(
        'FR|FS' => 'FR|FS',
        'all' => 0,
        'SR|SS' => 0,
      ),
    ),
  );
  $handler->display->display_options['path'] = 'admin/config/springboard/advocacy/targets';

  /* Display: Search Custom Targets Block */
  $handler = $view->new_display('block', 'Search Custom Targets Block', 'block_1');
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['defaults']['exposed_form'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Advocacy: Salutation */
  $handler->display->display_options['fields']['salutation']['id'] = 'salutation';
  $handler->display->display_options['fields']['salutation']['table'] = 'advocacy';
  $handler->display->display_options['fields']['salutation']['field'] = 'salutation';
  /* Field: Advocacy: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Advocacy: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Advocacy: Email */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'advocacy';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  /* Field: Advocacy: Politcal Party */
  $handler->display->display_options['fields']['party']['id'] = 'party';
  $handler->display->display_options['fields']['party']['table'] = 'advocacy';
  $handler->display->display_options['fields']['party']['field'] = 'party';
  /* Field: Advocacy: Role */
  $handler->display->display_options['fields']['role']['id'] = 'role';
  $handler->display->display_options['fields']['role']['table'] = 'advocacy';
  $handler->display->display_options['fields']['role']['field'] = 'role';
  /* Field: Advocacy: State */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'advocacy';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  /* Field: Advocacy: Target Actions */
  $handler->display->display_options['fields']['target_actions']['id'] = 'target_actions';
  $handler->display->display_options['fields']['target_actions']['table'] = 'advocacy';
  $handler->display->display_options['fields']['target_actions']['field'] = 'target_actions';
  $handler->display->display_options['fields']['target_actions']['label'] = 'Actions';
  $handler->display->display_options['fields']['target_actions']['which_links'] = array(
    'edit' => 'edit',
    'delete' => 'delete',
    'view' => 0,
  );
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Advocacy: Search Multiple Target Fields by Keywords */
  $handler->display->display_options['filters']['combine']['id'] = 'combine';
  $handler->display->display_options['filters']['combine']['table'] = 'advocacy';
  $handler->display->display_options['filters']['combine']['field'] = 'combine';
  $handler->display->display_options['filters']['combine']['group'] = 1;
  $handler->display->display_options['filters']['combine']['exposed'] = TRUE;
  $handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['label'] = 'FIrst name or Last name';
  $handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
  $handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['combine']['fields'] = array(
    'first_name' => 'first_name',
    'last_name' => 'last_name',
  );
  /* Filter criterion: Advocacy: Search By Type */
  $handler->display->display_options['filters']['search_class_name']['id'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_class_name']['field'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['value'] = array(
    'Target' => 'Target',
  );
  $handler->display->display_options['filters']['search_class_name']['group'] = 1;
  $handler->display->display_options['filters']['search_class_name']['expose']['operator_id'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['label'] = 'Search By Type';
  $handler->display->display_options['filters']['search_class_name']['expose']['operator'] = 'search_class_name_op';
  $handler->display->display_options['filters']['search_class_name']['expose']['identifier'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_class_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  /* Filter criterion: Advocacy: Search By Gender */
  $handler->display->display_options['filters']['search_gender']['id'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_gender']['field'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['value'] = array(
    'M' => 'M',
    'F' => 'F',
  );
  $handler->display->display_options['filters']['search_gender']['group'] = 1;
  $handler->display->display_options['filters']['search_gender']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['operator_id'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['expose']['operator'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_gender']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Male',
      'operator' => 'in',
      'value' => array(
        'M' => 'M',
        'all' => 0,
        'F' => 0,
        0 => 0,
      ),
    ),
    2 => array(
      'title' => 'Female',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
        0 => 0,
      ),
    ),
    3 => array(
      'title' => 'Other',
      'operator' => 'in',
      'value' => array(
        0 => '0',
        'all' => 0,
        'M' => 0,
        'F' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By State */
  $handler->display->display_options['filters']['search_state']['id'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_state']['field'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['value'] = array(
    'AL' => 'AL',
    'AK' => 'AK',
    'AZ' => 'AZ',
    'AR' => 'AR',
    'CA' => 'CA',
    'CO' => 'CO',
    'CT' => 'CT',
    'DE' => 'DE',
    'DC' => 'DC',
    'FL' => 'FL',
    'GA' => 'GA',
    'HI' => 'HI',
    'ID' => 'ID',
    'IL' => 'IL',
    'IN' => 'IN',
    'IA' => 'IA',
    'KS' => 'KS',
    'KY' => 'KY',
    'LA' => 'LA',
    'ME' => 'ME',
    'MD' => 'MD',
    'MA' => 'MA',
    'MI' => 'MI',
    'MN' => 'MN',
    'MS' => 'MS',
    'MO' => 'MO',
    'MT' => 'MT',
    'NE' => 'NE',
    'NV' => 'NV',
    'NH' => 'NH',
    'NJ' => 'NJ',
    'NM' => 'NM',
    'NY' => 'NY',
    'NC' => 'NC',
    'ND' => 'ND',
    'OH' => 'OH',
    'OK' => 'OK',
    'OR' => 'OR',
    'PA' => 'PA',
    'RI' => 'RI',
    'SC' => 'SC',
    'SD' => 'SD',
    'TN' => 'TN',
    'TX' => 'TX',
    'UT' => 'UT',
    'VT' => 'VT',
    'VA' => 'VA',
    'WA' => 'WA',
    'WV' => 'WV',
    'WI' => 'WI',
    'WY' => 'WY',
  );
  $handler->display->display_options['filters']['search_state']['group'] = 1;
  $handler->display->display_options['filters']['search_state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_state']['expose']['operator_id'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['label'] = 'Search By State';
  $handler->display->display_options['filters']['search_state']['expose']['operator'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['identifier'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_state']['expose']['reduce'] = TRUE;

  /* Display: Alt FInd Targets Block */
  $handler = $view->new_display('block', 'Alt FInd Targets Block', 'block_3');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Find Targets';
  $handler->display->display_options['display_description'] = 'For use in message add form';
  $handler->display->display_options['defaults']['use_ajax'] = FALSE;
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['defaults']['exposed_form'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
  $handler->display->display_options['exposed_form']['options']['autosubmit_hide'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'salutation' => 'salutation',
    'first_name' => 'first_name',
    'last_name' => 'last_name',
    'email' => 'email',
    'party' => 'party',
    'role' => 'role',
    'state' => 'state',
    'target_actions' => 'target_actions',
    'last_name_1' => 'last_name_1',
  );
  $handler->display->display_options['style_options']['default'] = 'last_name';
  $handler->display->display_options['style_options']['info'] = array(
    'salutation' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'first_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'last_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'email' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'party' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'role' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'state' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'target_actions' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'last_name_1' => array(
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
  /* Field: Advocacy: Salutation */
  $handler->display->display_options['fields']['salutation']['id'] = 'salutation';
  $handler->display->display_options['fields']['salutation']['table'] = 'advocacy';
  $handler->display->display_options['fields']['salutation']['field'] = 'salutation';
  /* Field: Advocacy: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Advocacy: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'advocacy';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Advocacy: Email */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'advocacy';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  /* Field: Advocacy: Politcal Party */
  $handler->display->display_options['fields']['party']['id'] = 'party';
  $handler->display->display_options['fields']['party']['table'] = 'advocacy';
  $handler->display->display_options['fields']['party']['field'] = 'party';
  /* Field: Advocacy: Role */
  $handler->display->display_options['fields']['role']['id'] = 'role';
  $handler->display->display_options['fields']['role']['table'] = 'advocacy';
  $handler->display->display_options['fields']['role']['field'] = 'role';
  /* Field: Advocacy: State */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'advocacy';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  /* Field: Advocacy: Target Actions */
  $handler->display->display_options['fields']['target_actions']['id'] = 'target_actions';
  $handler->display->display_options['fields']['target_actions']['table'] = 'advocacy';
  $handler->display->display_options['fields']['target_actions']['field'] = 'target_actions';
  $handler->display->display_options['fields']['target_actions']['label'] = 'Action';
  $handler->display->display_options['fields']['target_actions']['which_links'] = array(
    'add' => 'add',
    'view' => 0,
    'edit' => 0,
    'delete' => 0,
  );
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Advocacy: Search Multiple Target Fields by Keywords */
  $handler->display->display_options['filters']['combine']['id'] = 'combine';
  $handler->display->display_options['filters']['combine']['table'] = 'advocacy';
  $handler->display->display_options['filters']['combine']['field'] = 'combine';
  $handler->display->display_options['filters']['combine']['group'] = 1;
  $handler->display->display_options['filters']['combine']['exposed'] = TRUE;
  $handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['description'] = 'First name, last name';
  $handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
  $handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
  $handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['combine']['fields'] = array(
    'first_name' => 'first_name',
    'last_name' => 'last_name',
  );
  /* Filter criterion: Advocacy: Search By State */
  $handler->display->display_options['filters']['search_state']['id'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_state']['field'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['value'] = array(
    'AL' => 'AL',
    'AK' => 'AK',
    'AZ' => 'AZ',
    'AR' => 'AR',
    'CA' => 'CA',
    'CO' => 'CO',
    'CT' => 'CT',
    'DE' => 'DE',
    'DC' => 'DC',
    'FL' => 'FL',
    'GA' => 'GA',
    'HI' => 'HI',
    'ID' => 'ID',
    'IL' => 'IL',
    'IN' => 'IN',
    'IA' => 'IA',
    'KS' => 'KS',
    'KY' => 'KY',
    'LA' => 'LA',
    'ME' => 'ME',
    'MD' => 'MD',
    'MA' => 'MA',
    'MI' => 'MI',
    'MN' => 'MN',
    'MS' => 'MS',
    'MO' => 'MO',
    'MT' => 'MT',
    'NE' => 'NE',
    'NV' => 'NV',
    'NH' => 'NH',
    'NJ' => 'NJ',
    'NM' => 'NM',
    'NY' => 'NY',
    'NC' => 'NC',
    'ND' => 'ND',
    'OH' => 'OH',
    'OK' => 'OK',
    'OR' => 'OR',
    'PA' => 'PA',
    'RI' => 'RI',
    'SC' => 'SC',
    'SD' => 'SD',
    'TN' => 'TN',
    'TX' => 'TX',
    'UT' => 'UT',
    'VT' => 'VT',
    'VA' => 'VA',
    'WA' => 'WA',
    'WV' => 'WV',
    'WI' => 'WI',
    'WY' => 'WY',
  );
  $handler->display->display_options['filters']['search_state']['group'] = 1;
  $handler->display->display_options['filters']['search_state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_state']['expose']['operator_id'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['label'] = 'State';
  $handler->display->display_options['filters']['search_state']['expose']['operator'] = 'search_state_op';
  $handler->display->display_options['filters']['search_state']['expose']['identifier'] = 'search_state';
  $handler->display->display_options['filters']['search_state']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['search_state']['expose']['reduce'] = TRUE;
  /* Filter criterion: Advocacy: Search By District */
  $handler->display->display_options['filters']['search_district_name']['id'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_district_name']['field'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['group'] = 1;
  $handler->display->display_options['filters']['search_district_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_district_name']['expose']['operator_id'] = 'search_district_name_op';
  $handler->display->display_options['filters']['search_district_name']['expose']['label'] = 'District';
  $handler->display->display_options['filters']['search_district_name']['expose']['operator'] = 'search_district_name_op';
  $handler->display->display_options['filters']['search_district_name']['expose']['identifier'] = 'search_district_name';
  $handler->display->display_options['filters']['search_district_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Advocacy: Search By Branch */
  $handler->display->display_options['filters']['search_role_1']['id'] = 'search_role_1';
  $handler->display->display_options['filters']['search_role_1']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_role_1']['field'] = 'search_role';
  $handler->display->display_options['filters']['search_role_1']['value'] = array(
    'FS' => 'FS',
    'FR' => 'FR',
    'SR' => 'SR',
    'SS' => 'SS',
    'FX' => 'FX',
    'SX' => 'SX',
  );
  $handler->display->display_options['filters']['search_role_1']['group'] = 1;
  $handler->display->display_options['filters']['search_role_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_role_1']['expose']['operator_id'] = 'search_role_1_op';
  $handler->display->display_options['filters']['search_role_1']['expose']['label'] = 'Branch';
  $handler->display->display_options['filters']['search_role_1']['expose']['operator'] = 'search_role_1_op';
  $handler->display->display_options['filters']['search_role_1']['expose']['identifier'] = 'search_role_1';
  $handler->display->display_options['filters']['search_role_1']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role_1']['expose']['remember_roles'] = array(
    2 => 0,
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_role_1']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_role_1']['group_info']['label'] = 'Branch';
  $handler->display->display_options['filters']['search_role_1']['group_info']['identifier'] = 'search_role_1';
  $handler->display->display_options['filters']['search_role_1']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_role_1']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_role_1']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Federal Executive',
      'operator' => 'in',
      'value' => array(
        'FX|SX' => 'FX|SX',
        'FX' => 'FX',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FS' => 0,
        'FR' => 0,
        'SR' => 0,
        'SS' => 0,
        'SX' => 0,
      ),
    ),
    2 => array(
      'title' => 'Federal Senate',
      'operator' => 'in',
      'value' => array(
        'FS' => 'FS',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FR' => 0,
        'SR' => 0,
        'SS' => 0,
        'FX|SX' => 0,
        'FX' => 0,
        'SX' => 0,
      ),
    ),
    3 => array(
      'title' => 'Federal House',
      'operator' => 'in',
      'value' => array(
        'FR' => 'FR',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FS' => 0,
        'SR' => 0,
        'SS' => 0,
        'FX|SX' => 0,
        'FX' => 0,
        'SX' => 0,
      ),
    ),
    4 => array(
      'title' => 'State Executive',
      'operator' => 'in',
      'value' => array(
        'SX' => 'SX',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FS' => 0,
        'FR' => 0,
        'SR' => 0,
        'SS' => 0,
        'FX|SX' => 0,
        'FX' => 0,
      ),
    ),
    5 => array(
      'title' => 'State Senate',
      'operator' => 'in',
      'value' => array(
        'SS' => 'SS',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FS' => 0,
        'FR' => 0,
        'SR' => 0,
        'FX|SX' => 0,
        'FX' => 0,
        'SX' => 0,
      ),
    ),
    6 => array(
      'title' => 'State House',
      'operator' => 'in',
      'value' => array(
        'SR' => 'SR',
        'all' => 0,
        'SR|SS' => 0,
        'FR|FS' => 0,
        'FS' => 0,
        'FR' => 0,
        'SS' => 0,
        'FX|SX' => 0,
        'FX' => 0,
        'SX' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By Party */
  $handler->display->display_options['filters']['search_party']['id'] = 'search_party';
  $handler->display->display_options['filters']['search_party']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_party']['field'] = 'search_party';
  $handler->display->display_options['filters']['search_party']['value'] = array(
    'D' => 'D',
    'R' => 'R',
    'O' => 'O',
  );
  $handler->display->display_options['filters']['search_party']['group'] = 1;
  $handler->display->display_options['filters']['search_party']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_party']['expose']['operator_id'] = 'search_party_op';
  $handler->display->display_options['filters']['search_party']['expose']['label'] = 'Party';
  $handler->display->display_options['filters']['search_party']['expose']['operator'] = 'search_party_op';
  $handler->display->display_options['filters']['search_party']['expose']['identifier'] = 'search_party';
  $handler->display->display_options['filters']['search_party']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_party']['expose']['remember_roles'] = array(
    2 => 0,
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_party']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_party']['group_info']['label'] = 'Party';
  $handler->display->display_options['filters']['search_party']['group_info']['identifier'] = 'search_party';
  $handler->display->display_options['filters']['search_party']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_party']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_party']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Democrat',
      'operator' => 'in',
      'value' => array(
        'D' => 'D',
        'all' => 0,
        'R' => 0,
        'I' => 0,
        'O' => 0,
      ),
    ),
    2 => array(
      'title' => 'Republican',
      'operator' => 'in',
      'value' => array(
        'R' => 'R',
        'all' => 0,
        'D' => 0,
        'I' => 0,
        'O' => 0,
      ),
    ),
    3 => array(
      'title' => 'Other',
      'operator' => 'in',
      'value' => array(
        'O' => 'O',
        'all' => 0,
        'D' => 0,
        'R' => 0,
        'I' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By Gender */
  $handler->display->display_options['filters']['search_gender']['id'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_gender']['field'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['value'] = array(
    'M' => 'M',
    'F' => 'F',
  );
  $handler->display->display_options['filters']['search_gender']['group'] = 1;
  $handler->display->display_options['filters']['search_gender']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['operator_id'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['label'] = 'Search By Gender';
  $handler->display->display_options['filters']['search_gender']['expose']['operator'] = 'search_gender_op';
  $handler->display->display_options['filters']['search_gender']['expose']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_gender']['expose']['reduce'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['label'] = 'Gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['identifier'] = 'search_gender';
  $handler->display->display_options['filters']['search_gender']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_gender']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Male',
      'operator' => 'in',
      'value' => array(
        'M' => 'M',
        'all' => 0,
        'F' => 0,
        'O' => 0,
      ),
    ),
    2 => array(
      'title' => 'Female',
      'operator' => 'in',
      'value' => array(
        'F' => 'F',
        'all' => 0,
        'M' => 0,
        'O' => 0,
      ),
    ),
    3 => array(
      'title' => 'Other',
      'operator' => 'in',
      'value' => array(
        'O' => 'O',
        'all' => 0,
        'M' => 0,
        'F' => 0,
      ),
    ),
  );
  /* Filter criterion: Advocacy: Search By Social Media Presence */
  $handler->display->display_options['filters']['search_social']['id'] = 'search_social';
  $handler->display->display_options['filters']['search_social']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_social']['field'] = 'search_social';
  $handler->display->display_options['filters']['search_social']['exposed'] = TRUE;
  $handler->display->display_options['filters']['search_social']['expose']['operator_id'] = 'search_social_op';
  $handler->display->display_options['filters']['search_social']['expose']['label'] = 'Search By Social Media Presence';
  $handler->display->display_options['filters']['search_social']['expose']['operator'] = 'search_social_op';
  $handler->display->display_options['filters']['search_social']['expose']['identifier'] = 'search_social';
  $handler->display->display_options['filters']['search_social']['expose']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_social']['is_grouped'] = TRUE;
  $handler->display->display_options['filters']['search_social']['group_info']['label'] = 'Social Media';
  $handler->display->display_options['filters']['search_social']['group_info']['identifier'] = 'search_social';
  $handler->display->display_options['filters']['search_social']['group_info']['widget'] = 'radios';
  $handler->display->display_options['filters']['search_social']['group_info']['multiple'] = TRUE;
  $handler->display->display_options['filters']['search_social']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Facebook',
      'operator' => 'in',
      'value' => array(
        'facebook' => 'facebook',
        'all' => 0,
        'twitter' => 0,
      ),
    ),
    2 => array(
      'title' => 'Twitter',
      'operator' => 'in',
      'value' => array(
        'twitter' => 'twitter',
        'all' => 0,
        'facebook' => 0,
      ),
    ),
  );


  $views[$view->name] = $view;


  // Return all the views.
  return $views;
}

