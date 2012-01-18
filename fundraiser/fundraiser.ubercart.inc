<?php

/**
 * @file
 * Ubercart based hook implementations for the fundraiser module.
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

function fundraiser_fundraiser_gateway_info() {
  return _uc_payment_gateway_list('', TRUE);
// From Ubsercart http://api.lullabot.com/_uc_payment_gateway_list/7
// Is ultimately configured by http://api.lullabot.com/hook_uc_payment_gateway
// Returns in the format:
// $gateway = array('id', 'title', 'credit', 'credit_refund', 'etc');
}

function fundraiser_fundraiser_validate_card($cc_number){
  if(variable_get('uc_credit_validate_numbers', TRUE)) {
    return _uc_credit_valid_card_number($cc_number);
  }
  return FALSE;
}

function fundraiser_fundraiser_validate_card_expiration($cc_expiration_month, $cc_expiration_year){
  return _uc_credit_valid_card_expiration($cc_expiration_month, $cc_expiration_year);
}

function fundraiser_fundraiser_validate_card_cvv($cc_cvv){
  if (variable_get('uc_credit_cvv_enabled', TRUE)) {
    return _uc_credit_valid_cvv($cc_cvv);
  }
  return FALSE;
}

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

function fundraiser_fundraiser_get_credit_encryption_path() {
  return variable_get('uc_credit_encryption_path', t('Not configured, see below.'));
}

function fundraiser_fundraiser_get_user_country($profile_value, $uid) {
  return db_query('SELECT uc.country_id FROM {uc_countries} uc '.
    'INNER JOIN {profile_value} pv ON pv.value = uc.country_iso_code_2 AND pv.uid = :uid AND pv.fid = :fid',
    array(':uid' => $uid, ':fid' => $profile_value))->fetchColumn();
}

function fundraiser_fundraiser_get_user_zone($profile_value, $uid) {
  return db_query('SELECT uz.zone_id FROM {uc_zones} uz '.
    'INNER JOIN {profile_value} pv ON pv.value = uz.zone_code AND pv.uid = :uid AND pv.fid = :fid',
    array(':uid' => $uid, ':fid' => $profile_value))->fetchColumn();
}

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