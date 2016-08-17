<?php

/**
 * @file
 * Views-related hooks for Springboard Quick Donate.
 */

/**
 * Implements hook_views_default_views_alter().
 *
 * Override the commerce_cardonfile module's cards view with some of our own
 * custom options. Exported from the adjusted view.
 */
function fundraiser_quick_donate_views_default_views_alter(&$views) {
  /* Display: Master */
  $handler = &$views['commerce_card_on_file_user_cards']->display['default']->handler;
  $handler->display->display_options['title'] = 'Stored cards';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view own card data';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'no results';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';

  // Empty out any pre-existing relationships.
  $handler->display->display_options['relationships'] = array();
  /* Relationship: Commerce Card on File: Owner */
  $handler->display->display_options['relationships']['uid']['id'] = 'uid';
  $handler->display->display_options['relationships']['uid']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['relationships']['uid']['field'] = 'uid';
  $handler->display->display_options['relationships']['uid']['required'] = TRUE;

  // Empty out any pre-existing fields.
  $handler->display->display_options['fields'] = array();
  /* Field: Commerce Card on File: Card type */
  $handler->display->display_options['fields']['card_type']['id'] = 'card_type';
  $handler->display->display_options['fields']['card_type']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['card_type']['field'] = 'card_type';
  $handler->display->display_options['fields']['card_type']['label'] = '';
  $handler->display->display_options['fields']['card_type']['exclude'] = TRUE;
  $handler->display->display_options['fields']['card_type']['element_label_colon'] = FALSE;
  /* Field: Commerce Card on File: Card number */
  $handler->display->display_options['fields']['card_number']['id'] = 'card_number';
  $handler->display->display_options['fields']['card_number']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['card_number']['field'] = 'card_number';
  $handler->display->display_options['fields']['card_number']['label'] = '';
  $handler->display->display_options['fields']['card_number']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['card_number']['alter']['text'] = '[card_type] - [card_number]';
  $handler->display->display_options['fields']['card_number']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['card_number']['element_wrapper_type'] = 'h2';
  $handler->display->display_options['fields']['card_number']['element_default_classes'] = FALSE;
  /* Field: Commerce Card on File: Name on the card */
  $handler->display->display_options['fields']['card_name']['id'] = 'card_name';
  $handler->display->display_options['fields']['card_name']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['card_name']['field'] = 'card_name';
  $handler->display->display_options['fields']['card_name']['label'] = '';
  $handler->display->display_options['fields']['card_name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['card_name']['element_default_classes'] = FALSE;
  /* Field: Commerce Card on File: Default card */
  $handler->display->display_options['fields']['instance_default']['id'] = 'instance_default';
  $handler->display->display_options['fields']['instance_default']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['instance_default']['field'] = 'instance_default';
  $handler->display->display_options['fields']['instance_default']['label'] = 'Default';
  $handler->display->display_options['fields']['instance_default']['exclude'] = TRUE;
  $handler->display->display_options['fields']['instance_default']['not'] = 0;
  /* Field: Commerce Card on File: Card expiration month */
  $handler->display->display_options['fields']['card_exp_month']['id'] = 'card_exp_month';
  $handler->display->display_options['fields']['card_exp_month']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['card_exp_month']['field'] = 'card_exp_month';
  $handler->display->display_options['fields']['card_exp_month']['label'] = '';
  $handler->display->display_options['fields']['card_exp_month']['exclude'] = TRUE;
  $handler->display->display_options['fields']['card_exp_month']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['card_exp_month']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['card_exp_month']['separator'] = '';
  /* Field: Commerce Card on File: Card expiration year */
  $handler->display->display_options['fields']['card_exp_year']['id'] = 'card_exp_year';
  $handler->display->display_options['fields']['card_exp_year']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['card_exp_year']['field'] = 'card_exp_year';
  $handler->display->display_options['fields']['card_exp_year']['label'] = '';
  $handler->display->display_options['fields']['card_exp_year']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['card_exp_year']['alter']['text'] = '[card_exp_month]/[card_exp_year]';
  $handler->display->display_options['fields']['card_exp_year']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['card_exp_year']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['card_exp_year']['separator'] = '';
  /* Field: Commerce Card Data: Billing Profile */
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['id'] = 'commerce_cardonfile_profile';
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['table'] = 'field_data_commerce_cardonfile_profile';
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['field'] = 'commerce_cardonfile_profile';
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['label'] = '';
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['commerce_cardonfile_profile']['element_default_classes'] = FALSE;
  /* Field: User: Address */
  $handler->display->display_options['fields']['sbp_address']['id'] = 'sbp_address';
  $handler->display->display_options['fields']['sbp_address']['table'] = 'field_data_sbp_address';
  $handler->display->display_options['fields']['sbp_address']['field'] = 'sbp_address';
  $handler->display->display_options['fields']['sbp_address']['relationship'] = 'uid';
  $handler->display->display_options['fields']['sbp_address']['label'] = '';
  $handler->display->display_options['fields']['sbp_address']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sbp_address']['element_default_classes'] = FALSE;
  /* Field: User: City */
  $handler->display->display_options['fields']['sbp_city']['id'] = 'sbp_city';
  $handler->display->display_options['fields']['sbp_city']['table'] = 'field_data_sbp_city';
  $handler->display->display_options['fields']['sbp_city']['field'] = 'sbp_city';
  $handler->display->display_options['fields']['sbp_city']['relationship'] = 'uid';
  $handler->display->display_options['fields']['sbp_city']['label'] = '';
  $handler->display->display_options['fields']['sbp_city']['exclude'] = TRUE;
  $handler->display->display_options['fields']['sbp_city']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sbp_city']['element_default_classes'] = FALSE;
  /* Field: User: State/Province */
  $handler->display->display_options['fields']['sbp_state']['id'] = 'sbp_state';
  $handler->display->display_options['fields']['sbp_state']['table'] = 'field_data_sbp_state';
  $handler->display->display_options['fields']['sbp_state']['field'] = 'sbp_state';
  $handler->display->display_options['fields']['sbp_state']['relationship'] = 'uid';
  $handler->display->display_options['fields']['sbp_state']['label'] = '';
  $handler->display->display_options['fields']['sbp_state']['exclude'] = TRUE;
  $handler->display->display_options['fields']['sbp_state']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sbp_state']['element_default_classes'] = FALSE;
  /* Field: User: Postal Code */
  $handler->display->display_options['fields']['sbp_zip']['id'] = 'sbp_zip';
  $handler->display->display_options['fields']['sbp_zip']['table'] = 'field_data_sbp_zip';
  $handler->display->display_options['fields']['sbp_zip']['field'] = 'sbp_zip';
  $handler->display->display_options['fields']['sbp_zip']['relationship'] = 'uid';
  $handler->display->display_options['fields']['sbp_zip']['label'] = '';
  $handler->display->display_options['fields']['sbp_zip']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['sbp_zip']['alter']['text'] = '[sbp_city], [sbp_state] [sbp_zip]';
  $handler->display->display_options['fields']['sbp_zip']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['sbp_zip']['element_default_classes'] = FALSE;
  /* Field: Commerce Card on File: Operations links */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';
  $handler->display->display_options['fields']['operations']['label'] = '';
  $handler->display->display_options['fields']['operations']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['operations']['add_destination'] = 0;
  /* Field: Commerce Card on File: Edit link */
  $handler->display->display_options['fields']['edit_card']['id'] = 'edit_card';
  $handler->display->display_options['fields']['edit_card']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['edit_card']['field'] = 'edit_card';
  $handler->display->display_options['fields']['edit_card']['label'] = '';
  $handler->display->display_options['fields']['edit_card']['exclude'] = TRUE;
  $handler->display->display_options['fields']['edit_card']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['edit_card']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['edit_card']['text'] = 'Edit';
  /* Field: Commerce Card on File: Delete link */
  $handler->display->display_options['fields']['delete_card']['id'] = 'delete_card';
  $handler->display->display_options['fields']['delete_card']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['fields']['delete_card']['field'] = 'delete_card';
  $handler->display->display_options['fields']['delete_card']['label'] = '';
  $handler->display->display_options['fields']['delete_card']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['delete_card']['alter']['text'] = '[edit_card] [delete_card]';
  $handler->display->display_options['fields']['delete_card']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['delete_card']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['delete_card']['text'] = 'Delete';

  // Empty out any pre-existing sorts.
  $handler->display->display_options['sorts'] = array();
  /* Sort criterion: Commerce Card on File: Default card */
  $handler->display->display_options['sorts']['instance_default']['id'] = 'instance_default';
  $handler->display->display_options['sorts']['instance_default']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['sorts']['instance_default']['field'] = 'instance_default';

  // Empty out any pre-existing arguments.
  $handler->display->display_options['arguments'] = array();
  /* Contextual filter: Commerce Card on File: Uid */
  $handler->display->display_options['arguments']['uid']['id'] = 'uid';
  $handler->display->display_options['arguments']['uid']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['arguments']['uid']['field'] = 'uid';
  $handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';

  // Empty out any pre-existing filters.
  $handler->display->display_options['filters'] = array();
  /* Filter criterion: Commerce Card on File: Card data status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'commerce_cardonfile';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = array(
    1 => '1',
  );

  /* Display: Page */
  $handler = &$views['commerce_card_on_file_user_cards']->display['page']->handler;
  $handler->display->display_options['path'] = 'user/%/cards';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Stored cards';
  $handler->display->display_options['menu']['description'] = 'Manage your stored card data.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
}

/**
 * Implements hook_views_data().
 */
function fundraiser_quick_donate_views_data() {
  $data['fundraiser_quick_donate_cardonfile']['table'] = array(
    'group' => t('Springboard Quick Donations'),
    'base' => array(
      'title' => t('Springboard Quck Donation Opt In'),
      'help' => t('Tracks users opted into quick donations.'),
    ),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
      'users' => array(
        'left_field' => 'uid',
        'field' => 'uid',
      ),
      'commerce_cardonfile' => array(
        'left_field' => 'card_id',
        'field' => 'card_id',
      ),
      'fundraiser_donation' => array(
        'left_field' => 'did',
        'field' => 'did',
      ),
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['nid'] = array(
    'title' => t('Fundraiser node'),
    'help' => t('Fundraiser form node id.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Fundraiser node'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['uid'] = array(
    'title' => t('Quick donation opt-in user'),
    'help' => t('User associated with quick donation opt-in record.'),
    'relationship' => array(
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in user'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['status'] = array(
    'title' => t('Quick donation opt-in status'),
    'help' => t('Whether the user is actively enrolled in quick donations.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['card_id'] = array(
    'title' => t('Quick donation opt-in card ID'),
    'help' => t('The card ID associated with the quick donation.'),
    'relationship' => array(
      'base' => 'commerce_cardonfile',
      'base field' => 'card_id',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in card ID'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['did'] = array(
    'title' => t('Quick donation opt-in donation ID'),
    'help' => t('The donation ID associated with the quick donation.'),
    'relationship' => array(
      'base' => 'fundraiser_donation',
      'base field' => 'did',
      'handler' => 'views_handler_relationship',
      'label' => t('Opt in donation ID'),
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['optin_date'] = array(
    'title' => t('Quick donation opt-in time'),
    'help' => t('Timestamp of when user opted into quick donations.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  $data['fundraiser_quick_donate_cardonfile']['ms'] = array(
    'title' => t('Quick donation market source'),
    'help' => t('Market source value for the quick donation record.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
