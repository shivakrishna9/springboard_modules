<?php

$plugin = array(
  'single' => TRUE,
  // Just do this one, it is needed.
  'title' => t('P2P Custom Pane Title'),
  // Title to show up on the pane screen.
  'description' => t('ER MAH GERD, ERT DERS THINS'),
  // Description to show up on the pane screen.
  'category' => t('Springboard P2P'),
  'edit form' => 'springboard_p2p_custom_title_edit_form',
  // A function that will return the settings form for the pane.
  'render callback' => 'springboard_p2p_custom_title_render',
  // A function that will return the renderable content.
  'admin info' => 'springboard_p2p_custom_title_admin_info',
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
function springboard_p2p_custom_title_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  // Custom textfield for our <h1> title.
  $form['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Panel Text'),
    '#description' => t('Text to display, it may use substitution strings'),
    '#default_value' => $conf['text'],
  );

  return $form;
}

/**
 * Submit function, note anything in the formstate[conf] automatically gets saved
 * Notice, the magic that automatically does that for you.
 */
function springboard_p2p_custom_title_edit_form_submit(&$form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Run-time rendering of the body of the block (content type)
 * See ctools_plugin_examples for more advanced info
 */
function springboard_p2p_custom_title_render($subtype, $conf, $args, $contexts) {

  // Initial content is blank.
  $block = new stdClass();
  $block->content = '<h1>' . $conf['text'] . '</h1>';

  return $block;
}
