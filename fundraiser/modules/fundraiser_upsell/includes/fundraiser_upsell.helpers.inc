<?php
/**
 * @file
 * fundraiser_upsell.helpers.inc - Helper & utility functions for fundraiser_upsell
 */

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
  $width = $node->upsell['modal_width'];
  $height = $node->upsell['modal_height'];
  $upsell_size = 'width:' . $width . 'px;height:' . $height . 'px;';
  (!empty($node->upsell['class'])) ? $upsell_class = $node->upsell['class'] : $upsell_class = '';
  $upsell_class = drupal_html_class($upsell_class);
  // Get the upsell content data
  if (!empty($node->upsell['content'])) {
    $upsell_header = $node->upsell['content'];
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
 * Process the upsell donaton form from the POSTed ajax callback
 */
function _fundraiser_upsell_ajax_process($node) {
  $data = $_POST;
  // Check the form token to be sure this is a valid callback
  if (!drupal_valid_token($data['form_token'], $data['form_id'])) {
    return drupal_json_output(false);
  }
  $content = fundraiser_upsell_upsell_ajax_submit($data);
  //$content = array ('content' => 'Thank you for your order');
  return drupal_json_output($content);
}

/**
 * Determine if we should display the upsell preview for a node
 * @param object $node
 */
function _fundraiser_upsell_preview_check($node) {
  // Only check nodes that are donation types with upsell enabled
  if (!empty($node) && fundraiser_is_donation_type($node->type) && $node->upsell['enabled']) {
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
  $donation = db_query("
    SELECT *
    FROM {fundraiser_donation} AS fd, {fundraiser_upsell} AS fu
    WHERE fd.sid = :sid
    AND fd.nid = fu.nid",
    array(':sid' => $sid)
    )->fetchAssoc();
  $order = commerce_order_load($donation['did']);
  // Add the donation & upsell info
  $order->upsell = $donation;
  return $order;
}

/**
 * Determine if the node has upsell enabled
 */
function _fundraiser_upsell_enabled($node) {
  if (!empty($node->upsell['enabled']) && $node->upsell['enabled']) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Get the suggested amount from the admin bracket settings
 * @param integer $amount
 */
function _fundraiser_upsell_find_suggested_amount($amount) {
  $brackets = variable_get("fundraiser_upsell_brackets", array());
  foreach ($brackets AS $i => $bracket) {
    // If the amount is within the brackets, then use this upsell setting
    if (($amount >= $bracket['low']) && ($amount <= $bracket['high'])) {
      $upsell = $bracket['upsell'];
    }
    // If this is the last loop and we don't have an upsell yet, set it for the highest bracket
    if ($i == count($brackets) - 1 && empty($upsell)) {
      $upsell = $bracket['upsell'];
    }
  }
  return $upsell;
}

/**
 * If we need to decouple the original order, clone it to a new order_id and set the value to $0
 * @param object $order
 * @return object $new_order
 */
function _fundraiser_upsell_clone_order($order, $options = null) {
  $donation = fundraiser_donation_get_donation($order->order_id);
  $new_order = $order;
  // Modify the order
  $new_order->log = "Cloned from order# $order->order_id";
  // $new_order->created = '';
  $new_order->changed = '';
  $new_order->revision_id = NULL;
  unset($new_order->revision_uid);
  unset($new_order->revision_timestamp);
  unset($new_order->revision_hostname);
  $new_order->order_id = NULL;
  $new_order->order_number = NULL;
  $new_order->in_new = TRUE;
  if(isset($options['amount'])) {
    $new_order->status = $options['status'];
    $new_order->commerce_order_total['und'][0]['amount'] = $options['amount'];
    $new_order->commerce_order_total['und'][0]['data']['components'][0]['price']['amount'] = $options['amount'];
  } else {
    $new_order->commerce_order_total['und'][0]['amount'] = 0;
    $new_order->commerce_order_total['und'][0]['data']['components'][0]['price']['amount'] = 0;
  }
  // Create new order
  commerce_order_save($new_order);
  // Copy the new order to fundraiser
  //watchdog('fundraiser', 'Order <pre>@order</pre>',
  //  array('@order' => print_r($order,true)), WATCHDOG_DEBUG, NULL);
  //watchdog('fundraiser', 'Donation <pre>@donation</pre>',
  //  array('@donation' => print_r($donation,true)), WATCHDOG_DEBUG, NULL);
  $donation_record = array(
    'did' => $new_order->order_id, // Donation id.
    'nid' => $donation->nid, // Fundraiser node id.
    'uid' => $donation->uid,  // Submission user id.
    'sid' => $donation->sid,  // Submission id.
    'status' => $new_order->status,
    'amount' => $donation->amount,
    'currency' => $donation->currency,
    'gateway' => $donation->gateway['id'],
    'txn_id' => '',
    'form_url' => $donation->form_url,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
  );
  _fundraiser_create_donation($donation_record);
  return $new_order;
}
