<?php
/**
 * @file
 * Provides default View of log data.
 */

/**
 * Implements hook_views_default_views().
 */
function silverpop_transact_views_default_views() {
  $view = new view();
  $view->name = 'salesforce_ngoc_batch_upload_items';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'salesforce_ngoc_sync_map';
  $view->human_name = 'Salesforce NGOC batch upload items';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Salesforce NGOC batch upload items';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'created' => 'created',
    'sf_status' => 'sf_status',
    'batch_upload_sfid' => 'batch_upload_sfid',
    'account_sfid' => 'account_sfid',
    'contact_sfid' => 'account_sfid',
    'opportunity_sfid' => 'account_sfid',
    'did' => 'did',
    'uid' => 'uid',
    'module' => 'module',
    'delta' => 'delta',
    'lasted_polled' => 'lasted_polled',
    'polling_attempts' => 'polling_attempts',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'sf_status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'batch_upload_sfid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'account_sfid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'contact_sfid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'opportunity_sfid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'did' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'uid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'module' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'delta' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'lasted_polled' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'polling_attempts' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: NGOC Batch Upload Sync Map: Date created */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'created';
  $handler->display->display_options['fields']['created']['exclude'] = TRUE;
  $handler->display->display_options['fields']['created']['date_format'] = 'short';
  $handler->display->display_options['fields']['created']['second_date_format'] = 'long';
  /* Field: NGOC Batch Upload Sync Map: Sf_status */
  $handler->display->display_options['fields']['sf_status']['id'] = 'sf_status';
  $handler->display->display_options['fields']['sf_status']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['sf_status']['field'] = 'sf_status';
  $handler->display->display_options['fields']['sf_status']['label'] = 'status';
  /* Field: NGOC Batch Upload Sync Map: Batch_upload_sfid */
  $handler->display->display_options['fields']['batch_upload_sfid']['id'] = 'batch_upload_sfid';
  $handler->display->display_options['fields']['batch_upload_sfid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['batch_upload_sfid']['field'] = 'batch_upload_sfid';
  /* Field: NGOC Batch Upload Sync Map: Account_sfid */
  $handler->display->display_options['fields']['account_sfid']['id'] = 'account_sfid';
  $handler->display->display_options['fields']['account_sfid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['account_sfid']['field'] = 'account_sfid';
  $handler->display->display_options['fields']['account_sfid']['label'] = 'SF Objects';
  $handler->display->display_options['fields']['account_sfid']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['account_sfid']['alter']['text'] = 'Account: [account_sfid]';
  $handler->display->display_options['fields']['account_sfid']['alter']['nl2br'] = TRUE;
  $handler->display->display_options['fields']['account_sfid']['element_type'] = 'div';
  $handler->display->display_options['fields']['account_sfid']['element_label_colon'] = FALSE;
  /* Field: NGOC Batch Upload Sync Map: Contact_sfid */
  $handler->display->display_options['fields']['contact_sfid']['id'] = 'contact_sfid';
  $handler->display->display_options['fields']['contact_sfid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['contact_sfid']['field'] = 'contact_sfid';
  $handler->display->display_options['fields']['contact_sfid']['label'] = 'Contact';
  $handler->display->display_options['fields']['contact_sfid']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['contact_sfid']['alter']['text'] = 'Contact: [contact_sfid]';
  $handler->display->display_options['fields']['contact_sfid']['element_type'] = 'div';
  /* Field: NGOC Batch Upload Sync Map: Opportunity_sfid */
  $handler->display->display_options['fields']['opportunity_sfid']['id'] = 'opportunity_sfid';
  $handler->display->display_options['fields']['opportunity_sfid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['opportunity_sfid']['field'] = 'opportunity_sfid';
  $handler->display->display_options['fields']['opportunity_sfid']['label'] = 'Opportunity';
  $handler->display->display_options['fields']['opportunity_sfid']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['opportunity_sfid']['alter']['text'] = 'Opportunity: [opportunity_sfid]';
  $handler->display->display_options['fields']['opportunity_sfid']['element_type'] = 'div';
  /* Field: NGOC Batch Upload Sync Map: Did */
  $handler->display->display_options['fields']['did']['id'] = 'did';
  $handler->display->display_options['fields']['did']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['did']['field'] = 'did';
  $handler->display->display_options['fields']['did']['separator'] = '';
  /* Field: NGOC Batch Upload Sync Map: Uid */
  $handler->display->display_options['fields']['uid']['id'] = 'uid';
  $handler->display->display_options['fields']['uid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['uid']['field'] = 'uid';
  $handler->display->display_options['fields']['uid']['separator'] = '';
  /* Field: NGOC Batch Upload Sync Map: Module */
  $handler->display->display_options['fields']['module']['id'] = 'module';
  $handler->display->display_options['fields']['module']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['module']['field'] = 'module';
  /* Field: NGOC Batch Upload Sync Map: Lasted_polled */
  $handler->display->display_options['fields']['lasted_polled']['id'] = 'lasted_polled';
  $handler->display->display_options['fields']['lasted_polled']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['lasted_polled']['field'] = 'lasted_polled';
  $handler->display->display_options['fields']['lasted_polled']['date_format'] = 'short';
  $handler->display->display_options['fields']['lasted_polled']['second_date_format'] = 'long';
  /* Field: NGOC Batch Upload Sync Map: Polling_attempts */
  $handler->display->display_options['fields']['polling_attempts']['id'] = 'polling_attempts';
  $handler->display->display_options['fields']['polling_attempts']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['fields']['polling_attempts']['field'] = 'polling_attempts';
  $handler->display->display_options['fields']['polling_attempts']['label'] = 'attempts';
  /* Sort criterion: NGOC Batch Upload Sync Map: Date created */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: NGOC Batch Upload Sync Map: Sf_status */
  $handler->display->display_options['filters']['sf_status']['id'] = 'sf_status';
  $handler->display->display_options['filters']['sf_status']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['filters']['sf_status']['field'] = 'sf_status';
  $handler->display->display_options['filters']['sf_status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['sf_status']['expose']['operator_id'] = 'sf_status_op';
  $handler->display->display_options['filters']['sf_status']['expose']['label'] = 'Status';
  $handler->display->display_options['filters']['sf_status']['expose']['operator'] = 'sf_status_op';
  $handler->display->display_options['filters']['sf_status']['expose']['identifier'] = 'sf_status';
  $handler->display->display_options['filters']['sf_status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );
  /* Filter criterion: NGOC Batch Upload Sync Map: Batch_upload_sfid */
  $handler->display->display_options['filters']['batch_upload_sfid']['id'] = 'batch_upload_sfid';
  $handler->display->display_options['filters']['batch_upload_sfid']['table'] = 'salesforce_ngoc_sync_map';
  $handler->display->display_options['filters']['batch_upload_sfid']['field'] = 'batch_upload_sfid';
  $handler->display->display_options['filters']['batch_upload_sfid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['batch_upload_sfid']['expose']['operator_id'] = 'batch_upload_sfid_op';
  $handler->display->display_options['filters']['batch_upload_sfid']['expose']['label'] = 'Batch upload sfid';
  $handler->display->display_options['filters']['batch_upload_sfid']['expose']['operator'] = 'batch_upload_sfid_op';
  $handler->display->display_options['filters']['batch_upload_sfid']['expose']['identifier'] = 'batch_upload_sfid';
  $handler->display->display_options['filters']['batch_upload_sfid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/springboard/reports/integration-reports/batch-upload-item';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Batch Upload Object Polling Status';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $handler->display->display_options['tab_options']['weight'] = '0';

  $views[$view->name] = $view;
  return $views;
}
