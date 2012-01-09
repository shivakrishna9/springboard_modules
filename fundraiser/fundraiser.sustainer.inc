<?php

/**
 * @file
 * Sustainer management functions for the fundraiser module.
 *
 *
 * @ingroup fundraiser
 */

/**
 * Menu callback for the my recurring payments tab of the user profile.
 */
function fundraiser_recurring_user_overview($uid) {
  // gather all information for the page and send it through a theme function
  $orders = _fundraiser_get_recurring_orders($uid);
  // grab the first order id so we can load up the page
  $order_ids = array_keys($orders);
  // redirect to the first found recurring order
  drupal_goto('user/' . $uid->uid . '/recurring_overview/' . $order_ids[0]);
}

/**
 * Loads a specific order for the my recurring payments tab.
 */
function fundraiser_recurring_user_edit_forms($uid, $order_id) {

  $base = drupal_get_path('module', 'fundraiser');
  drupal_add_js($base . '/sustainer.js');
  drupal_add_css($base . '/sustainer.css');
  // gather all information for the page and send it through a theme function
  $orders = _fundraiser_get_recurring_orders($uid);
  $order = _fundraiser_get_unprocessed_order($order_id);
  $nid = $order->products[0]->nid;

  $cancelled = _fundraiser_recurring_payment_cancelled($order_id);
  $billing_info = _fundraiser_get_billing_information($order_id, $order);
  $payment_info = _fundraiser_get_payment_information($order_id, $order);

  // get full payment schedule
  $payment_schedule = _fundraiser_recurring_payment_schedule($order_id);

  // get forms
  include_once('fundraiser.sustainer.forms.inc');
  $donation_amount_form = drupal_get_form('fundraiser_donation_amount_form', $order_id, $nid, $payment_info['donation_amount']);
  $billing_update_form = drupal_get_form('fundraiser_billing_update_form', $order_id, $order);
  $cancel_form = drupal_get_form('fundraiser_cancel_form', $order_id);

  $output = theme('sustainer_management_page', $order_id, $orders, $payment_info, $billing_info, $donation_amount_form, $billing_update_form, $cancel_form, $payment_schedule, $cancelled);
  return $output;
}

/**
 * Gets the recurring orders for a given user id.
 */
function _fundraiser_get_recurring_orders(&$user) {
  $orders = array();
  $result = db_query("SELECT f.order_id, f.recurring_status FROM {fundraiser_webform_order} f INNER JOIN {uc_orders} u ON u.order_id = f.order_id  WHERE recurring_status != %d AND u.uid = %d", DONATION_NON_RECURRING, $user->uid);
  while ($data = db_fetch_object($result)) {
    $created = _fundraiser_get_start_date($data->order_id);
    $next_charge = _fundraiser_get_next_charge_date($data->order_id);
    $last_charge = _fundraiser_get_last_charge_date($data->order_id);
    $amount = _fundraiser_get_current_donation_amount($data->order_id);
    $orders[$data->order_id] = array(
      'amount' => $amount,
      'start_date' => $created,
      'status' => $data->recurring_status,
      'next_charge_date' => $next_charge,
      'expire_date' => $last_charge,
    );
  }

  return $orders;
}

/**
 * Gets the payment information for a recurring donation.
 */
function _fundraiser_get_payment_information($order_id, &$order) {
  $payment_info = array(
    'orders_remaining' => _fundraiser_remaining_recurring_charge_count($order_id),
    'orders_processed' => _fundraiser_processed_recurring_charge_count($order_id),
    'donation_amount' => $order->order_total
  );
  return $payment_info;
}

/**
 * Gets the billing information for a recurring donation.
 */
function _fundraiser_get_billing_information($master_order_id, &$order) {
  $billing_info = array(
    'billing_first_name' => $order->billing_first_name,
    'billing_last_name' => $order->billing_last_name,
    'billing_street' => $order->billing_street1,
    'billing_street2' => $order->billing_street2,
    'billing_city' => $order->billing_city,
    'billing_state' => uc_get_zone_code($order->billing_zone),
    'billing_postal_code' => $order->billing_postal_code,
  );
  return $billing_info;
}


/**
 * Gets an unprocessed order in a recurring donation series.
 */
function _fundraiser_get_unprocessed_order($master_order_id) {
  $order_id = db_result(db_query("SELECT order_id FROM {fundraiser_recurring} WHERE master_order_id = %d AND (gateway_resp IS NULL OR gateway_resp = 'canceled') limit 1", $master_order_id));
  if ($order_id) {
    return uc_order_load($order_id);
  }
  return NULL;
}

