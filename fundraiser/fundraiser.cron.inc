<?php
// $Id$

/**
 * @file
 * Cron related functions for hte fundraiser module.
 * This file is included only when cron or custom fundraiser cron are run.
 */

/**
 * Loads the recurring donation processor key.
 *
 * @return FALSE if no key is found or the key domain does not match the current domain.
 */
function fundraiser_processor_key_match() {
  static $key;
  if (!empty($key)) {
    return ($key == $_SERVER['HTTP_HOST']);
  }
  $dir = module_invoke_all('fundraiser_get_credit_encryption_path');
  $dir = $dir[0];
  if ($dir && $dir !== t('Not configured, see below.')) {
    $filename = rtrim($dir, '/\\') . '/sustainer.key';
    if (file_exists($filename)) {
      if (!$file = fopen($filename, 'r')) {
        return FALSE;
      }
      $key = fread($file, filesize($filename));
      if (trim($key) != trim($_SERVER['HTTP_HOST'])) {
        return FALSE;
      }
      fclose($file);
    }
    else {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }
  return TRUE;
}

/**
 * Submits recurring donations for payment
 */
function fundraiser_process_recurring_donations() {

  $successes = 0;
  $fails = 0;

  $results = db_query('SELECT * FROM {fundraiser_recurring} ' .
    'WHERE next_charge < :next_charge AND (gateway_resp IS NULL OR gateway_resp = \'failed\') AND attempts < 3',
    array(':next_charge' => time()));

  // Loop over the found orders
  foreach ($results as $result) {
    $order_info = (array) $result;
    $order_id = $order_info['order_id'];

    // Get the order data from our order handler (Ubercart).
    $order = module_invoke_all('fundraiser_get_order', $order_id);
    $order = $order[0];

    $total = $order->order_total;

    // Get data out of the given order.
    $ref_id = array_shift(array_keys($order->data['cc_txns']['references']));
    $data = array(
      'txn_type' => 'reference_txn',
      'ref_id' => $ref_id,
    );
    $charge_function = _fundraiser_get_charge_function($order_id, $order->products[0]->nid, $data);

    // Process the charge.
    if (empty($charge_function)) {
      watchdog('fundraiser', 'Cannot determine the payment gateway for order @id. Node id of donation form is @nid',
        array('@id' => $order->order_id, '@nid' => $order->products[0]->nid), WATCHDOG_CRITICAL);
    }
    else {
      // process the order
      $result = $charge_function($order_id, $total, $data);
      if ($result['success']) {
        $successes++;
        uc_payment_enter($order_id, 'fundraiser_recurring', $total, 0, NULL, 'Submitted for payment via cron run.');
        uc_order_comment_save($order_id, 0, t('Submitted for payment via cron run. Transaction Id: @txn_id', array('@txn_id' => $result['data']['txn_id'])), 'admin');
        db_query("UPDATE {uc_orders} SET order_status = 'payment_received' WHERE order_id = :order_id",array(':order_id' => $order_id));
        db_query("UPDATE {fundraiser_recurring} SET gateway_resp = 'success', txn_id = :txn_id WHERE order_id = :order_id", array(':txn_id' => $result['data']['txn_id'], ':order_id' => $order_id));

        // Send credit card notification email when only a single payment is left
        if (fundraiser_recurring_payments_remaining($order_info['master_order_id']) == 1) {
          fundraiser_send_cc_notification($order_info['master_order_id'], $order->uid);
        }

        // Check for Salesforce Fundraiser module and update order status
        if (module_exists('sf_donation')) {
          sf_queue_insert($order->order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
        }
      }
      else {
        $fails++;
        // Allow other modules to respond to the failure. Including Ubercart.
        module_invoke_all('fundraiser_order_decline', $order->order_id);

        // Update data internal to fundraiser.
        db_query('UPDATE {fundraiser_recurring} SET '.
          'gateway_resp = \'failed\', attempts = attempts + 1, next_charge = unix_timestamp(timestampadd(DAY, 1, from_unixtime(next_charge))) '.
          'WHERE order_id = :order_id', array(':order_id' => $order_id)
        );
        // get the number of times this order has been attempted
        $attempt_count = db_query('SELECT attempts FROM {fundraiser_recurring} WHERE order_id = :order_id',
          array(':order_id' => $order->order_id))->fetchColumn();
        if ($attempt_count == 3) {
          // Final attempt, add debug alert.
          watchdog('fundraiser', 'Payment for recurring donation @id has failed 3 times. The order will not be submitted for payment again. Gateway message: @message',
            array('@id' => $order->order_id, '@message' => $result['message']), WATCHDOG_DEBUG);
        }
        else {
          // Log this failure
          watchdog('fundraiser', 'Payment for recurring donation @id has failed @attempts times. It will be processed again in 1 day. Gateway message: @message',
            array('@id' => $order->order_id, '@attempts' => $attempt_count, '@message' => $result['message']), WATCHDOG_DEBUG);
        }

        // Fire sustainer payment failure in triggers. TODO - this needs some more thought put to it for a clean interface. -SeH
        module_invoke_all('fundraiser', 'sustainer_payment_declined', $order);
      }
    }
  }

  if ($successes > 0 || $fails > 0) {
    watchdog('fundraiser', '!successes recurring fees processed successfully; !fails failed.', array('!successes' => $successes, '!fails' => $fails));
  }
}

/**
 * Gets the gateway associated with a given order. First it looks to see if the gateway has been
 * stored with the individual order. If it hasn't it uses the gateway that was assigned to the
 * donation form where the order originated.
 */
function _fundraiser_get_charge_function($order_id, $nid, &$data) {
  // First, see if the gateway is stored with the order.
  $gateway = _fundraiser_get_fields_from_table('fundraiser_recurring', array('order_id' => $order_id), 'gateway');
  if (!$gateway || empty($gateway)) {
    // Nothing on the order? Get the gateway from the original donation form.
    $gateway = _fundraiser_get_fields_from_table('fundraiser', array('nid' => $nid), 'gateway');
  }
  if (module_exists('gateway_manager')) {
    $info = gateway_manager_gateway_info($gateway);
    $charge_function = $info['charge_function'];
    $data['config'] = $info['config'];
    return $charge_function;
  }
  else {
    $gateways = _fundraiser_get_gateway_info();
    // get the charge function for the given gateway
    foreach ($gateways as $k => $v) {
      if ($v['id'] == $gateway) {
        return $v['credit'];
      }
    }
  }
  return FALSE;
}

/**
 * Returns the number of unprocessed payments for a recurring donation series.
 */
function fundraiser_recurring_payments_remaining($master_order_id) {
  return db_query('SELECT count(order_id) FROM {fundraiser_recurring} WHERE master_order_id = :master_order_id AND gateway_resp IS NULL',
    array(':master_order_id' => $master_order_id))->fetchColumn();
}

/**
 * Kicks off an email to a sustainer to remind them their credit card is about to expire.
 */
function fundraiser_send_cc_notification($master_order_id, $uid) {
  $user = user_load($uid);
  // load the last remaining order so we can get some info for the email
  $order_id = db_query('SELECT order_id FROM {fundraiser_recurring} WHERE master_order_id = :master_order_id AND gateway_resp IS NULL',
    array(':master_order_id' => $master_order_id))->fetchColumn();
  // Get the order data from our order handler (Ubercart).
  $order = module_invoke_all('fundraiser_get_order', $order_id);
  $order = $order[0];

  // Set up some params that can be used in the email.
  $params['exp_month'] = $order->payment_details['cc_exp_month'];
  $params['exp_year'] = $order->payment_details['cc_exp_year'];
  $params['cc_last_4'] = substr($order->payment_details['cc_number'], -4);
  $params['cc_type'] = $order->payment_details['cc_type'];
  $params['amount'] = $order->order_total;
  $params['order_id'] = $order_id;
  $params['user'] = $user;
  $params['master_order_id'] = $master_order_id;

  watchdog('fundraiser', 'Credit card expiration email sent to @mail', array('@mail' => $user->mail), WATCHDOG_INFO);
  drupal_mail('fundraiser', 'fundraiser_cc_notification', $user->mail, user_preferred_language($user), $params);
}
