<?php

/**
 * @file
 * Ubercart based hook implementations for the fundraiser module.
 * These functions are split off with the idea that eventually this could become it's own module, linking fundraiser and ubercart.
 */

/**
 * Implementation of hook_order_actions() from Ubercart.
 * Add a refund option to the Order list
 * From http://drupalcontrib.org/api/drupal/contributions!ubercart!docs!hooks.php/function/hook_order_actions/6
 */
function fundraiser_order_actions($order) {
  if (user_access('edit orders') && (in_array($order->order_status, array('payment_received', 'partially_refunded')) || empty($order->order_status)) && fundraiser_gateway_can_refund_order($order)) {
    $module_path = base_path() . drupal_get_path('module', 'fundraiser');
    $title = t('Issue refund on order !order_id.', array('!order_id' => $order->order_id));
    $actions[] = array(
      'name' => t('Refund'),
      'url' => 'admin/store/orders/' . $order->order_id . '/refund',
      'icon' => '<img src="' . $module_path . '/images/refund.gif" alt="' . $title . '" />',
      'title' => $title,
    );
    return $actions;
  }
}

/**
 * Implementation of hook_order().
 * From http://www.ubercart.org/docs/api/hook_order
 * When an order is deleted, keep fundraiser tables in sync.
 */
function fundraiser_order($op, &$arg1, $arg2) {
  switch ($op) {
    case 'delete':
      db_delete('fundraiser_recurring')->condition('order_id', $arg1->order_id)->execute();
      break;
  }
}

/**
 * Implementation of hook_preprocess_uc_order().
 * From: http://www.ubercart.org/forum/development/18047/inserting_variables_invoices_ubercart_23_and_above
 * Adds additional variables for use in donation receipt emails sent by UC.
 */
function fundraiser_preprocess_uc_order(&$variables) {
  switch ($variables['op']) {
    case 'checkout-mail':
      $nid = $variables['order']->products[0]->nid;
      $message = db_query("SELECT receipt_email_message FROM {fundraiser} WHERE nid = :nid", array(':nid' => $nid))->fetchColumn();
      $variables['fundraiser_message'] = $message;
      break;
  }
}

/**
 * Implements hook_fundraiser_get_user_country().
 */
function fundraiser_fundraiser_get_user_country($profile_field_id, $uid) {
  return db_query('SELECT uc.country_id FROM {uc_countries} uc '.
    'INNER JOIN {profile_value} pv ON pv.value = uc.country_iso_code_2 AND pv.uid = :uid AND pv.fid = :fid',
    array(':uid' => $uid, ':fid' => $profile_field_id))->fetchColumn();
}

/**
 * Implements hook_fundraiser_get_user_zone().
 */
function fundraiser_fundraiser_get_user_zone($profile_field_id, $uid) {
  return db_query('SELECT uz.zone_id FROM {uc_zones} uz '.
    'INNER JOIN {profile_value} pv ON pv.value = uz.zone_code AND pv.uid = :uid AND pv.fid = :fid',
    array(':uid' => $uid, ':fid' => $profile_field_id))->fetchColumn();
}

/**
 * Implements hook_fundraiser_user_has_recurring_orders().
 */
