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
    'title' => 'Registration',
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
  $display->title = 'Sign up for an Account';
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
    $pane->pid = 'new-3f8ff8a6-0965-475e-aeee-484ec16ba512';
    $pane->panel = 'right';
    $pane->type = 'login_intro';
    $pane->subtype = 'login_intro';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'text' => '',
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-login',
    );
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '3f8ff8a6-0965-475e-aeee-484ec16ba512';
    $display->content['new-3f8ff8a6-0965-475e-aeee-484ec16ba512'] = $pane;
    $display->panels['right'][0] = 'new-3f8ff8a6-0965-475e-aeee-484ec16ba512';
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
    $pane = new stdClass();
    $pane->pid = 'new-667571e6-db71-468f-a95b-ccba6761872a';
    $pane->panel = 'top';
    $pane->type = 'page_messages';
    $pane->subtype = 'page_messages';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-messages',
    );
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '667571e6-db71-468f-a95b-ccba6761872a';
    $display->content['new-667571e6-db71-468f-a95b-ccba6761872a'] = $pane;
    $display->panels['top'][1] = 'new-667571e6-db71-468f-a95b-ccba6761872a';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = '0';
  $handler->conf['display'] = $display;
  $page->default_handlers[$handler->name] = $handler;
  $pages['p2p_registration_page'] = $page;

  $page = new stdClass();
  $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
  $page->api_version = 1;
  $page->name = 'springboard_p2p_login_page';
  $page->task = 'page';
  $page->admin_title = 'Springboard P2P Login page';
  $page->admin_description = '';
  $page->path = 'springboard_p2p/login';
  $page->access = array();
  $page->menu = array();
  $page->arguments = array();
  $page->conf = array(
    'admin_paths' => FALSE,
  );
  $page->default_handlers = array();
  $handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'page_springboard_p2p_login_page_panel_context';
  $handler->task = 'page';
  $handler->subtask = 'springboard_p2p_login_page';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'Panel',
    'no_blocks' => 0,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => 'p2p-page-type p2p-login-page',
    'css_id' => '',
    'css' => '',
    'contexts' => array(),
    'relationships' => array(),
  );
  $display = new panels_display();
  $display->layout = 'onecol';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'middle' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = '';
  $display->uuid = '8c9c3eb7-fd3b-4739-b087-ccb9e2fa6e82';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-8c4878a3-7ad3-43eb-99ea-be1ab11e3358';
    $pane->panel = 'middle';
    $pane->type = 'campaign_header_no_context';
    $pane->subtype = 'campaign_header_no_context';
    $pane->shown = TRUE;
    $pane->access = array();
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
    $pane->uuid = '8c4878a3-7ad3-43eb-99ea-be1ab11e3358';
    $display->content['new-8c4878a3-7ad3-43eb-99ea-be1ab11e3358'] = $pane;
    $display->panels['middle'][0] = 'new-8c4878a3-7ad3-43eb-99ea-be1ab11e3358';
    $pane = new stdClass();
    $pane->pid = 'new-ec87ad4b-670e-4f49-bac0-a7130a8ce60c';
    $pane->panel = 'middle';
    $pane->type = 'custom_title';
    $pane->subtype = 'custom_title';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'text' => 'Already Registered?',
      'override_title' => 1,
      'override_title_text' => '<No Title>',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-title-wrapper',
    );
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = 'ec87ad4b-670e-4f49-bac0-a7130a8ce60c';
    $display->content['new-ec87ad4b-670e-4f49-bac0-a7130a8ce60c'] = $pane;
    $display->panels['middle'][1] = 'new-ec87ad4b-670e-4f49-bac0-a7130a8ce60c';
    $pane = new stdClass();
    $pane->pid = 'new-02324aa6-20f2-4e6d-b2df-09b5f9b6225c';
    $pane->panel = 'middle';
    $pane->type = 'page_messages';
    $pane->subtype = 'page_messages';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-messages',
    );
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '02324aa6-20f2-4e6d-b2df-09b5f9b6225c';
    $display->content['new-02324aa6-20f2-4e6d-b2df-09b5f9b6225c'] = $pane;
    $display->panels['middle'][2] = 'new-02324aa6-20f2-4e6d-b2df-09b5f9b6225c';
    $pane = new stdClass();
    $pane->pid = 'new-b59ab21a-1e06-4fd3-8250-ced0257e94d7';
    $pane->panel = 'middle';
    $pane->type = 'fboauth_login';
    $pane->subtype = 'fboauth_login';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => array(
        'title' => array(
          'prefix' => '',
          'suffix' => '',
        ),
        'content' => array(
          'prefix' => '<div class="panel-pane-wrapper pane-login">',
          'suffix' => '',
        ),
        'theme' => 0,
      ),
      'style' => 'wrapper_raw',
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-fb-login',
    );
    $pane->extras = array();
    $pane->position = 3;
    $pane->locks = array();
    $pane->uuid = 'b59ab21a-1e06-4fd3-8250-ced0257e94d7';
    $display->content['new-b59ab21a-1e06-4fd3-8250-ced0257e94d7'] = $pane;
    $display->panels['middle'][3] = 'new-b59ab21a-1e06-4fd3-8250-ced0257e94d7';
    $pane = new stdClass();
    $pane->pid = 'new-706eea80-ee1a-4f5f-8450-42c0358c5cd1';
    $pane->panel = 'middle';
    $pane->type = 'custom';
    $pane->subtype = 'custom';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'admin_title' => '',
      'title' => '',
      'body' => '<p>OR</p>',
      'format' => 'full_html',
      'substitute' => TRUE,
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-or',
    );
    $pane->extras = array();
    $pane->position = 4;
    $pane->locks = array();
    $pane->uuid = '706eea80-ee1a-4f5f-8450-42c0358c5cd1';
    $display->content['new-706eea80-ee1a-4f5f-8450-42c0358c5cd1'] = $pane;
    $display->panels['middle'][4] = 'new-706eea80-ee1a-4f5f-8450-42c0358c5cd1';
    $pane = new stdClass();
    $pane->pid = 'new-bc08a237-4d8b-4f09-9b18-9922f550802b';
    $pane->panel = 'middle';
    $pane->type = 'login';
    $pane->subtype = 'login';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => array(
        'title' => array(
          'prefix' => '',
          'suffix' => '',
        ),
        'content' => array(
          'prefix' => '',
          'suffix' => '</div>',
        ),
        'theme' => 0,
      ),
      'style' => 'wrapper_raw',
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => '',
    );
    $pane->extras = array();
    $pane->position = 5;
    $pane->locks = array();
    $pane->uuid = 'bc08a237-4d8b-4f09-9b18-9922f550802b';
    $display->content['new-bc08a237-4d8b-4f09-9b18-9922f550802b'] = $pane;
    $display->panels['middle'][5] = 'new-bc08a237-4d8b-4f09-9b18-9922f550802b';
  $display->hide_title = PANELS_TITLE_NONE;
  $display->title_pane = '0';
  $handler->conf['display'] = $display;
  $page->default_handlers[$handler->name] = $handler;
  $pages['springboard_p2p_login_page'] = $page;

  return $pages;

}
