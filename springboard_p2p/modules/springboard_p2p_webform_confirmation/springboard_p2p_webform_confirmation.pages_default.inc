<?php
/**
 * @file
 * springboard_p2p_webform_confirmation.pages_default.inc
 */

/**
 * Implements hook_default_page_manager_handlers().
 */
function springboard_p2p_webform_confirmation_default_page_manager_handlers() {
  $export = array();

  $handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'pm_existing_pages_p2p_webform_confirmation_panel_context';
  $handler->task = 'pm_existing_pages';
  $handler->subtask = 'p2p_webform_confirmation';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'P2P Donation Confirmation Page',
    'no_blocks' => 0,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => 'p2p-page-type',
    'css_id' => '',
    'css' => '',
    'contexts' => array(),
    'relationships' => array(),
    'access' => array(
      'logic' => 'and',
      'plugins' => array(
        0 => array(
          'name' => 'donation_thank_you',
          'settings' => NULL,
          'context' => 'argument_entity_id:node_1',
          'not' => FALSE,
        ),
      ),
    ),
  );
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
  $display->title = 'Thank you for supporting my campaign';
  $display->uuid = '78059b1d-cc5a-49a3-9d54-57685c7f82f5';
  $display->content = array();
  $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-bcec0694-5531-400c-8d05-d7521fb72c98';
    $pane->panel = 'top';
    $pane->type = 'campaign_header_no_context';
    $pane->subtype = 'campaign_header_no_context';
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
    $pane->uuid = 'bcec0694-5531-400c-8d05-d7521fb72c98';
    $display->content['new-bcec0694-5531-400c-8d05-d7521fb72c98'] = $pane;
    $display->panels['top'][0] = 'new-bcec0694-5531-400c-8d05-d7521fb72c98';
    $pane = new stdClass();
    $pane->pid = 'new-a179f976-122c-4cd6-af6e-f4e0ab7e609b';
    $pane->panel = 'top';
    $pane->type = 'custom_title';
    $pane->subtype = 'custom_title';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'text' => 'Thank you for supporting my campaign',
      'markup_tag' => 'h1',
      'override_title' => FALSE,
      'override_title_text' => '<no title>',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
      'style' => 'naked',
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'confirm-title',
    );
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = 'a179f976-122c-4cd6-af6e-f4e0ab7e609b';
    $display->content['new-a179f976-122c-4cd6-af6e-f4e0ab7e609b'] = $pane;
    $display->panels['top'][1] = 'new-a179f976-122c-4cd6-af6e-f4e0ab7e609b';
    $pane = new stdClass();
    $pane->pid = 'new-ee048603-b569-4e46-b3c7-a65191b2a187';
    $pane->panel = 'top';
    $pane->type = 'custom_title';
    $pane->subtype = 'custom_title';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'text' => 'Help me spread the word. Share my campaign with your friends.',
      'markup_tag' => 'h2',
      'override_title' => FALSE,
      'override_title_text' => '<no title>',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
      'style' => 'naked',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = 'ee048603-b569-4e46-b3c7-a65191b2a187';
    $display->content['new-ee048603-b569-4e46-b3c7-a65191b2a187'] = $pane;
    $display->panels['top'][2] = 'new-ee048603-b569-4e46-b3c7-a65191b2a187';
    $pane = new stdClass();
    $pane->pid = 'new-74ca6fd2-c234-4242-8e6d-48033fb76985';
    $pane->panel = 'top';
    $pane->type = 'custom';
    $pane->subtype = 'custom';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'admin_title' => '',
      'title' => '',
      'body' => '<ul><li>[sb_social:facebook_share_link]</li><li>[sb_social:twitter_share_link]</li></ul>',
      'format' => 'full_html',
      'substitute' => 1,
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'p2p-social-pane',
    );
    $pane->extras = array();
    $pane->position = 3;
    $pane->locks = array();
    $pane->uuid = '74ca6fd2-c234-4242-8e6d-48033fb76985';
    $display->content['new-74ca6fd2-c234-4242-8e6d-48033fb76985'] = $pane;
    $display->panels['top'][3] = 'new-74ca6fd2-c234-4242-8e6d-48033fb76985';
    $pane = new stdClass();
    $pane->pid = 'new-1aa6f706-2978-4bc9-8bda-7afcc18d304a';
    $pane->panel = 'top';
    $pane->type = 'custom';
    $pane->subtype = 'custom';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'admin_title' => 'signup',
      'title' => '',
      'body' => '<p><a href="/p2p/register">Sign up here to start your own campaign.</a></p>',
      'format' => 'full_html',
      'substitute' => 1,
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array(
      'css_id' => '',
      'css_class' => 'pane-signup',
    );
    $pane->extras = array();
    $pane->position = 4;
    $pane->locks = array();
    $pane->uuid = '1aa6f706-2978-4bc9-8bda-7afcc18d304a';
    $display->content['new-1aa6f706-2978-4bc9-8bda-7afcc18d304a'] = $pane;
    $display->panels['top'][4] = 'new-1aa6f706-2978-4bc9-8bda-7afcc18d304a';
    $pane = new stdClass();
    $pane->pid = 'new-12ef7582-b921-4b36-a1d8-4092c1de8cb0';
    $pane->panel = 'top';
    $pane->type = 'pm_existing_pages';
    $pane->subtype = 'pm_existing_pages';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      0 => 'task_id',
      1 => 'pane_title_from_menu',
      'task_id' => 'node_overview',
      'pane_title_from_menu' => 0,
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 5;
    $pane->locks = array();
    $pane->uuid = '12ef7582-b921-4b36-a1d8-4092c1de8cb0';
    $display->content['new-12ef7582-b921-4b36-a1d8-4092c1de8cb0'] = $pane;
    $display->panels['top'][5] = 'new-12ef7582-b921-4b36-a1d8-4092c1de8cb0';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-a179f976-122c-4cd6-af6e-f4e0ab7e609b';
  $handler->conf['display'] = $display;
  $export['pm_existing_pages_p2p_webform_confirmation_panel_context'] = $handler;

  return $export;
}