function fundraiser_fundraiser_user_has_recurring_orders($uid) {
  $count = db_query('SELECT COUNT(u.order_id) FROM uc_orders u '.
    'INNER JOIN fundraiser_recurring f ON f.master_order_id = u.order_id '.
    'WHERE u.uid = :uid ',
    array(':uid' => $uid))->fetchColumn();
  if ($count > 0) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_fundraiser_get_country_zones().
 */
function fundraiser_fundraiser_get_country_zones($country_id) {
  $options = array('required' => uc_address_field_required('zone'));
  $zones = uc_zone_select(uc_get_field_name('zone'), NULL, $country_id, $options);
  $zone_array = array();
  foreach ($zones['#options'] as $key => $value) {
    $zone_array[$key] = $value;
  }
  return $zone_array;
}

/**
 * Implements hook_fundraiser_gateway_info().
 */
function fundraiser_fundraiser_gateway_info() {
  return _uc_payment_gateway_list('', TRUE);
// From Ubsercart http://api.lullabot.com/_uc_payment_gateway_list/7
// Is ultimately configured by http://api.lullabot.com/hook_uc_payment_gateway
// Returns in the format:
// $gateway = array('id', 'title', 'credit', 'credit_refund', 'etc');
}

/**
 * Implements hook_fundraiser_get_credit_encryption_path().
 */
function fundraiser_fundraiser_get_credit_encryption_path() {
  return variable_get('uc_credit_encryption_path', t('Not configured, see below.'));
}

/**
 * Implements hook_fundraiser_validate_card().
 */
function fundraiser_fundraiser_validate_card($cc_number){
  if(variable_get('uc_credit_validate_numbers', TRUE)) {
    return _uc_credit_valid_card_number($cc_number);
  }
  return FALSE;
}

/**
 * Implements hook_fundraiser_validate_card_expiration().
 */
function fundraiser_fundraiser_validate_card_expiration($cc_expiration_month, $cc_expiration_year){
  return _uc_credit_valid_card_expiration($cc_expiration_month, $cc_expiration_year);
}


/**
 * Implements hook_fundraiser_validate_card_cvv().
 */
function fundraiser_fundraiser_validate_card_cvv($cc_cvv){
  if (variable_get('uc_credit_cvv_enabled', TRUE)) {
    return _uc_credit_valid_cvv($cc_cvv);
  }
  return FALSE;
}

/**
 * Implements hook_fundraiser_create_order().
 */
function fundraiser_fundraiser_create_order($user, $fundraiser_fields, $recurs, $additional_order_fields) {
  // pick out the values we need to generate an order
  $donation_amount = $fundraiser_fields['amount'];
  $cc_number = $fundraiser_fields['card_number'];
  $cc_cvv = $fundraiser_fields['card_cvv'];
  $cc_expiration_month = $fundraiser_fields['card_expiration_month'];
  $cc_expiration_year = $fundraiser_fields['card_expiration_year'];
  $first_name = $fundraiser_fields['first_name'];
  $last_name = $fundraiser_fields['last_name'];
  $email = $fundraiser_fields['email'];
  $billing_address = $fundraiser_fields['address'];
  $billing_address_2 = $fundraiser_fields['address_line_2'];
  $billing_city = $fundraiser_fields['city'];
  $billing_country = $fundraiser_fields['country'];
  $billing_state = $fundraiser_fields['state'];
  $billing_zipcode = $fundraiser_fields['zip'];
  $quantity = $fundraiser_fields['quantity'];

  // Look for other amount if set.
  if ($donation_amount == "other") {
    $donation_amount = preg_replace("/[^\d\.]/i", "", $fundraiser_fields['other_amount']);
  }

  // Create an UC order.
  $order = uc_order_new($user->uid);
  $order->products[0] = uc_product_load($node);
  $order->products[0]->price = $donation_amount;
  $order->products[0]->qty = empty($quantity) ? 1 : $quantity;
  $order->products[0]->title = $node->title;
  $order->products[0]->nid = $node_id;
  $order->products[0]->data = array(
    'shippable' => $order->products[0]->shippable,
    'model' => $order->products[0]->model,
    'varprice' => $donation_amount,
    'module' => 'uc_product',
  );

  // Multiply amount by quantity if available.
  if (!empty($quantity)) {
    $donation_amount = $donation_amount * $quantity;
  }

  // Set order properties.
  $order->primary_email = $email;
  $order->order_total = $donation_amount;
  $order->billing_first_name = $first_name;
  $order->billing_last_name = $last_name;
  $order->billing_city = $billing_city;
  $order->billing_street1 = $billing_address;
  $order->billing_street2 = $billing_address_2;
  $order->billing_postal_code = $billing_zipcode;
  $order->billing_zone = $billing_state;
  $order->billing_country = $billing_country;
  $order->payment_method = 'credit';
  $order->payment_details = array(
    'cc_type' => _fundraiser_get_cc_type($cc_number),
    'cc_owner' => '',
    'cc_number' => $cc_number,
    'cc_start_month' => '',
    'cc_start_year' => '',
    'cc_exp_month' => $cc_expiration_month,
    'cc_exp_year' => $cc_expiration_year,
    'cc_issue' => '',
    'cc_cvv' => $cc_cvv,
    'cc_bank' => '',
  );
  $order->line_items = array();
  $order->data['recurring_status'] = $recurs;
  foreach ($additional_order_fields as $field => $value) {
    $order->data[$field] = $value;
  }

  // If using the gateway manager, store the gateway that was used to make the payment
  if (module_exists('gateway_manager')) {
    $order->data['gateway'] = $node->gateway;
  }

  // cache the cc details stored by the handler.
  uc_credit_cache('save', $order->payment_details, FALSE);
  // save the order
  uc_order_save($order);
  return $order->order_id;
}

/**
 * Implements hook_fundraiser_get_order().
 */
function fundraiser_fundraiser_get_order($order_id) {
  $order = uc_order_load($order_id);
  // Return the order information.
  return $order;
}

/**
 * Implements hook_fundraiser_order_success().
 */
function fundraiser_fundraiser_order_success($order_id) {
  // Complete the sale in Ubercart.
  $order = uc_order_load($order_id);
  uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
  uc_payment_enter($order_id, 'fundraiser', $order->order_total, 0, NULL, t('Payment processed by the fundraiser module.'));
  db_query("UPDATE {uc_orders} SET order_status = 'payment_received' WHERE order_id = :order_id", array(':order_id' => $order_id));
  // And done.
}

/**
 * Implements hook_fundraiser_create_future_orders().
 */
function fundraiser_fundraiser_create_future_orders($user, $order_id, $gateway) {
  $orders = array();
  // Determine the number of future donations (Salesforce hard limit is 50. Set as 49 because original order is 1)
  $counter = min(_fundraiser_months_between_dates($order->payment_details['cc_exp_month'], $order->payment_details['cc_exp_year']), 49);
  for ($i = 1; $i <= $counter; $i++) {
    $next_order = uc_order_new($user->uid);
    $new_id = $next_order->order_id;
    $next_charge = strtotime("+$i months");
    $orders[] = array(
      'order_id' => $order_id,
      'new_id' => $new_id,
      'next_charge' => $next_charge,
      'gateway' => $gateway,
    );

    // Copy all values to new order
    $next_order = clone $order;
    $next_order->order_id = $new_id;
    $next_order->order_status = 'pending_future_payment';
    unset($next_order->products[0]->order_product_id);
    uc_order_save($next_order);
    // Add a comment to the order stating when it will be charged.
    uc_order_comment_save($new_id, 0, t('Payment will be processed on !date.', array('!date' => date('n/j/Y', $next_charge))), 'admin');
  }
  // Return newly created order set to fundraiser.
  return $orders;
}

/**
 * Implements hook_fundraiser_order_decline().
 */
function fundraiser_fundraiser_order_decline($order_id) {
  // Mark the sale in Ubercart as failed.
  db_query("UPDATE {uc_orders} SET order_status = 'failed' WHERE order_id = :order_id", array(':order_id' => $order_id));
  // Clear the credit card cache between orders
  uc_credit_cache('clear');
}

/**
 * Implements hook_fundraiser_order_exception().
 */
function fundraiser_fundraiser_order_exception($order_id) {
  return fundraiser_fundraiser_order_decline($order_id);
}

/**
 * Implements hook_fundraiser_order_cancel().
 */
function fundraiser_fundraiser_order_cancel($order_id) {
  // Mark the sale in Ubercart as cencelled.
  db_query("UPDATE {uc_orders} SET order_status = 'canceled' WHERE order_id = :order_id", array(':order_id' => $order_id));
  // Make a comment on the order.
  uc_order_comment_save($order_id, 0, t('Payment was canceled during checkout by the user.'));
}