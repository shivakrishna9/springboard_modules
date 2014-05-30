<?php

$plugin = array(
  'single' => TRUE,
  // Just do this one, it is needed.
  'title' => t('Custom Pane Title'),
  'description' => t('Custom title override pane'),
  'category' => t('Springboard P2P'),
  'edit form' => 'springboard_p2p_custom_title_edit_form',
  'render callback' => 'springboard_p2p_custom_title_render',
  'defaults' => array( // Array of defaults for the settings form.
    'text' => '',
    'override_title' => FALSE,
    'override_title_text' => '<no title>',
  ),
  'all contexts' => TRUE,
  // This is NEEDED to be able to use substitution strings in your pane.
);

/**
 * An edit form for the pane's settings.
 */
function springboard_p2p_custom_title_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  // Hide the pane title  / override, we don't want it and it's not a required field.
  $form['override_title_text'] = array(
    '#type' => '',
  );

  $form['override_title'] = array(
    '#type' => '',
  );

  $form['override_title_markup'] = array(
    '#markup' => t(''),
  );

  // Custom textfield for our <h1> title.
  $form['text'] = array(
    '#type' => 'textfield',
    '#title' => t('Panel Text'),
    '#description' => t('Use this textbox for an h1 title pane.'),
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

  // Define the output array.
  $output = array();

  // Render the textfield as an h1 tag since panels core does not work for that too well.
  $output[] =  '<h1 class="pane-title">' . $conf['text'] . '</h1>';

  // Initial content is blank.
  $block = new stdClass();
  $block->title = '';
  $block->content = implode('', $output);

  return $block;
}
