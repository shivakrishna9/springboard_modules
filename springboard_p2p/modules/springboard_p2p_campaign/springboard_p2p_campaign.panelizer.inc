<?php
/**
 * @file
 * springboard_p2p_campaign.panelizer.inc
 */

/**
 * Implements hook_panelizer_defaults().
 */
function springboard_p2p_campaign_panelizer_defaults() {
  $export = array();

  $panelizer = new stdClass();
  $panelizer->disabled = FALSE; /* Edit this to true to make a default panelizer disabled initially */
  $panelizer->api_version = 1;
  $panelizer->name = 'node:p2p_campaign:default';
  $panelizer->title = 'Default';
  $panelizer->panelizer_type = 'node';
  $panelizer->panelizer_key = 'p2p_campaign';
  $panelizer->no_blocks = FALSE;
  $panelizer->css_id = '';
  $panelizer->css = '';
  $panelizer->pipeline = 'standard';
  $panelizer->contexts = array();
  $panelizer->relationships = array();
  $panelizer->access = array();
  $panelizer->view_mode = 'page_manager';
  $panelizer->css_class = 'p2p-page-type';
  $panelizer->title_element = 'H2';
  $panelizer->link_to_entity = TRUE;
  $panelizer->extra = array();
  $display = new panels_display();
  $display->layout = 'p2p_landing_twocol_bricks';
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
  $display->uuid = '542f3390-cfa6-4710-96c6-8d1649f14a4b';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-5c0b9c01-b1ae-4bdb-8808-77979ea83c35';
    $pane->panel = 'middle_left';
    $pane->type = 'views_panes';
    $pane->subtype = 'p2p_campaign-panel_pane_1';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '5c0b9c01-b1ae-4bdb-8808-77979ea83c35';
    $display->content['new-5c0b9c01-b1ae-4bdb-8808-77979ea83c35'] = $pane;
    $display->panels['middle_left'][0] = 'new-5c0b9c01-b1ae-4bdb-8808-77979ea83c35';
    $pane = new stdClass();
    $pane->pid = 'new-19ad46df-6219-453a-b6bc-2426c9092b4e';
    $pane->panel = 'middle_right';
    $pane->type = 'views_panes';
    $pane->subtype = 'p2p_top_personal_campaigns_by_category-panel_pane_1';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '19ad46df-6219-453a-b6bc-2426c9092b4e';
    $display->content['new-19ad46df-6219-453a-b6bc-2426c9092b4e'] = $pane;
    $display->panels['middle_right'][0] = 'new-19ad46df-6219-453a-b6bc-2426c9092b4e';
    $pane = new stdClass();
    $pane->pid = 'new-9f85f546-7f9c-4847-8315-a98b4835fff9';
    $pane->panel = 'middle_right';
    $pane->type = 'block';
    $pane->subtype = 'views-767aa344537c01f1aa6bf0d0620b80ac';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'inherit_path' => 1,
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '9f85f546-7f9c-4847-8315-a98b4835fff9';
    $display->content['new-9f85f546-7f9c-4847-8315-a98b4835fff9'] = $pane;
    $display->panels['middle_right'][1] = 'new-9f85f546-7f9c-4847-8315-a98b4835fff9';
    $pane = new stdClass();
    $pane->pid = 'new-728990ea-0369-4f19-ba3c-be7af0dc1543';
    $pane->panel = 'middle_right';
    $pane->type = 'custom';
    $pane->subtype = 'pane_search_link';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'admin_title' => '',
      'title' => '',
      'body' => '',
      'format' => 'filtered_html',
      'substitute' => TRUE,
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '728990ea-0369-4f19-ba3c-be7af0dc1543';
    $display->content['new-728990ea-0369-4f19-ba3c-be7af0dc1543'] = $pane;
    $display->panels['middle_right'][2] = 'new-728990ea-0369-4f19-ba3c-be7af0dc1543';
    $pane = new stdClass();
    $pane->pid = 'new-86d7e57f-ef28-4822-9b16-7aedb4af3c10';
    $pane->panel = 'middle_right';
    $pane->type = 'views_panes';
    $pane->subtype = 'search_for_a_campaign-panel_pane_1';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 3;
    $pane->locks = array();
    $pane->uuid = '86d7e57f-ef28-4822-9b16-7aedb4af3c10';
    $display->content['new-86d7e57f-ef28-4822-9b16-7aedb4af3c10'] = $pane;
    $display->panels['middle_right'][3] = 'new-86d7e57f-ef28-4822-9b16-7aedb4af3c10';
    $pane = new stdClass();
    $pane->pid = 'new-fb351353-71d7-4a33-85d9-b9c30d616b33';
    $pane->panel = 'top';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_campaign_banner';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'title',
      'formatter' => 'image',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(
        'image_style' => 'p2p_top_banner',
        'image_link' => '',
      ),
      'context' => 'panelizer',
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = 'fb351353-71d7-4a33-85d9-b9c30d616b33';
    $display->content['new-fb351353-71d7-4a33-85d9-b9c30d616b33'] = $pane;
    $display->panels['top'][0] = 'new-fb351353-71d7-4a33-85d9-b9c30d616b33';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = '0';
  $panelizer->display = $display;
  $export['node:p2p_campaign:default'] = $panelizer;

  return $export;
}
