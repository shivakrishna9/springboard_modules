<?php

/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_advocacy_views_default_views() {

$view = new view();
$view->name = 'targets';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'advocacy';
$view->human_name = 'Targets Admin Page';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Targets';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  4 => '4',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
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
/* Field: Advocacy: Target ID */
$handler->display->display_options['fields']['id']['id'] = 'id';
$handler->display->display_options['fields']['id']['table'] = 'advocacy';
$handler->display->display_options['fields']['id']['field'] = 'id';
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
$handler->display->display_options['filters']['search_types']['id'] = 'search_types';
$handler->display->display_options['filters']['search_types']['table'] = 'advocacy';
$handler->display->display_options['filters']['search_types']['field'] = 'search_types';
$handler->display->display_options['filters']['search_types']['value'] = array(
  'custom' => 'custom',
);
$handler->display->display_options['filters']['search_types']['expose']['operator_id'] = 'search_types_op';
$handler->display->display_options['filters']['search_types']['expose']['label'] = 'Advocacy Search Types';
$handler->display->display_options['filters']['search_types']['expose']['operator'] = 'search_types_op';
$handler->display->display_options['filters']['search_types']['expose']['identifier'] = 'search_types';
$handler->display->display_options['filters']['search_types']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['search_types']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  5 => 0,
);
$handler->display->display_options['filters']['search_types']['expose']['reduce'] = TRUE;
/* Filter criterion: Advocacy: Search Multiple Target Fields by Keywords */
$handler->display->display_options['filters']['combine']['id'] = 'combine';
$handler->display->display_options['filters']['combine']['table'] = 'advocacy';
$handler->display->display_options['filters']['combine']['field'] = 'combine';
$handler->display->display_options['filters']['combine']['operator'] = 'contains';
$handler->display->display_options['filters']['combine']['exposed'] = TRUE;
$handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['label'] = 'Search Multiple Target Fields by Keywords';
$handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
$handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  5 => 0,
);
$handler->display->display_options['filters']['combine']['fields'] = array(
  'first_name' => 'first_name',
  'last_name' => 'last_name',
  'email' => 'email',
  'party' => 'party',
  'role' => 'role',
  'state' => 'state',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/config/springboard/advocacy/custom-targets';

  $views[$view->name] = $view;

  // Return all the views.
  return $views;
}

