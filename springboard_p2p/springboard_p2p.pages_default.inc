<?php
/**
 * @file
 * springboard_p2p.pages_default.inc
 */

/**
 * Implements hook_default_page_manager_pages().
 */
function springboard_p2p_default_page_manager_pages() {
  $page = new stdClass();
  $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
  $page->api_version = 1;
  $page->name = 'p2p_registration_page';
  $page->task = 'page';
  $page->admin_title = 'Springboard P2P registration page';
  $page->admin_description = '';
  $page->path = 'springboard_p2p/register';
  $page->access = array(
    'plugins' => array(
      0 => array(
        'name' => 'perm',
        'settings' => array(
          'perm' => 'register for springboard p2p',
        ),
        'context' => 'logged-in-user',
        'not' => FALSE,
      ),
    ),
    'logic' => 'and',
    'type' => 'none',
    'settings' => NULL,
  );
  $page->menu = array();
  $page->arguments = array();
  $page->conf = array(
    'admin_paths' => FALSE,
  );
  $page->default_handlers = array();
  $handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'page_p2p_registration_page_panel_context';
  $handler->task = 'page';
  $handler->subtask = 'p2p_registration_page';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'Panel',
    'no_blocks' => 0,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => 'p2p-page-type',
    'css_id' => '',
    'css' => '',
    'contexts' => array(),
    'relationships' => array(),
  );
  $display = new panels_display();
  $display->layout = 'p2p_threecol_stacked';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'top' => NULL,
      'left' => NULL,
      'middle' => NULL,
      'right' => NULL,
      'bottom' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '';
  $display->uuid = '67bb0998-9a48-4122-99a9-08f2b979df70';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-98c8de5f-7ec0-4e9a-a90a-133af5b6c99c';
    $pane->panel = 'left';
    $pane->type = 'fboauth_login';
    $pane->subtype = 'fboauth_login';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-social-login',
    );
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '98c8de5f-7ec0-4e9a-a90a-133af5b6c99c';
    $display->content['new-98c8de5f-7ec0-4e9a-a90a-133af5b6c99c'] = $pane;
    $display->panels['left'][0] = 'new-98c8de5f-7ec0-4e9a-a90a-133af5b6c99c';
    $pane = new stdClass();
    $pane->pid = 'new-4258aeae-338f-463f-862a-64f2b307951a';
    $pane->panel = 'middle';
    $pane->type = 'register';
    $pane->subtype = 'register';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'reg-pane',
    );
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '4258aeae-338f-463f-862a-64f2b307951a';
    $display->content['new-4258aeae-338f-463f-862a-64f2b307951a'] = $pane;
    $display->panels['middle'][0] = 'new-4258aeae-338f-463f-862a-64f2b307951a';
    $pane = new stdClass();
    $pane->pid = 'new-770ce805-317b-4423-8075-448f5441d6b9';
    $pane->panel = 'top';
    $pane->type = 'campaign_header_no_context';
    $pane->subtype = 'campaign_header_no_context';
    $pane->shown = TRUE;
    $pane->access = array(
      'plugins' => array(),
    );
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-campaign-header',
    );
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '770ce805-317b-4423-8075-448f5441d6b9';
    $display->content['new-770ce805-317b-4423-8075-448f5441d6b9'] = $pane;
    $display->panels['top'][0] = 'new-770ce805-317b-4423-8075-448f5441d6b9';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-98c8de5f-7ec0-4e9a-a90a-133af5b6c99c';
  $handler->conf['display'] = $display;
  $page->default_handlers[$handler->name] = $handler;
  $pages['p2p_registration_page'] = $page;

  return $pages;

}
