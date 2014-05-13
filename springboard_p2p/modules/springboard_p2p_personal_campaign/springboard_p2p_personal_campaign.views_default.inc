<?php
/**
 * @file
 * springboard_p2p_personal_campaign.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_p2p_personal_campaign_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'p2p_personal_campaigns_by_campaign';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Peer to peer Personal campaigns by campaign';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Personal campaigns by campaign';
  $handler->display->display_options['hide_admin_links'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  $handler->display->display_options['row_options']['links'] = FALSE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Campaign (field_p2p_campaign) */
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['id'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['table'] = 'field_data_field_p2p_campaign';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['field'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['validate']['type'] = 'node';
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['validate_options']['types'] = array(
    'p2p_campaign' => 'p2p_campaign',
  );
  $handler->display->display_options['arguments']['field_p2p_campaign_target_id']['validate_options']['access'] = TRUE;
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

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'springboard_p2p/campaigns/campaign';
  $export['p2p_personal_campaigns_by_campaign'] = $view;

  $view = new view();
  $view->name = 'p2p_personal_campaigns_by_campaigner';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Peer to peer Personal campaigns by campaigner';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Personal campaigns by campaigner';
  $handler->display->display_options['hide_admin_links'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'No personal campaigns found.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  /* Field: Content: Submission Goal */
  $handler->display->display_options['fields']['field_p2p_personal_submit_goal']['id'] = 'field_p2p_personal_submit_goal';
  $handler->display->display_options['fields']['field_p2p_personal_submit_goal']['table'] = 'field_data_field_p2p_personal_submit_goal';
  $handler->display->display_options['fields']['field_p2p_personal_submit_goal']['field'] = 'field_p2p_personal_submit_goal';
  /* Field: Content: Fundraising Goal */
  $handler->display->display_options['fields']['field_p2p_personal_campaign_goal']['id'] = 'field_p2p_personal_campaign_goal';
  $handler->display->display_options['fields']['field_p2p_personal_campaign_goal']['table'] = 'field_data_field_p2p_personal_campaign_goal';
  $handler->display->display_options['fields']['field_p2p_personal_campaign_goal']['field'] = 'field_p2p_personal_campaign_goal';
  $handler->display->display_options['fields']['field_p2p_personal_campaign_goal']['settings'] = array(
    'thousand_separator' => ' ',
    'decimal_separator' => '.',
    'scale' => '2',
    'prefix_suffix' => 1,
  );
  /* Field: Content: Progress */
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['id'] = 'field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['table'] = 'field_data_field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['field'] = 'field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['click_sort_column'] = 'amount';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['type'] = 'springboard_p2p_fields_personal_campaign_progress_default';
  /* Field: Content: Fundraising Deadline */
  $handler->display->display_options['fields']['field_p2p_fundraising_deadline']['id'] = 'field_p2p_fundraising_deadline';
  $handler->display->display_options['fields']['field_p2p_fundraising_deadline']['table'] = 'field_data_field_p2p_fundraising_deadline';
  $handler->display->display_options['fields']['field_p2p_fundraising_deadline']['field'] = 'field_p2p_fundraising_deadline';
  $handler->display->display_options['fields']['field_p2p_fundraising_deadline']['label'] = 'Deadline';
  $handler->display->display_options['fields']['field_p2p_fundraising_deadline']['settings'] = array(
    'format_type' => 'long',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  /* Field: Content: Edit link */
  $handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
  $handler->display->display_options['fields']['edit_node']['label'] = '';
  $handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
  /* Field: Content: Delete link */
  $handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
  $handler->display->display_options['fields']['delete_node']['label'] = '';
  $handler->display->display_options['fields']['delete_node']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['delete_node']['alter']['absolute'] = TRUE;
  $handler->display->display_options['fields']['delete_node']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Author uid */
  $handler->display->display_options['arguments']['uid']['id'] = 'uid';
  $handler->display->display_options['arguments']['uid']['table'] = 'node';
  $handler->display->display_options['arguments']['uid']['field'] = 'uid';
  $handler->display->display_options['arguments']['uid']['default_action'] = 'empty';
  $handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['uid']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['uid']['validate']['type'] = 'user';
  $handler->display->display_options['arguments']['uid']['validate_options']['restrict_roles'] = TRUE;
  $handler->display->display_options['arguments']['uid']['validate_options']['roles'] = array(
    3 => '3',
    6 => '6',
  );
  $handler->display->display_options['arguments']['uid']['validate']['fail'] = 'empty';
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

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'springboard_p2p/campaigns/user';
  $export['p2p_personal_campaigns_by_campaigner'] = $view;

  $view = new view();
  $view->name = 'p2p_personal_campaigns_by_category';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = ' Peer to peer Personal campaigns by category';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Personal campaigns by category';
  $handler->display->display_options['hide_admin_links'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  $handler->display->display_options['row_options']['links'] = FALSE;
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['id'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['table'] = 'field_data_field_p2p_campaign';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['field'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['required'] = TRUE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Category (field_p2p_category) */
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['id'] = 'field_p2p_category_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['table'] = 'field_data_field_p2p_category';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['field'] = 'field_p2p_category_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['relationship'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate']['type'] = 'node';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate_options']['types'] = array(
    'p2p_category' => 'p2p_category',
  );
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate_options']['access'] = TRUE;
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

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'springboard_p2p/campaigns/category';
  $export['p2p_personal_campaigns_by_category'] = $view;

  $view = new view();
  $view->name = 'p2p_top_personal_campaigns_by_category';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Peer to peer Top Personal campaigns by category';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Personal campaigns by category';
  $handler->display->display_options['hide_admin_links'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'field_p2p_campaign_progress' => 'field_p2p_campaign_progress',
    'progress' => 'progress',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_p2p_campaign_progress' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'progress' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Entity Reference: Referenced Entity */
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['id'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['table'] = 'field_data_field_p2p_campaign';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['field'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['relationships']['field_p2p_campaign_target_id']['required'] = TRUE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  /* Field: Content: Progress */
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['id'] = 'field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['table'] = 'field_data_field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['field'] = 'field_p2p_campaign_progress';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['click_sort_column'] = 'amount';
  $handler->display->display_options['fields']['field_p2p_campaign_progress']['type'] = 'springboard_p2p_fields_personal_campaign_progress_default';
  /* Sort criterion: Content: Progress (field_p2p_campaign_progress:amount) */
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['id'] = 'field_p2p_campaign_progress_amount';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['table'] = 'field_data_field_p2p_campaign_progress';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['field'] = 'field_p2p_campaign_progress_amount';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['order'] = 'DESC';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['exposed'] = TRUE;
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_amount']['expose']['label'] = 'Progress (field_p2p_campaign_progress:amount)';
  /* Sort criterion: Content: Progress (field_p2p_campaign_progress:submissions) */
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['id'] = 'field_p2p_campaign_progress_submissions';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['table'] = 'field_data_field_p2p_campaign_progress';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['field'] = 'field_p2p_campaign_progress_submissions';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['order'] = 'DESC';
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['exposed'] = TRUE;
  $handler->display->display_options['sorts']['field_p2p_campaign_progress_submissions']['expose']['label'] = 'Progress (field_p2p_campaign_progress:submissions)';
  /* Contextual filter: Content: Category (field_p2p_category) */
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['id'] = 'field_p2p_category_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['table'] = 'field_data_field_p2p_category';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['field'] = 'field_p2p_category_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['relationship'] = 'field_p2p_campaign_target_id';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate']['type'] = 'node';
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate_options']['types'] = array(
    'p2p_category' => 'p2p_category',
  );
  $handler->display->display_options['arguments']['field_p2p_category_target_id']['validate_options']['access'] = TRUE;
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

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'springboard_p2p/campaigns/top';
  $export['p2p_top_personal_campaigns_by_category'] = $view;

  return $export;
}
