<?php
/**
 * @file
 * Provides default view configurations.
 */

/**
 * Implements hook_views_default_views().
 */
function springboard_dw_views_default_views() {
  $views = array();
  
  // springboard_dw_donations_report view:
  $view = new view();
  $view->name = 'springboard_dw_donations_report';
  $view->description = 'Fetches donation reports from the external data warehouse.';
  $view->tag = 'default';
  $view->base_table = 'sp_dw_donations_psudo';
  $view->human_name = 'Springboard DW Donations Report';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Archived Donations';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer fundraiser';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Go';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '3';
  $handler->display->display_options['pager']['options']['expose']['items_per_page'] = TRUE;
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options'] = '20, 50, 100';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'transaction_id' => 'transaction_id',
    'transaction_date' => 'transaction_date',
    'amount' => 'amount',
    'status' => 'status',
    'payment_method' => 'payment_method',
    'first_name' => 'first_name',
    'last_name' => 'last_name',
    'email' => 'email',
    'recurs_monthly' => 'recurs_monthly',
    'form_name' => 'form_name',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'transaction_id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'transaction_date' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'amount' => array(
      'sortable' => 1,
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
    'payment_method' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'first_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'last_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'email' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'recurs_monthly' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'form_name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = 'Reminder! Donation data syncs to the warehouse within 15 minutes of submission.';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'No donations were returned for your search.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Springboard DW Donations: Donation ID */
  $handler->display->display_options['fields']['donation_id']['id'] = 'donation_id';
  $handler->display->display_options['fields']['donation_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['donation_id']['field'] = 'donation_id';
  $handler->display->display_options['fields']['donation_id']['label'] = 'Order ID';
  $handler->display->display_options['fields']['donation_id']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['donation_id']['alter']['path'] = '/admin/reports/springboard-dw/donations/[donation_id]';
  /* Field: Springboard DW Donations: Transaction Date */
  $handler->display->display_options['fields']['transaction_date']['id'] = 'transaction_date';
  $handler->display->display_options['fields']['transaction_date']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['transaction_date']['field'] = 'transaction_date';
  $handler->display->display_options['fields']['transaction_date']['label'] = 'Date';
  $handler->display->display_options['fields']['transaction_date']['date_format'] = 'short';
  $handler->display->display_options['fields']['transaction_date']['second_date_format'] = 'long';
  /* Field: Springboard DW Donations: Donation Amount */
  $handler->display->display_options['fields']['amount']['id'] = 'amount';
  $handler->display->display_options['fields']['amount']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['amount']['field'] = 'amount';
  $handler->display->display_options['fields']['amount']['label'] = 'Amount';
  /* Field: Springboard DW Donations: Transaction Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Order Status';
  /* Field: Springboard DW Donations: Gateway */
  $handler->display->display_options['fields']['gateway']['id'] = 'gateway';
  $handler->display->display_options['fields']['gateway']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['gateway']['field'] = 'gateway';
  $handler->display->display_options['fields']['gateway']['label'] = 'Payment Method';
  /* Field: Springboard DW Donations: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Springboard DW Donations: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Springboard DW Donations: Email Address */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  $handler->display->display_options['fields']['email']['label'] = 'Email';
  $handler->display->display_options['fields']['email']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['email']['alter']['path'] = 'mailto:[email]';
  /* Field: Springboard DW Donations: Form Name */
  $handler->display->display_options['fields']['form_name']['id'] = 'form_name';
  $handler->display->display_options['fields']['form_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['form_name']['field'] = 'form_name';
  $handler->display->display_options['fields']['form_name']['label'] = 'Form';
  /* Field: Springboard DW Donations: Recurs Monthly */
  $handler->display->display_options['fields']['recurs_monthly']['id'] = 'recurs_monthly';
  $handler->display->display_options['fields']['recurs_monthly']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['recurs_monthly']['field'] = 'recurs_monthly';
  /* Field: Springboard DW Donations: Payment */
  $handler->display->display_options['fields']['payment_link']['id'] = 'payment_link';
  $handler->display->display_options['fields']['payment_link']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['payment_link']['field'] = 'payment_link';
  /* Sort criterion: Springboard DW Donations: Transaction ID */
  $handler->display->display_options['sorts']['transaction_id']['id'] = 'transaction_id';
  $handler->display->display_options['sorts']['transaction_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['sorts']['transaction_id']['field'] = 'transaction_id';
  /* Filter criterion: Springboard DW Donations: Donation ID */
  $handler->display->display_options['filters']['donation_id']['id'] = 'donation_id';
  $handler->display->display_options['filters']['donation_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['donation_id']['field'] = 'donation_id';
  $handler->display->display_options['filters']['donation_id']['group'] = 1;
  $handler->display->display_options['filters']['donation_id']['exposed'] = TRUE;
  $handler->display->display_options['filters']['donation_id']['expose']['operator_id'] = 'donation_id_op';
  $handler->display->display_options['filters']['donation_id']['expose']['label'] = 'Donation ID';
  $handler->display->display_options['filters']['donation_id']['expose']['operator'] = 'donation_id_op';
  $handler->display->display_options['filters']['donation_id']['expose']['identifier'] = 'donation_id';
  $handler->display->display_options['filters']['donation_id']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: Email Address */
  $handler->display->display_options['filters']['email']['id'] = 'email';
  $handler->display->display_options['filters']['email']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['email']['field'] = 'email';
  $handler->display->display_options['filters']['email']['operator'] = 'contains';
  $handler->display->display_options['filters']['email']['group'] = 1;
  $handler->display->display_options['filters']['email']['exposed'] = TRUE;
  $handler->display->display_options['filters']['email']['expose']['operator_id'] = 'email_op';
  $handler->display->display_options['filters']['email']['expose']['label'] = 'Email Address';
  $handler->display->display_options['filters']['email']['expose']['operator'] = 'email_op';
  $handler->display->display_options['filters']['email']['expose']['identifier'] = 'email';
  $handler->display->display_options['filters']['email']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: Transaction Date */
  $handler->display->display_options['filters']['transaction_date']['id'] = 'transaction_date';
  $handler->display->display_options['filters']['transaction_date']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['transaction_date']['field'] = 'transaction_date';
  $handler->display->display_options['filters']['transaction_date']['operator'] = 'between';
  $handler->display->display_options['filters']['transaction_date']['group'] = 1;
  $handler->display->display_options['filters']['transaction_date']['exposed'] = TRUE;
  $handler->display->display_options['filters']['transaction_date']['expose']['operator_id'] = 'transaction_date_op';
  $handler->display->display_options['filters']['transaction_date']['expose']['label'] = 'Donation date between:';
  $handler->display->display_options['filters']['transaction_date']['expose']['operator'] = 'transaction_date_op';
  $handler->display->display_options['filters']['transaction_date']['expose']['identifier'] = 'transaction_date';
  $handler->display->display_options['filters']['transaction_date']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  $handler->display->display_options['filters']['transaction_date']['form_type'] = 'date_popup';
  $handler->display->display_options['filters']['transaction_date']['date_fields'] = array(
    'transaction_date' => 'transaction_date',
  );
  /* Filter criterion: Springboard DW Donations: Gateway */
  $handler->display->display_options['filters']['gateway']['id'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['gateway']['field'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['operator'] = 'contains';
  $handler->display->display_options['filters']['gateway']['group'] = 1;
  $handler->display->display_options['filters']['gateway']['exposed'] = TRUE;
  $handler->display->display_options['filters']['gateway']['expose']['operator_id'] = 'gateway_op';
  $handler->display->display_options['filters']['gateway']['expose']['label'] = 'Gateway';
  $handler->display->display_options['filters']['gateway']['expose']['operator'] = 'gateway_op';
  $handler->display->display_options['filters']['gateway']['expose']['identifier'] = 'gateway';
  $handler->display->display_options['filters']['gateway']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: Donation Amount */
  $handler->display->display_options['filters']['amount']['id'] = 'amount';
  $handler->display->display_options['filters']['amount']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['amount']['field'] = 'amount';
  $handler->display->display_options['filters']['amount']['group'] = 1;
  $handler->display->display_options['filters']['amount']['exposed'] = TRUE;
  $handler->display->display_options['filters']['amount']['expose']['operator_id'] = 'amount_op';
  $handler->display->display_options['filters']['amount']['expose']['label'] = 'Donation Amount';
  $handler->display->display_options['filters']['amount']['expose']['operator'] = 'amount_op';
  $handler->display->display_options['filters']['amount']['expose']['identifier'] = 'amount';
  $handler->display->display_options['filters']['amount']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: First Name */
  $handler->display->display_options['filters']['first_name']['id'] = 'first_name';
  $handler->display->display_options['filters']['first_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['first_name']['field'] = 'first_name';
  $handler->display->display_options['filters']['first_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['first_name']['group'] = 1;
  $handler->display->display_options['filters']['first_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['first_name']['expose']['operator_id'] = 'first_name_op';
  $handler->display->display_options['filters']['first_name']['expose']['label'] = 'First Name';
  $handler->display->display_options['filters']['first_name']['expose']['operator'] = 'first_name_op';
  $handler->display->display_options['filters']['first_name']['expose']['identifier'] = 'first_name';
  $handler->display->display_options['filters']['first_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: Last Name */
  $handler->display->display_options['filters']['last_name']['id'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['last_name']['field'] = 'last_name';
  $handler->display->display_options['filters']['last_name']['operator'] = 'contains';
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
    6 => 0,
  );
  /* Filter criterion: Springboard DW Donations: Form Name */
  $handler->display->display_options['filters']['form_name']['id'] = 'form_name';
  $handler->display->display_options['filters']['form_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['filters']['form_name']['field'] = 'form_name';
  $handler->display->display_options['filters']['form_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['form_name']['group'] = 1;
  $handler->display->display_options['filters']['form_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['form_name']['expose']['operator_id'] = 'form_name_op';
  $handler->display->display_options['filters']['form_name']['expose']['label'] = 'Form Name';
  $handler->display->display_options['filters']['form_name']['expose']['operator'] = 'form_name_op';
  $handler->display->display_options['filters']['form_name']['expose']['identifier'] = 'form_name';
  $handler->display->display_options['filters']['form_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );

  /* Display: Donations List */
  $handler = $view->new_display('page', 'Donations List', 'page');
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: Springboard DW Donations: Transaction ID */
  $handler->display->display_options['sorts']['transaction_id']['id'] = 'transaction_id';
  $handler->display->display_options['sorts']['transaction_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['sorts']['transaction_id']['field'] = 'transaction_id';
  $handler->display->display_options['sorts']['transaction_id']['order'] = 'DESC';
  $handler->display->display_options['sorts']['transaction_id']['expose']['label'] = 'Transaction ID';
  $handler->display->display_options['path'] = 'admin/reports/springboard-dw/donations';

  /* Display: Donation Details */
  $handler = $view->new_display('page', 'Donation Details', 'page_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Donation Details';
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '1';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['header'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Springboard DW Donations: Transaction ID */
  $handler->display->display_options['fields']['transaction_id']['id'] = 'transaction_id';
  $handler->display->display_options['fields']['transaction_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['transaction_id']['field'] = 'transaction_id';
  $handler->display->display_options['fields']['transaction_id']['label'] = 'Order ID';
  /* Field: Springboard DW Donations: Transaction Date */
  $handler->display->display_options['fields']['transaction_date']['id'] = 'transaction_date';
  $handler->display->display_options['fields']['transaction_date']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['transaction_date']['field'] = 'transaction_date';
  $handler->display->display_options['fields']['transaction_date']['label'] = 'Date';
  /* Field: Springboard DW Donations: Donation Amount */
  $handler->display->display_options['fields']['amount']['id'] = 'amount';
  $handler->display->display_options['fields']['amount']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['amount']['field'] = 'amount';
  $handler->display->display_options['fields']['amount']['label'] = 'Amount';
  /* Field: Springboard DW Donations: Transaction Status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Order Status';
  /* Field: Springboard DW Donations: Payment Method */
  $handler->display->display_options['fields']['payment_method']['id'] = 'payment_method';
  $handler->display->display_options['fields']['payment_method']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['payment_method']['field'] = 'payment_method';
  /* Field: Springboard DW Donations: First Name */
  $handler->display->display_options['fields']['first_name']['id'] = 'first_name';
  $handler->display->display_options['fields']['first_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['first_name']['field'] = 'first_name';
  /* Field: Springboard DW Donations: Last Name */
  $handler->display->display_options['fields']['last_name']['id'] = 'last_name';
  $handler->display->display_options['fields']['last_name']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['last_name']['field'] = 'last_name';
  /* Field: Springboard DW Donations: Email Address */
  $handler->display->display_options['fields']['email']['id'] = 'email';
  $handler->display->display_options['fields']['email']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['email']['field'] = 'email';
  $handler->display->display_options['fields']['email']['label'] = 'Email';
  $handler->display->display_options['fields']['email']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['email']['alter']['path'] = 'mailto:[email]';
  /* Field: Springboard DW Donations: Recurs Monthly */
  $handler->display->display_options['fields']['recurs_monthly']['id'] = 'recurs_monthly';
  $handler->display->display_options['fields']['recurs_monthly']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['recurs_monthly']['field'] = 'recurs_monthly';
  /* Field: Springboard DW Donations: City */
  $handler->display->display_options['fields']['city']['id'] = 'city';
  $handler->display->display_options['fields']['city']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['city']['field'] = 'city';
  /* Field: Springboard DW Donations: Country */
  $handler->display->display_options['fields']['country']['id'] = 'country';
  $handler->display->display_options['fields']['country']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['country']['field'] = 'country';
  /* Field: Springboard DW Donations: Currency */
  $handler->display->display_options['fields']['currency']['id'] = 'currency';
  $handler->display->display_options['fields']['currency']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['currency']['field'] = 'currency';
  /* Field: Springboard DW Donations: Donation Amount */
  $handler->display->display_options['fields']['amount_1']['id'] = 'amount_1';
  $handler->display->display_options['fields']['amount_1']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['amount_1']['field'] = 'amount';
  /* Field: Springboard DW Donations: Donation Form ID */
  $handler->display->display_options['fields']['form_id']['id'] = 'form_id';
  $handler->display->display_options['fields']['form_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['form_id']['field'] = 'form_id';
  /* Field: Springboard DW Donations: Donation ID */
  $handler->display->display_options['fields']['donation_id']['id'] = 'donation_id';
  $handler->display->display_options['fields']['donation_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['donation_id']['field'] = 'donation_id';
  /* Field: Springboard DW Donations: Email Address */
  $handler->display->display_options['fields']['email_1']['id'] = 'email_1';
  $handler->display->display_options['fields']['email_1']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['email_1']['field'] = 'email';
  /* Field: Springboard DW Donations: Extra Props Data */
  $handler->display->display_options['fields']['extra_props']['id'] = 'extra_props';
  $handler->display->display_options['fields']['extra_props']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['extra_props']['field'] = 'extra_props';
  /* Field: Springboard DW Donations: Gateway */
  $handler->display->display_options['fields']['gateway']['id'] = 'gateway';
  $handler->display->display_options['fields']['gateway']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['gateway']['field'] = 'gateway';
  /* Field: Springboard DW Donations: Mailing Address */
  $handler->display->display_options['fields']['address']['id'] = 'address';
  $handler->display->display_options['fields']['address']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['address']['field'] = 'address';
  /* Field: Springboard DW Donations: Mailing Address - Line 2 */
  $handler->display->display_options['fields']['address_line_2']['id'] = 'address_line_2';
  $handler->display->display_options['fields']['address_line_2']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['address_line_2']['field'] = 'address_line_2';
  /* Field: Springboard DW Donations: Payment Method */
  $handler->display->display_options['fields']['payment_method_1']['id'] = 'payment_method_1';
  $handler->display->display_options['fields']['payment_method_1']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['payment_method_1']['field'] = 'payment_method';
  /* Field: Springboard DW Donations: State */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  /* Field: Springboard DW Donations: Submission ID */
  $handler->display->display_options['fields']['submission_id']['id'] = 'submission_id';
  $handler->display->display_options['fields']['submission_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['submission_id']['field'] = 'submission_id';
  /* Field: Springboard DW Donations: Zip/Postal Code */
  $handler->display->display_options['fields']['zip']['id'] = 'zip';
  $handler->display->display_options['fields']['zip']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['zip']['field'] = 'zip';
  /* Field: Springboard DW Donations: Transaction Date */
  $handler->display->display_options['fields']['transaction_date_1']['id'] = 'transaction_date_1';
  $handler->display->display_options['fields']['transaction_date_1']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['fields']['transaction_date_1']['field'] = 'transaction_date';
  $handler->display->display_options['fields']['transaction_date_1']['date_format'] = 'short';
  $handler->display->display_options['fields']['transaction_date_1']['second_date_format'] = 'long';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Springboard DW Donations: Donation ID */
  $handler->display->display_options['arguments']['donation_id']['id'] = 'donation_id';
  $handler->display->display_options['arguments']['donation_id']['table'] = 'sp_dw_donations_psudo';
  $handler->display->display_options['arguments']['donation_id']['field'] = 'donation_id';
  $handler->display->display_options['arguments']['donation_id']['default_action'] = 'default';
  $handler->display->display_options['arguments']['donation_id']['default_argument_type'] = 'node';
  $handler->display->display_options['arguments']['donation_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['donation_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['donation_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  $handler->display->display_options['path'] = 'admin/reports/springboard-dw/donations/%';

  $views[$view->name] = $view;
  // End springboard_dw_donations_report view.

  return $views;
}
