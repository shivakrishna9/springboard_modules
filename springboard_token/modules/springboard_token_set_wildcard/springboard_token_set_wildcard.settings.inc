<?php

/**
 * Returns current path wildcard+field definitions.
 */
function _springboard_token_set_wildcard_get_wildcard_definitions() {
  $wildcard_settings = array();

  // Springboard tags list:
  $wildcard_settings['sb_tags_list_add'] = array(
    'wildcard_path' => '/^admin\/springboard\/springboard-tags\/add/',
    'add_form_id' => 'ctools_export_ui_edit_item_form',
    'edit_wildcard' => 'sb_tags_list_edit',
    'next_id_query' => NULL,
    'next_id_query_tokens' => array(),
    'next_id_form_submitted_field_key' => 'admin_title',
    'edit_path' => 'admin/springboard/springboard-tags/list/:admin_title/edit',
    'edit_path_token_names' => array(':admin_title'),
    'form_keys' => array(
      'tag',
      'admin_description',
    ),
    'default_tsids' => array(
      'tag' => 21,
      'admin_description' => 21, 
    ),
  );
  $wildcard_settings['sb_tags_list_edit'] = array(
    'wildcard_path' => '/^admin\/springboard\/springboard-tags\/list\/\w+\/edit/',
    'form_keys' => $wildcard_settings['sb_tags_list_add']['form_keys'],
    'default_tsids' => $wildcard_settings['sb_tags_list_add']['default_tsids'],
  );

  // Webform confirmation page settings:
  $wildcard_settings['webform_conf_page'] = array(
    'wildcard_path' => '/^node\/\d+\/webform\/configure/',
    'form_keys' => array(
      'confirmation',
      'submit_description'),
    'default_tsids' => array(
      'confirmation' => 9,
      'submit_description' => 9, 
    ),
  );

  // webform email component (add and edit forms):
  $wildcard_settings['webform_email_comp_add'] = array(
    'wildcard_path' => '/^node\/\d+\/webform\/emails\/new/',
    'add_form_id' => 'webform_email_edit_form',
    'edit_wildcard' => 'webform_email_comp_edit',
    'next_id_query' => "SELECT nid, eid FROM {webform_emails} ORDER BY eid DESC",
    'next_id_query_tokens' => array(),
    'edit_path' => 'node/:nid/webform/emails/:eid',
    'edit_path_token_names' => array(':nid', ':eid'),
    'form_keys' => array('template'),
    'default_tsids' => array('template' => 8),
  );
  $wildcard_settings['webform_email_comp_edit'] = array(
    'wildcard_path' => '/^node\/\d+\/webform\/emails\/\d+/',
    'form_keys' => $wildcard_settings['webform_email_comp_add']['form_keys'],
    'default_tsids' => $wildcard_settings['webform_email_comp_add']['default_tsids'],
  );

  // Webform component (add and edit forms):
  $wildcard_settings['webform_component_add'] = array(
    'wildcard_path' => '/^node\/\d+\/webform\/components\/new\//',
    'add_form_id' => 'webform_component_edit_form',
    'edit_wildcard' => 'webform_component_edit',
    'next_id_query' => "SELECT nid, cid FROM {webform_component} WHERE nid = :nid ORDER BY cid DESC",
    'next_id_query_tokens' => array(':nid' => TRUE),
    'edit_path' => 'node/:nid/webform/components/:cid',
    'edit_path_token_names' => array(':nid', ':cid'),
    'form_keys' => array('description'),
    'default_tsids' => array('description' => 9),
  );
  $wildcard_settings['webform_component_edit'] = array(
    'wildcard_path' => '/^node\/\d+\/webform\/components\/\d+/',
    'form_keys' => $wildcard_settings['webform_component_add']['form_keys'],
    'default_tsids' => $wildcard_settings['webform_component_add']['default_tsids'],
  );

  // Message action message (add and edit forms):
  $wildcard_settings['message_action_message_add'] = array(
    'wildcard_path' => '/^node\/\d+\/messages\/add\/sba_message_action_message/',
    'add_form_id' => 'sba_message_edit_form',
    'edit_wildcard' => 'message_action_message_edit',
    'next_id_query' => "SELECT sba_message_id FROM {sba_message} ORDER BY sba_message_id DESC",
    'next_id_query_tokens' => array(),
    'edit_path' => 'node/:nid/messages/sba_message/:sba_message_id/edit',
    'edit_tokens_args' => array(
      ':nid' => 1,
    ),
    'edit_path_token_names' => array(':sba_message_id', ':nid'),
    'form_keys' => array(
      'field_sba_message',
      'field_sba_signature',
    ),
    'default_tsids' => array(
      'field_sba_message' => 17,
      'field_sba_signature' => 17,
    ),
  );
  $wildcard_settings['message_action_message_edit'] = array(
    'wildcard_path' => '/^node\/\d+\/messages\/sba_message\/\d+\/edit/',
    'form_keys' => $wildcard_settings['message_action_message_add']['form_keys'],
    'default_tsids' => $wildcard_settings['message_action_message_add']['default_tsids'],
  );

  // Social share:
  $wildcard_settings['social_share'] = array(
    'wildcard_path' => '/^node\/\d+\/share_settings/',
    'form_keys' => array('fb_description', 'email_message'),
    'default_tsids' => array(
      'fb_description' => 6,
      'email_message' => 6,
    ),
  );

  // Webform goals (add and edit forms):
  $wildcard_settings['webform_goals_add'] = array(
    'wildcard_path' => '/^admin\/config\/content\/webform_goals\/add/',
    'add_form_id' => 'webform_goals_add',
    'edit_wildcard' => 'webform_goals_edit',
    'next_id_query' => "SELECT gid FROM {webform_goals} ORDER BY gid DESC",
    'next_id_query_tokens' => array(),
    'edit_path' => 'admin/config/content/webform_goals/:gid/edit',
    'edit_path_token_names' => array(':gid'),
    'form_keys' => array(
      'starter_message',
      'template',
    ),
    'default_tsids' => array(
      'starter_message' => 20,
      'template' => 20,
    ),
  );
  $wildcard_settings['webform_goals_edit'] = array(
    'wildcard_path' => '/^admin\/config\/content\/webform_goals\/\d+\/edit/',
    'form_keys' => $wildcard_settings['webform_goals_add']['form_keys'],
    'default_tsids' => $wildcard_settings['webform_goals_add']['default_tsids'],
  );

  return $wildcard_settings;
}

