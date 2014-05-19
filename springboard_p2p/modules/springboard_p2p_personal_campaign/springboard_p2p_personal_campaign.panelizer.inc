<?php
/**
 * @file
 * springboard_p2p_personal_campaign.panelizer.inc
 */

/**
 * Implements hook_panelizer_defaults().
 */
function springboard_p2p_personal_campaign_panelizer_defaults() {
  $export = array();

  $panelizer = new stdClass();
  $panelizer->disabled = FALSE; /* Edit this to true to make a default panelizer disabled initially */
  $panelizer->api_version = 1;
  $panelizer->name = 'node:p2p_personal_campaign:default';
  $panelizer->title = 'Default';
  $panelizer->panelizer_type = 'node';
  $panelizer->panelizer_key = 'p2p_personal_campaign';
  $panelizer->no_blocks = FALSE;
  $panelizer->css_id = '';
  $panelizer->css = '';
  $panelizer->pipeline = 'standard';
  $panelizer->contexts = array();
  $panelizer->relationships = array();
  $panelizer->access = array();
  $panelizer->view_mode = 'page_manager';
  $panelizer->css_class = '';
  $panelizer->title_element = 'H2';
  $panelizer->link_to_entity = TRUE;
  $panelizer->extra = array();
  $display = new panels_display();
  $display->layout = 'p2p_twocol_bricks';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'top' => NULL,
      'middle_left' => NULL,
      'middle_right' => NULL,
      'bottom' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '%node:title';
  $display->uuid = '3a5a357b-35c3-404d-95c7-710fe59c4a26';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-0cd4f51e-4ba7-474d-a3ad-e1a931ded90f';
    $pane->panel = 'middle_left';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:body';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'hidden',
      'formatter' => 'text_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '0cd4f51e-4ba7-474d-a3ad-e1a931ded90f';
    $display->content['new-0cd4f51e-4ba7-474d-a3ad-e1a931ded90f'] = $pane;
    $display->panels['middle_left'][0] = 'new-0cd4f51e-4ba7-474d-a3ad-e1a931ded90f';
    $pane = new stdClass();
    $pane->pid = 'new-cd2b586f-36a8-4776-bd9d-773c7f27dfc8';
    $pane->panel = 'middle_left';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_campaign';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'entityreference_label',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'link' => FALSE,
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = 'cd2b586f-36a8-4776-bd9d-773c7f27dfc8';
    $display->content['new-cd2b586f-36a8-4776-bd9d-773c7f27dfc8'] = $pane;
    $display->panels['middle_left'][1] = 'new-cd2b586f-36a8-4776-bd9d-773c7f27dfc8';
    $pane = new stdClass();
    $pane->pid = 'new-a6baa2cb-e251-4a0e-9f6b-83446a4dcdf3';
    $pane->panel = 'middle_left';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_video_embed';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'video_embed_field',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'description' => 1,
        'description_position' => 'bottom',
        'video_style' => 'normal',
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = 'a6baa2cb-e251-4a0e-9f6b-83446a4dcdf3';
    $display->content['new-a6baa2cb-e251-4a0e-9f6b-83446a4dcdf3'] = $pane;
    $display->panels['middle_left'][2] = 'new-a6baa2cb-e251-4a0e-9f6b-83446a4dcdf3';
    $pane = new stdClass();
    $pane->pid = 'new-c8617b25-8dae-48c6-bd81-cea6fc4df20d';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_fundraising_deadline';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'date_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'format_type' => 'long',
        'fromto' => 'both',
        'multiple_from' => '',
        'multiple_number' => '',
        'multiple_to' => '',
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = 'c8617b25-8dae-48c6-bd81-cea6fc4df20d';
    $display->content['new-c8617b25-8dae-48c6-bd81-cea6fc4df20d'] = $pane;
    $display->panels['middle_right'][0] = 'new-c8617b25-8dae-48c6-bd81-cea6fc4df20d';
    $pane = new stdClass();
    $pane->pid = 'new-d4ef12a9-ee7d-44ff-8747-3e42752e1988';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_fundraising_deadline';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'date_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'format_type' => 'long',
        'fromto' => 'both',
        'multiple_from' => '',
        'multiple_number' => '',
        'multiple_to' => '',
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = 'd4ef12a9-ee7d-44ff-8747-3e42752e1988';
    $display->content['new-d4ef12a9-ee7d-44ff-8747-3e42752e1988'] = $pane;
    $display->panels['middle_right'][1] = 'new-d4ef12a9-ee7d-44ff-8747-3e42752e1988';
    $pane = new stdClass();
    $pane->pid = 'new-b2d525b3-d6e2-4581-bc06-ef5d0b54d511';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_personal_campaign_url';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'text_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = 'b2d525b3-d6e2-4581-bc06-ef5d0b54d511';
    $display->content['new-b2d525b3-d6e2-4581-bc06-ef5d0b54d511'] = $pane;
    $display->panels['middle_right'][2] = 'new-b2d525b3-d6e2-4581-bc06-ef5d0b54d511';
    $pane = new stdClass();
    $pane->pid = 'new-5adb8079-671d-4846-9098-53e47759e265';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_personal_campaign_url';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'text_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 3;
    $pane->locks = array();
    $pane->uuid = '5adb8079-671d-4846-9098-53e47759e265';
    $display->content['new-5adb8079-671d-4846-9098-53e47759e265'] = $pane;
    $display->panels['middle_right'][3] = 'new-5adb8079-671d-4846-9098-53e47759e265';
    $pane = new stdClass();
    $pane->pid = 'new-85c17e1a-aa0a-4d13-86ec-dabb5bc7456d';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_personal_submit_goal';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'text_default',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 4;
    $pane->locks = array();
    $pane->uuid = '85c17e1a-aa0a-4d13-86ec-dabb5bc7456d';
    $display->content['new-85c17e1a-aa0a-4d13-86ec-dabb5bc7456d'] = $pane;
    $display->panels['middle_right'][4] = 'new-85c17e1a-aa0a-4d13-86ec-dabb5bc7456d';
    $pane = new stdClass();
    $pane->pid = 'new-680839c7-6c53-41bf-bbf8-400322b078da';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_personal_campaign_goal';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'number_decimal',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'decimal_separator' => '.',
        'prefix_suffix' => TRUE,
        'scale' => 2,
        'thousand_separator' => ' ',
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 5;
    $pane->locks = array();
    $pane->uuid = '680839c7-6c53-41bf-bbf8-400322b078da';
    $display->content['new-680839c7-6c53-41bf-bbf8-400322b078da'] = $pane;
    $display->panels['middle_right'][5] = 'new-680839c7-6c53-41bf-bbf8-400322b078da';
    $pane = new stdClass();
    $pane->pid = 'new-8c5c3580-a2e6-475f-92ce-03ecdb6df16a';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_campaign_images';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'image',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'image_link' => '',
        'image_style' => '',
      ),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 6;
    $pane->locks = array();
    $pane->uuid = '8c5c3580-a2e6-475f-92ce-03ecdb6df16a';
    $display->content['new-8c5c3580-a2e6-475f-92ce-03ecdb6df16a'] = $pane;
    $display->panels['middle_right'][6] = 'new-8c5c3580-a2e6-475f-92ce-03ecdb6df16a';
    $pane = new stdClass();
    $pane->pid = 'new-543e8722-f0d1-4104-9bf6-e15783a9b33d';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_campaign_progress';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'above',
      'formatter' => 'springboard_p2p_fields_personal_campaign_progress_auto',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
      'context' => 'panelizer',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 7;
    $pane->locks = array();
    $pane->uuid = '543e8722-f0d1-4104-9bf6-e15783a9b33d';
    $display->content['new-543e8722-f0d1-4104-9bf6-e15783a9b33d'] = $pane;
    $display->panels['middle_right'][7] = 'new-543e8722-f0d1-4104-9bf6-e15783a9b33d';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-0cd4f51e-4ba7-474d-a3ad-e1a931ded90f';
  $panelizer->display = $display;
  $export['node:p2p_personal_campaign:default'] = $panelizer;

  return $export;
}