/**
 * Counts the number of processed recurring orders.
 */
function _fundraiser_processed_recurring_charge_count($order_id) {
  return db_result(db_query("select count(order_id) from {fundraiser_recurring} where master_order_id = %d
    and gateway_resp = 'success'", $order_id));
}

/**
 * Counts the number of unprocessed orders remaining.
 */
function _fundraiser_remaining_recurring_charge_count($order_id) {
  return db_result(db_query("select count(order_id) from {fundraiser_recurring} where master_order_id = %d
    and (gateway_resp IS NULL or gateway_resp = 'failed')", $order_id));
}

/**
 * Returns the entire payment schedule for a recurring donation series.
 */
function _fundraiser_recurring_payment_schedule($master_order_id) {
  $orders = array();

  $result = db_query(
  "
    SELECT u.order_total as amount, f.next_charge as charge_date, f.gateway_resp as status
    FROM {fundraiser_recurring} f
    INNER JOIN {uc_orders} u ON u.order_id = f.order_id
    WHERE f.master_order_id = %d
    ORDER BY f.next_charge asc
  ",
  $master_order_id
  );

  while ($data = db_fetch_array($result)) {
    $orders[] = $data;
  }
  return $orders;
}

/**
 * Determines whether or not a recurring donation series is cancelled.
 */
function _fundraiser_recurring_payment_cancelled($order_id) {
  $status = db_result(db_query("SELECT recurring_status FROM {fundraiser_webform_order} WHERE order_id = %d", $order_id));
  if ($status == DONATION_CANCELLED) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Gets the current amount of a recurring donation. Gets an unprocessed order as the amount may have changed in the past.
 */
function _fundraiser_get_current_donation_amount($order_id) {
  // get amound from the next unprocessed recurring donation
  return db_result(db_query("SELECT u.order_total FROM {uc_orders} u INNER JOIN {fundraiser_recurring} f ON f.order_id = u.order_id WHERE (f.gateway_resp IS NULL OR f.gateway_resp = 'canceled') AND f.master_order_id = %d ORDER BY next_charge LIMIT 1", $order_id));
}

/**
 * Gets the next charge date of a recurring donation.
 */
function _fundraiser_get_next_charge_date($order_id) {
  return db_result(db_query("SELECT min(next_charge) FROM {fundraiser_recurring} WHERE master_order_id = %d", $order_id));
}

/**
 * Gets the last payment date of a recurring donation.
 */
function _fundraiser_get_last_charge_date($order_id) {
  return db_result(db_query("SELECT max(next_charge) FROM {fundraiser_recurring} WHERE master_order_id = %d", $order_id));
}

/**
 * Gets the start date of a recurring donation.
 */
function _fundraiser_get_start_date($order_id) {
  return db_result(db_query("SELECT created FROM {uc_orders} WHERE order_id = %d", $order_id));
}

/**
 * Override of Drupal core expand_date found in includes/form.inc
 */
function _fundraiser_expand_date($element) {
  // Default to current date
  if (empty($element['#value'])) {
    $element['#value'] = array('day' => format_date(time(), 'custom', 'j'),
                            'month' => format_date(time(), 'custom', 'n'),
                            'year' => format_date(time(), 'custom', 'Y'));
  }

  $element['#tree'] = TRUE;

  // Determine the order of day, month, year in the site's chosen date format.
  $format = variable_get('date_format_short', 'm/d/Y - H:i');
  $sort = array();
  $sort['day'] = max(strpos($format, 'd'), strpos($format, 'j'));
  $sort['month'] = max(strpos($format, 'm'), strpos($format, 'M'));
  $sort['year'] = strpos($format, 'Y');
  asort($sort);
  $order = array_keys($sort);

  $current_year = date('Y');

  // Output multi-selector for date.
  foreach ($order as $type) {
    switch ($type) {
      case 'day':
        $options = drupal_map_assoc(range(1, 31));
        break;
      case 'month':
        $options = drupal_map_assoc(range(1, 12), 'map_month');
        break;
      case 'year':
        $options = drupal_map_assoc(range($current_year, $current_year + 10));
        break;
    }
    $parents = $element['#parents'];
    $parents[] = $type;
    $element[$type] = array(
      '#type' => 'select',
      '#value' => $element['#value'][$type],
      '#attributes' => $element['#attributes'],
      '#options' => $options,
    );
  }

  return $element;
}
ment;
}