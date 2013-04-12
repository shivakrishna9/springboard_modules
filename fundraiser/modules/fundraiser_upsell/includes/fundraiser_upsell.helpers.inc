<?php

/**
 * Generate the render array for the upsell form and thank you response
 * @param object $node
 * The webform donation node object
 * @param integer $sid
 * The user's specific sid for the donation node
 * @return array $output
 * A render array for generating the content
 */
function _fundraiser_upsell_content($node, $sid) {
  // Add the css & js
  $path = drupal_get_path('module', 'fundraiser_upsell');
  drupal_add_css($path . '/css/fundraiser_upsell.css');
  drupal_add_js($path . '/js/fundraiser_upsell.js');
  drupal_add_js($path . '/js/jquery.blockUI.js');
  // Get the size and class for the modal 
  $width = $node->modal_width;
  $height = $node->modal_height;
  $upsell_size = 'width:' . $width . 'px;height:' . $height . 'px;';
  (!empty($node->upsell_class)) ? $upsell_class = $node->upsell_class : $upsell_class = '';
  $upsell_class = drupal_html_class($upsell_class);
  // Get the upsell content data
  if (!empty($node->fundraiser_upsell_content)) {
    $upsell_header = $node->fundraiser_upsell_content;
  }
  else {
    $upsell_header = variable_get('fundraiser_upsell_default_content', '');
  }
  $upsell_disclaimer = variable_get('fundraiser_upsell_default_content_disclaimer', '');
  // Build the render array
  $content = array();
  $content['message_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('message-wrapper'),
      'class' => array('hidden'),
    ),
  );
  $content['message_wrapper']['message_modal'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('message-modal'),
      'class' => array('live', $upsell_class),
      'style' => array($upsell_size),
    ),
  );
  $content['message_wrapper']['message_modal']['form'] = drupal_get_form('fundraiser_upsell_donation_form', $sid);
  $content['message_wrapper']['message_modal']['form']['#prefix'] = $upsell_header;
  $content['message_wrapper']['message_modal']['form']['#suffix'] = $upsell_disclaimer;

  return $content;
}

/**
 * Generate the upsell and thank you content for preview
 * @param object $node
 * The webform donation node object
 * @return array $output
 * A render array for generating the content
 */
function _fundraiser_upsell_content_preview($node) {
  // Grab the render array and modify it  
  $content = _fundraiser_upsell_content($node, 0);
  $content['message_wrapper']['#prefix'] = 'This is a preview of the upsell and thank you content for this form.';
  $content['message_wrapper']['message_modal']['#attributes']['class'][0] = 'preview';
  // Add the thank you content for ease of display
  $content['message_wrapper']['message_return'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('message-return'),
    ),
  );  
  $content['message_wrapper']['message_return']['content'] = array(
    '#markup' => variable_get('fundraiser_upsell_thank_you_content', 'Thank you!'),
  );
  // Stop the preview form from actually working
  unset($content['message_wrapper']['message_modal']['form']['#submit']);
  unset($content['message_wrapper']['message_modal']['form']['#action']);
  // Render array and return it
  return drupal_render($content);
}

/**
 * Determine if we should display the upsell preview for a node
 * @param object $node
 */
function _fundraiser_upsell_preview_check($node) {
  // Only check nodes that are donation types with upsell enabled
  if (!empty($node) && fundraiser_is_donation_type($node->type) && $node->fundraiser_upsell_enabled) {
    // Make sure user has permissions
    if (user_access('administer fundraiser upsells')) {
      return TRUE;
    }
  }
  return FALSE;
}


/**
 * Check the cookies and settings to see if upsell should be displayed
 */
function _fundraiser_upsell_check_cookies() {
  $display = FALSE;
  $rejection = (!empty($_COOKIE['fundraiser_upsell_rejection'])) ? $_COOKIE['fundraiser_upsell_rejection'] : FALSE;
  $acceptance = (!empty($_COOKIE['fundraiser_upsell_acceptance'])) ? $_COOKIE['fundraiser_upsell_acceptance'] : FALSE;
  $debug = variable_get('fundraiser_upsell_debug', FALSE);
  // No cookies OR debug is on, then we should display the upsell
  if ((!$rejection && !$acceptance) || $debug) {
    $display = TRUE; 
  }
  return $display;
}

/**
 * Generate the order object from the sid
 * @param integer $sid
 * @param object $order
 */
function _fundraiser_upsell_get_order($sid=NULL) {
  // Return empty object if no sid
  if (empty($sid)) {
    return new stdClass();
  }
  // Otherwise look up and load the order object
  $order_id = db_query(
    "SELECT did FROM {fundraiser_donation} WHERE sid = :sid",
    array(':sid' => $sid)
    )->fetchField();
  // @TODO: make sure this works with DC too
  $order = uc_order_load($order_id);
  return $order;
}

/**
 * Get the suggested amount from the admin bracket settings
 * @param integer $amount
 */
function _fundraiser_upsell_find_suggested_amount($amount) {
  $brackets = variable_get("fundraiser_upsell_brackets", array());
  foreach ($brackets AS $i => $bracket) {
    if (($amount >= $bracket['low']) && ($amountt <= $bracket['high'])) {
      return $bracket['upsell'];
    }
  }
}

function _fundraiser_upsell_amount_less_than_highest_bracket($amt) {
  return $amt < _fundraiser_upsell_higest_bracket();
}

function _fundraiser_upsell_higest_bracket() {
  $brackets = variable_get("fundraiser_upsell_brackets", array());
  $highest = 0;
  foreach ($brackets as $bracket) {
    if ($bracket['high'] > $highest) {
      $highest = $bracket['high'];
    }
  }

  return $highest;
}