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
  // Get the content for the form.
  $upsell_class = (!empty($node->upsell['class'])) ? $node->upsell['class'] : '';
  $upsell_header = (!empty($node->upsell['header'])) ? $node->upsell['header'] : variable_get('fundraiser_upsell_default_content', '');
  $upsell_disclaimer = variable_get('fundraiser_upsell_default_content_disclaimer', '');
  // Build the output
  $content = '<div id="message-wrapper" class="hidden">';
  $content .= '<div id="message-modal" class="live-modal ' . $upsell_class;
  $content .= '" style="' . $upsell_size . '">';
  $content .= $upsell_header;
  $content .= drupal_get_form('fundraiser_upsell_donation_form', $sid);
  $content .= '<div id="message-disclaimer">' . $upsell_disclaimer . '</div>';
  $content .= '</div></div>';

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
  // Add the css & js
  $path = drupal_get_path('module', 'fundraiser_upsell');
  drupal_add_css($path . '/css/fundraiser_upsell.css');
  drupal_add_js($path . '/js/fundraiser_upsell.js');
  drupal_add_js($path . '/js/jquery.blockUI.js');
  // Get the size and class for the modal
  $width = $node->upsell['modal_width'];
  $height = $node->upsell['modal_height'];
  $upsell_size = 'width:' . $width . 'px;height:' . $height . 'px;';
  // Get the content for the form.
  $upsell_class = (!empty($node->upsell['class'])) ? $node->upsell['class'] : '';
  $upsell_header = (!empty($node->upsell['header'])) ? $node->upsell['header'] : variable_get('fundraiser_upsell_default_content', '');
  $upsell_disclaimer = variable_get('fundraiser_upsell_default_content_disclaimer', '');
  $upsell_thankyou = variable_get('fundraiser_upsell_thank_you_content', 'Thank you for your recurring donation!');
  // Build the output
  $content = '<div id="upsell-preview">This is a preview of the upsell and thank you content for this form.</div>';
  $content .= '<div id="message-wrapper" class="hidden">';
  $content .= '<div id="message-modal" class="preview-modal ' . $upsell_class;
  $content .= '" style="' . $upsell_size . '">';
  $content .= $upsell_header;
  $content .= drupal_get_form('fundraiser_upsell_donation_form', 0);
  $content .= '<div id="message-disclaimer">' . $upsell_disclaimer . '</div>';
  $content .= '</div></div>';
  // Add the thank you message for the preview
  $content .= '<div id="message-return" class="hidden">' . $upsell_thankyou . '</div>';

  return $content;
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
  $donation = db_fetch_array(db_query("
    SELECT *
    FROM {fundraiser_webform_order} AS fwo, {fundraiser_upsell} AS fu
    WHERE fwo.sid = %d
    AND fwo.webform_nid = fu.nid", $sid));
  $order = uc_order_load($donation['order_id']);
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
function _fundraiser_upsell_clone_order($order, $options = NULL) {
  $donation = fundraiser_donation_get_donation($order->order_id);
  $new_order = $order;
  // Modify the order
  $new_order->log = "Cloned from order# $order->order_id";
  $new_order->changed = '';
  $new_order->revision_id = NULL;
  unset($new_order->revision_uid);
  unset($new_order->revision_timestamp);
  unset($new_order->revision_hostname);
  $new_order->order_id = NULL;
  $new_order->order_number = NULL;
  $new_order->in_new = TRUE;
  // Change the value
  if (isset($options['amount'])) {
    $new_order->status = $options['status'];
    $new_order->commerce_order_total['und'][0]['amount'] = $options['amount'];
    $new_order->commerce_order_total['und'][0]['data']['components'][0]['price']['amount'] = $options['amount'];
  }
  else {
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
    'uid' => $donation->uid, // Submission user id.
    'sid' => $donation->sid, // Submission id.
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

/**
 * Check to see if a donation was successfully added as a recurring.
 *
 * @param int $did
 *   The order/donaton id
 *
 * @return bool
 */
function _fundraiser_upsell_check_recurring($did) {
  $is_recurring = db_query('
    SELECT count(*)
    FROM {fundraiser_sustainers}
    WHERE master_did = :mid', array(':mid' => $did))->fetchField();
  return ($is_recurring > 0) ? TRUE : FALSE;
}
