<?php
// $Id$

/**
 * @file
 * Cron related functions for the fundraiser module.
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
 * Kick off recurring donations for payment.
 */
function fundraiser_process_recurring_donations() {
  // And after all of that is done, provide a hook to allow for modules to respond.
  module_invoke_all('fundraiser_donation_recurring');
  $successes = 0;
  $fails = 0;
  $recurrings = _fundraiser_get_recurring();
  // Loop over the found orders
  foreach ($recurrings as $recurring) {
    // Recreate the donation to process from the given values.
    $donation['donation']->did = $recurring->order_id;
    fundraiser_donation_retrieve_future($donation);
    // The donation information should be at at $donation['donation'], ready to process.
    fundraiser_donation_process($donation);
    // The results should be at $donation['result'], ready to respond to the aftermath.
    $result = $donation['result'];
    if (isset($result['success']) && $result['success']) {
      fundraiser_donation_success_recurring($donation);
      // Record the success.
      $successes++;
    }
    else {
      fundraiser_donation_decline_recurring($donation);
      // Record the fail.
      $fails++;
    }
  }
  if ($successes > 0 || $fails > 0) {
    watchdog('fundraiser', '!successes recurring donations processed successfully; !fails failed.',
      array('!successes' => $successes, '!fails' => $fails));
  }
  // And after all of that is done, provide a hook to allow for modules to respond.
  module_invoke_all('fundraiser_donation_post_recurring');
}

/**
 * Donation processing, a donation needs to be retrieved from stored data prior to processing.
 */
function fundraiser_donation_retrieve_future(&$donation) {
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  module_invoke_all('fundraiser_donation_retrieve_future', $donation);
}

/**
 * Donation processing, a given donation was successful.
 */
function fundraiser_donation_success_recurring(&$donation) {
  // Send credit card notification email when only a single payment is left
  if (fundraiser_recurring_payments_remaining($order_info['master_order_id']) == 1) {
    fundraiser_send_cc_notification($order_info['master_order_id'], $order->uid);
  }
  // Allow other modules to respond to the success.
  module_invoke_all('fundraiser_donation_success_recurring', $donation);
}

/**
 * Donation processing, a given donation was successful.
 */
function fundraiser_donation_decline_recurring(&$donation) {
  // Update data internal to fundraiser.
  db_query('UPDATE {fundraiser_recurring} SET '.
    'gateway_resp = \'failed\', attempts = attempts + 1, '.
    'next_charge = unix_timestamp(timestampadd(DAY, 1, from_unixtime(next_charge))) '.
    'WHERE order_id = :order_id', array(':order_id' => $order_id)
  );
  // get the number of times this order has been attempted
  $attempt_count = db_query('SELECT attempts FROM {fundraiser_recurring} WHERE order_id = :order_id',
    array(':order_id' => $order->order_id))->fetchColumn();
  if ($attempt_count == 3) {
    // Final attempt, add debug alert.
    watchdog('fundraiser', 'Payment for recurring donation @id has failed 3 times. The donation will not be submitted for payment again. Gateway message: @message',
      array('@id' => $order->order_id, '@message' => $result['message']), WATCHDOG_DEBUG);
  }
  else {
    // Log this failure
    watchdog('fundraiser', 'Payment for recurring donation @id has failed @attempts times. It will be processed again in 1 day. Gateway message: @message',
      array('@id' => $order->order_id, '@attempts' => $attempt_count, '@message' => $result['message']), WATCHDOG_DEBUG);
  }
  // Allow other modules to respond to the success.
  module_invoke_all('fundraiser_donation_decline_recurring', $donation);
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
