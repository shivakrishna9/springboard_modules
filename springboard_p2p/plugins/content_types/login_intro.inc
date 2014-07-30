<?php
/**
 * @file
 * Plugin to display a get started pane.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Custom Login Pane'),
  'description' => t('Login Intro'),
  'category' => t('Springboard P2P'),
  // A category to put this under.
  'edit form' => 'springboard_p2p_login_intro_edit_form',
  // A function that will return the settings form for the pane.
  'render callback' => 'springboard_p2p_login_intro_render',
  // A function that will return the renderable content.
  'admin info' => 'springboard_p2p_login_intro_admin_info',
  // A function that will return the information displayed on the admin screen (optional).
  'defaults' => array( // Array of defaults for the settings form.
    'text' => '',
  ),
  'all contexts' => TRUE,
  // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function springboard_p2p_login_intro_edit_form($form, &$form_state) {
  // No settings beyond context, which has already been handled.
  return $form;
}

/**
 * Run-time rendering of the body of the block (content type).
 *
 * See ctools_plugin_examples for more advanced info.
 */
function springboard_p2p_login_intro_render($subtype, $conf, $args, $contexts) {

  // Title.
  $title = t('Already a Supporter?');

  // Pull the login message text from P2P Settings.
  $logintext = variable_get('springboard_p2p_login_message_area');

  // Define the reg link passing the string id.
  $loginlink = '/p2p/login';

  // Query the url string campaign id if it's not empty.
  if (!empty($_GET['p2p_cid'])) {
    $urlstring = $_GET['p2p_cid'];
    $loginlinkid = '?p2p_cid=' . $urlstring;
  }

  $output = array();

  // Create the output to be rendered.
  if (empty($_GET['p2p_cid'])) {
    $output[] = '<a class="get-started-link" href=" ' . $loginlink . ' ">' . t('Login') . '<i class="fa fa-cog fa-spin get-started-spinner"></i></a>';
  }

  else {
    $output[] = '<a class="get-started-link" href=" ' . $loginlink . $loginlinkid . ' ">' . t('Login') . '<i class="fa fa-cog fa-spin get-started-spinner"></i></a>';
  }

  // Render the login message text.
  $output[] = '<div class="login-text-wrapper"> ';
  $output[] = $logintext['value'];
  $output[] = '</div>';

  // Initial content is blank.
  // Build the content type block.
  $block = new stdClass();
  $block->title = $title;
  $block->content = implode('', $output);
  return $block;

}
