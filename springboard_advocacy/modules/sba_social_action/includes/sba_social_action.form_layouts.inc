<?php

/**
 * @file
 * Helper functions for message action form layouts.
 */

/**
 * Layout Definitions.
 *
 * Called from sba_social_action_form_layouts_info().
 *
 * @return array
 */
function sba_social_action_form_layouts() {
  $templates = array();

  $templates['social_two_column_body_left'] = array(
    'name' => t('Call to action left, message and user form right.'),
    'types' => array('sba_social_action'),
    'theme' => 'social_two_column_body_left',
    'pattern' => 'social_two_column_body_left\'0-9\'+',
    'template' => 'two-column-body-left',
    'path' => drupal_get_path('module', 'sba_social_action') . '/templates/form_layouts',
    'hide title' => FALSE,
  );

  $templates['social_two_column_body_right'] = array(
    'name' => t('Call to action right, message and user form left'),
    'types' => array('sba_social_action'),
    'theme' => 'social_two_column_body_right',
    'pattern' => 'social_two_column_body_right\'0-9\'+',
    'template' => 'two-column-body-right',
    'path' => drupal_get_path('module', 'sba_social_action') . '/templates/form_layouts',
    'hide title' => FALSE,
  );
  return $templates;
}


/**
 * Implements hook_preprocess_hook().
 */
function sba_social_action_preprocess_social_two_column_body_left(&$vars) {
  sba_social_action_build_preprocess($vars);
}

/**
 * Implements hook_preprocess_hook().
 */
function sba_social_action_preprocess_social_two_column_body_right(&$vars) {
  sba_social_action_build_preprocess($vars);
}


/**
 * Helper function for form layouts preprocessors.
 */
function sba_social_action_build_preprocess(&$vars) {
  $vars['quicksign'] = '';
  if (module_exists('sba_quicksign') && sba_quicksign_is_enabled($vars['element']['#node'])) {
    $quicksign = drupal_get_form('sba_quicksign_form');
    $vars['quicksign'] = drupal_render($quicksign);
  }

//  $vars['participants'] = '';
//  $view = views_get_view('recent_action_participants');
//  $view->set_display('block_1');
//  $view->set_arguments(array(arg(1)));
//  $view->pre_execute();
//  $view->execute();
//  if (isset($view->result) && count($view->result)) {
//    $table = $view->preview('block_1');
//    $vars['participants'] = $table;
//  }

  $element = $vars['element'];

  // Override core's theme_form in order to remove <form> tag
  // and use it in a template variable.
  // see: theme_social_action_form_layout_form
  if (isset($element['#action'])) {
    $element['#attributes']['action'] = drupal_strip_dangerous_protocols($element['#action']);
  }
  element_set_attributes($element, array('method', 'id'));
  if (empty($element['#attributes']['accept-charset'])) {
    $element['#attributes']['accept-charset'] = "UTF-8";
  }
  $vars['open_form_tag']  = '<form' . drupal_attributes($element['#attributes']) . '><div>';
  $vars['close_form_tag'] = '</div></form>';

  $node = $element['#node'];
  $vars['webform'] = drupal_render($element['submitted']);
  $vars['message_fieldset'] = drupal_render($element['sba_messages']);
  $vars['form'] = $element;
  $vars['social'] = '';
  // Insert social icons.
  if (module_exists('sb_social')) {
    $social_enabled = in_array($node->type, array_filter(variable_get('springboard_social_enabled_content_types', array())));
    $social_configured = variable_get('springboard_social_addthis_profile_id', FALSE);
    if ($social_enabled && !empty($social_configured)) {
      $vars['social'] = token_replace('[sb_social:share_links]', array('node' => $node));
    }
  }

  // Fieldset legends.
  $vars['form_legend'] = t('Take Action Now!');
  $vars['user_legend'] = t('Your Information');
  // We remove image field in the default node--sba-message-action.tpl.php
  // and put it into form layouts here.
  $image_field = field_get_items('node', $node, 'field_social_action_img');
  $image = field_view_value('node', $node, 'field_social_action_img', $image_field[0]);
  if (!empty($image['#item'])) {
    $vars['image'] = drupal_render($image);
  }

  // We remove call to action field in the default
  // node--sba-message-action.tpl.php
  // and put it into form layouts here.
  $call_to_action = field_get_items('node', $node, 'field_message_call_to_action');
  $call = field_view_value('node', $node, 'field_message_call_to_action', $call_to_action[0]);
  $vars['call'] = drupal_render($call);

  // We remove call to action field in the
  // default node--sba-message-action.tpl.php
  // and put it into form layouts here.
  $body_field = field_get_items('node', $node, 'body');
  $body = field_view_value('node', $node, 'body', $body_field[0]);
  $vars['body'] = drupal_render($body);

  // Insert webform goals.
  if (module_exists('webform_goals')) {
    $goals = webform_goal_load_multiple($node->nid);
    $goal = array_pop($goals);
    // Get the most recently enabled goal.
    if (!empty($goal)) {
      $vars['goal'] = '<div class="wg-goal" style="display: none;" data-gid="' . $goal['gid'] . '"></div>';
    }
  }
}


/**
 * Override core's theme_form in order to remove <form> tag.
 *
 * And use it in a template variable defined in
 * sba_social_action_build_preprocess.
 *
 * @param array $vars
 *   The template vars.
 *
 * @return mixed
 *   The form without the form tags.
 */
function theme_social_action_form_layout_form(array $vars) {
  $element = $vars['element'];

  // core's version:
  //  if (isset($element['#action'])) {
  //    $element['#attributes']['action'] = drupal_strip_dangerous_protocols($element['#action']);
  //  }
  //  element_set_attributes($element, array('method', 'id'));
  //  if (empty($element['#attributes']['accept-charset'])) {
  //    $element['#attributes']['accept-charset'] = "UTF-8";
  //  }
  //return '<form' . drupal_attributes($element['#attributes']) . '><div>' . $element['#children'] . '</div></form>';

  return $element['#children'];
}

/**
 * Theme wrapper to create "Your Information" fieldset.
 *
 * And fluid bootstrap divs on non-form-layout nodes.
 *
 * @param array $vars
 *
 * @return string
 */
function theme_social_action_submitted_wrapper($vars) {
  return '<div class="row-fluid"><fieldset id="your-information-wrapper" class = "form-wrapper">' . $vars['element']['#children']  . '</fieldset></div>';
}

function theme_social_action_messages_wrapper($vars) {
  return '<div id = "right" class = "span7">' . $vars['element']['#children']  . '</div>';
}
/**
 * Theme wrapper to create "Take Action Now" fieldset
 * and fluid bootstrap divs on non-form-layout nodes.
 *
 * @param $vars
 * @return string
 */
function theme_social_action_webform_wrapper($vars) {
  $legend = t('Find Social Target');
  return ' <div class="row-fluid"><fieldset id="take-action-wrapper" class = "form-wrapper"><legend>' . $legend . '</legend>' . $vars['element']['#children']  . '</fieldset></div>';
}

