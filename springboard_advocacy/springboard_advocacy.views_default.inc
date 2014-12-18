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
  $view->description = 'Advocacy Custom Target List';
  $view->tag = 'Advocacy';
  $view->base_table = 'advocacy';
  $view->human_name = 'Targets Admin Page';
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
  /* Field: Advocacy: Target ID */
  $handler->display->display_options['fields']['id']['id'] = 'id';
  $handler->display->display_options['fields']['id']['table'] = 'advocacy';
  $handler->display->display_options['fields']['id']['field'] = 'id';
  $handler->display->display_options['fields']['id']['label'] = 'Actions';
  $handler->display->display_options['fields']['id']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['id']['alter']['text'] = '<a href = \'/admin/springboard/advocacy/custom-targets/[id]/edit\'>Edit</a> | ';
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
  $handler->display->display_options['filters']['last_name']['operator'] = '=';
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
  /* Filter criterion: Advocacy: Search By Level */
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
  $handler->display->display_options['filters']['search_role']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
  );
  $handler->display->display_options['filters']['search_role']['expose']['reduce'] = TRUE;

  /* Display: Search Custom Targets */
  $handler = $view->new_display('page', 'Search Custom Targets', 'page');
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
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
  $handler->display->display_options['filters']['last_name']['operator'] = '=';
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
  $handler->display->display_options['path'] = 'admin/config/springboard/advocacy/custom-targets';

  /* Display: Search Legislators */
  $handler = $view->new_display('page', 'Search Legislators', 'page_1');
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Advocacy: Search By Type */
  $handler->display->display_options['filters']['search_class_name']['id'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['table'] = 'advocacy';
  $handler->display->display_options['filters']['search_class_name']['field'] = 'search_class_name';
  $handler->display->display_options['filters']['search_class_name']['value'] = array(
    'Legislator' => 'Legislator',
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
  $handler->display->display_options['filters']['last_name']['operator'] = '=';
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
  /* Filter criterion: Advocacy: Search By Level */
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
  $handler->display->display_options['path'] = 'admin/config/springboard/advocacy/legislators';

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
  $handler->display->display_options['filters']['last_name']['operator'] = '=';
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
  /* Filter criterion: Advocacy: Search By Level */
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

  $views[$view->name] = $view;

  // Return all the views.
  return $views;
}

