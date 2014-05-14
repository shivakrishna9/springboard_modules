<?php
/**
 * @file
 * springboard_p2p_landing.panelizer.inc
 */

/**
 * Implements hook_panelizer_defaults().
 */
function springboard_p2p_landing_panelizer_defaults() {
  $export = array();

  $panelizer = new stdClass();
  $panelizer->disabled = FALSE; /* Edit this to true to make a default panelizer disabled initially */
  $panelizer->api_version = 1;
  $panelizer->name = 'node:p2p_campaign_landing:default';
  $panelizer->title = 'Default';
  $panelizer->panelizer_type = 'node';
  $panelizer->panelizer_key = 'p2p_campaign_landing';
  $panelizer->no_blocks = FALSE;
  $panelizer->css_id = '';
  $panelizer->css = '';
  $panelizer->pipeline = 'standard';
  $panelizer->contexts = array();
  $panelizer->relationships = array();
  $panelizer->access = array();
  $panelizer->view_mode = 'page_manager';
  $panelizer->css_class = 'p2p-panelizer-campaign-landing';
  $panelizer->title_element = 'H2';
  $panelizer->link_to_entity = TRUE;
  $panelizer->extra = array();
  $display = new panels_display();
  $display->layout = 'p2p_landing_twocol_bricks';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'center' => NULL,
      'top' => NULL,
      'middle_left' => NULL,
      'middle_right' => NULL,
      'bottom' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '%node:title';
  $display->uuid = 'f63f1a99-39ec-4e1e-a3de-887fd61e4808';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-75fdd4db-bb0b-4436-b8ea-5454fc55fbf9';
    $pane->panel = 'middle_left';
    $pane->type = 'views_panes';
    $pane->subtype = 'p2p_referenced_campaigns-panel_pane_1';
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
    $pane->uuid = '75fdd4db-bb0b-4436-b8ea-5454fc55fbf9';
    $display->content['new-75fdd4db-bb0b-4436-b8ea-5454fc55fbf9'] = $pane;
    $display->panels['middle_left'][0] = 'new-75fdd4db-bb0b-4436-b8ea-5454fc55fbf9';
    $pane = new stdClass();
    $pane->pid = 'new-b8c28bff-a379-4ecb-9df7-6299d4bcbe6f';
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
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = 'b8c28bff-a379-4ecb-9df7-6299d4bcbe6f';
    $display->content['new-b8c28bff-a379-4ecb-9df7-6299d4bcbe6f'] = $pane;
    $display->panels['middle_right'][0] = 'new-b8c28bff-a379-4ecb-9df7-6299d4bcbe6f';
    $pane = new stdClass();
    $pane->pid = 'new-92074d7e-71a2-414e-8997-b8b69ef41adc';
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
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '92074d7e-71a2-414e-8997-b8b69ef41adc';
    $display->content['new-92074d7e-71a2-414e-8997-b8b69ef41adc'] = $pane;
    $display->panels['middle_right'][1] = 'new-92074d7e-71a2-414e-8997-b8b69ef41adc';
    $pane = new stdClass();
    $pane->pid = 'new-2b715f39-4aca-4454-b3ba-ee14443b1274';
    $pane->panel = 'middle_right';
    $pane->type = 'entity_field';
    $pane->subtype = 'node:field_p2p_side_block';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'label' => 'hidden',
      'formatter' => 'blockreference_without_title',
      'delta_limit' => 0,
      'delta_offset' => '0',
      'delta_reversed' => FALSE,
      'formatter_settings' => array(),
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
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '2b715f39-4aca-4454-b3ba-ee14443b1274';
    $display->content['new-2b715f39-4aca-4454-b3ba-ee14443b1274'] = $pane;
    $display->panels['middle_right'][2] = 'new-2b715f39-4aca-4454-b3ba-ee14443b1274';
    $pane = new stdClass();
    $pane->pid = 'new-9bb0248f-6996-43d9-bfae-970ef5cf9e44';
    $pane->panel = 'top';
    $pane->type = 'views_panes';
    $pane->subtype = 'p2p_campaign_landing_slideshow-panel_pane_1';
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
    $pane->uuid = '9bb0248f-6996-43d9-bfae-970ef5cf9e44';
    $display->content['new-9bb0248f-6996-43d9-bfae-970ef5cf9e44'] = $pane;
    $display->panels['top'][0] = 'new-9bb0248f-6996-43d9-bfae-970ef5cf9e44';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-75fdd4db-bb0b-4436-b8ea-5454fc55fbf9';
  $panelizer->display = $display;
  $export['node:p2p_campaign_landing:default'] = $panelizer;

  return $export;
}
