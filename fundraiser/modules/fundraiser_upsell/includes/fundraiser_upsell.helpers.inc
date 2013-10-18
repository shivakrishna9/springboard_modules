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
  $upsell_content = (!empty($node->upsell['content'])) ? $node->upsell['content'] : variable_get('fundraiser_upsell_default_content', '');
  $upsell_disclaimer = variable_get('fundraiser_upsell_default_content_disclaimer', '');
  // Build the output
  $content = '<div id="message-wrapper" class="hidden">';
  $content .= '<div id="message-modal" class="live-modal ' . $upsell_class;
  $content .= '" style="' . $upsell_size . '">';
  $content .= '<div class="message-content">' . $upsell_content . '</div>';
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
  $upsell_content = (!empty($node->upsell['content'])) ? $node->upsell['content'] : variable_get('fundraiser_upsell_default_content', '');
  $upsell_disclaimer = variable_get('fundraiser_upsell_default_content_disclaimer', '');
  $upsell_thankyou = variable_get('fundraiser_upsell_thank_you_content', 'Thank you for your recurring donation!');
  // Build the output
  $content = '<div id="upsell-preview">This is a preview of the upsell and thank you content for this form.</div>';
  $content .= '<div id="message-wrapper" class="hidden">';
  $content .= '<div id="message-modal" class="preview-modal ' . $upsell_class;
  $content .= '" style="' . $upsell_size . '">';
  $content .= '<div class="message-content">' . $upsell_content . '</div>';
  $content .= drupal_get_form('fundraiser_upsell_donation_form', 0);
  $content .= '<div id="message-disclaimer">' . $upsell_disclaimer . '</div>';
  $content .= '</div></div>';
  // Add the thank you message for the preview
  $content .= '<div id="message-return" class="hidden preview-modal thank-you ' . $upsell_class;
  $content .= '" style="' . $upsell_size . '"><div class="message-content">' . $upsell_thankyou . '</div></div>';

  return $content;
}

/**
 * Process the upsell donaton form from the POSTed ajax callback
 */
function _fundraiser_upsell_ajax_process($node) {
  $data = $_POST;
  // Check the form token to be sure this is a valid callback
  if (!drupal_valid_token($data['form_token'], $data['form_id'], TRUE)) {
    watchdog('fundraiser_upsell', 'Upsell order %oid failed because of an invalid form token.', array('%oid' => $order->order_id), WATCHDOG_NOTICE);
    $content = t('There was a problem with your order. Please try again');
  }
  else {
    $content = fundraiser_upsell_upsell_ajax_submit($data);
  }
  return drupal_json($content);
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
 * Much of this comes from fundraiser_create_future_orders()
 *
 * @param object $order
 *
 * @return object $new_order
 */
function _fundraiser_upsell_clone_order($order, $options = NULL) {
  // Create new order.
  $new_order = uc_order_new($order->uid);
  // Copy all values to new order.
  $new_id = $new_order->order_id;
  $new_order = clone $order;
  $new_order->order_id = $new_id;
  // Set the proper status.
  $status = (isset($options['status'])) ? $options['status'] : $order->order_status;
  $new_order->order_status = $status;
  // Get the amount and set it.
  $amount = (isset($options['amount'])) ? $options['amount'] : $order->order_total;
  $new_order->products[0]->price = $amount;
  $new_order->order_total = $amount;
  // Remove the response from the original order.
  if (!empty($new_order->data['response'])) {
    unset($new_order->data['response']);
  }
  // Remove the old product id and save.
  unset($new_order->products[0]->order_product_id);
  uc_order_save($new_order);
  // Add a comment to the order stating when it will be charged.
  $next_charge = strtotime("+1 months");
  uc_order_comment_save($new_id, 0, t('Payment will be processed on !date.', array('!date' => date('n/j/Y', $next_charge))), 'admin');
  // Add the new order to the fundraiser_recurring table so it will be processed in the future
  $sustainer_key = fundraiser_get_sustainer_key_value();
  $order_id = $new_id;
  $gateway = $order->data['gateway'];
  $nid = $order->products[0]->nid;
  $sid = ($_GET['sid']) ? $_GET['sid'] : 0;
  $donation_form_url = _fundraiser_donation_form_path($nid);
  db_query("INSERT INTO {fundraiser_recurring} (master_order_id, order_id, next_charge, gateway, form_url, sustainer_key) VALUES (%d, %d, '%s', '%s', '%s', '%s')",
    $order_id, $new_id, $next_charge, $gateway, $donation_form_url, $sustainer_key);
  // Now add it to the fundraiser_webform_order table
  db_query("INSERT INTO {fundraiser_webform_order} (webform_nid, order_id, sid, recurring_status, user_agent, form_url) VALUES (%d, %d, %d, %d, '%s', '%s')",
    $nid, $order_id, $form_state['values']['details']['sid'], $order->data['recurring_status'], $_SERVER['HTTP_USER_AGENT'], $donation_form_url);

  return $new_order;
}

/**
 * Check to see if a donation was successfully added as a recurring.
 *
 * @param int $did
 *   The order/donation id
 *
 * @return bool
 */
function _fundraiser_upsell_check_recurring($did) {
  $is_recurring = db_result(db_query('
    SELECT count(*)
    FROM {fundraiser_recurring}
    WHERE master_order_id = %d', $did));
  return ($is_recurring > 0) ? TRUE : FALSE;
}
