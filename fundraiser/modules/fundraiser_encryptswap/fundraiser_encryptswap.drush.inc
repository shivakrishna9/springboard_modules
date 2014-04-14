<?php

/**
 * Implements hook_drush_command().
 */
function fundraiser_encryptswap_drush_command() {
  $items['fundraiser-encryptswap'] = array(
    'description' => 'Swap encrypted donation data from one key to another.',
    'callback' => 'fundraiser_encryptswap_do',
    'arguments' => array(
      'order-id' => 'The order ID to swap keys. Use "all" to swap keys on all Ubercart orders.',
    ),
    'options' => array(
      'old-key' => 'The old key. If blank, uses the current contents of the key file.',
      'new-key' => 'The new key. If blank, uses the current contents of the key file.',
      'pretend' => 'Pretend mode. Do not save changes to database.',
    ),
  );
  return $items;
}

/**
 * Command callback.
 */
function fundraiser_encryptswap_do($order_id = NULL) {
  $orders = array();
  if ($order_id == 'all') {
    // Fetch all order IDs.
    $result = db_query("SELECT order_id FROM uc_orders ORDER BY order_id ASC");
    while ($order = db_result($result)) {
      $orders[] = $order;
    }
  }
  elseif (!empty($order_id)) {
    // Use the order ID passed in as an argument.
    $orders = array($order_id);
  }

  // Call the process function for each order.
  foreach ($orders as $this_order_id) {
    $result = fundraiser_encryptswap_do_order($this_order_id);
  }
}

/**
 * Helper function to switch the encryption keys for a given Ubercart order ID.
 *
 * @param $order_id
 *   ID for the Ubercart order.
 *
 * @return
 *   FALSE if error was encountered.
 */
function fundraiser_encryptswap_do_order($order_id) {
  // Load the Ubercart order object.
  if (!empty($order_id)) {
    $order = uc_order_load($order_id);
  }
  if (empty($order)) {
    drush_log(dt('Could not load order @order_id.', array('@order_id' => $order_id)), 'error');
    return FALSE;
  }

  //
  // ** DECRYPT **
  //
  $decrypt = new uc_encryption_class;

  // The $order object should already have cc_data decrypted using the default
  // key, but we need to reload this as encrypted text so we can use the
  // old-key parameter.
  $order->data = unserialize(db_result(db_query("SELECT data FROM {uc_orders} WHERE order_id = %d", $order_id)));
  if (empty($order->data)) {
    drush_log(dt('Could not load order data for order @order_id.', array('@order_id' => $order_id)), 'error');
    return FALSE;
  }

  // Get the old encrypt key. Allow for a new key value to be passed in as an
  // argument, but fall back on default key.
  $old_key = drush_get_option('old-key', uc_credit_encryption_key());

  // Decrypt using the old key.
  $decrypted_string = $decrypt->decrypt($old_key, $order->data['cc_data']);
  $decrypted_data = @unserialize($decrypted_string);
  if (empty($decrypted_data)) {
    drush_log(dt('Could not decrypt or unserialize the data on order @order_id.', array('@order_id' => $order->order_id)), 'error');
    return FALSE;
  }

  //
  // ** ENCRYPT **
  //
  $encrypt = new uc_encryption_class;

  // Get the new encrypt key. Allow for a new key value to be passed in as an
  // argument, but fall back on default key.
  $new_key = drush_get_option('new-key', uc_credit_encryption_key());

  // Encrypt using the new key.
  $encrypted_string = $encrypt->encrypt($new_key, serialize($decrypted_data));
  if (!empty($encrypt->errors)) {
    drush_log(dt('Could not encrypt or serialize the data on order @order_id.', array('@order_id' => $order->order_id)), 'error');
    return FALSE;
  }

  // Add the encrypted data to the order.
  if (!empty($encrypted_string)) {
    $order->data['cc_data'] = $encrypted_string;
  }

  // Save changes to the order (unless we're in pretend mode).
  if (!drush_get_option('pretend', FALSE)) {
    db_query("UPDATE {uc_orders} SET data = '%s' WHERE order_id = %d", serialize($order->data), $order_id);
    if (db_affected_rows() > 0) {
      drush_log(dt('Order @order_id encryption updated with new key.', array('@order_id' => $order->order_id)), 'ok');
    }
    else {
      drush_log(dt('Order @order_id could not be saved.', array('@order_id' => $order->order_id)), 'error');
    }
  }
  else {
    drush_log(dt('Pretend mode, no changes made to order @order_id.', array('@order_id' => $order->order_id)), 'warning');
  }
}
