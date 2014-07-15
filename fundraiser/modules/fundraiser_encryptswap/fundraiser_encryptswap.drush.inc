<?php

/**
 * Implements hook_drush_command().
 */
function fundraiser_encryptswap_drush_command() {
  $items['fundraiser-encryptswap'] = array(
    'description' => 'Swap encrypted donation data from one key to another.',
    'callback' => 'fundraiser_encryptswap_do',
    'arguments' => array(
      'order-id' => 'The order ID to swap keys. Use "all" to swap keys on all commerce orders.',
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
    $orders = db_query("SELECT order_id FROM commerce_order ORDER BY order_id ASC")->fetchCol();
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
 * Helper function to switch the encryption keys for a given Commerce order ID.
 *
 * @param $order_id
 *   Entity ID for the Commerce order.
 *
 * @return
 *   FALSE if error was encountered.
 */
function fundraiser_encryptswap_do_order($order_id) {
  encrypt_initialize();
  $methods = encrypt_get_methods('full');

  // Load the commerce order object. Reset the static cache.
  if (!empty($order_id)) {
    $orders = commerce_order_load_multiple(array($order_id), array(), TRUE);
  }
  if (empty($orders)) {
    drush_log(dt('Could not load order @order_id.', array('@order_id' => $order_id)), 'error');
    return FALSE;
  }
  $order = reset($orders);
  if (empty($order)) {
    drush_log(dt('Could not load order @order_id.', array('@order_id' => $order_id)), 'error');
    return FALSE;
  }

  //
  // ** DECRYPT **
  //

  // Grab the encrypted fundraiser_commerce array.
  $serialized_data = unserialize($order->data['fundraiser_commerce']);

  // Extract the (unencrypted) encrypt metadata.
  $old_method = $serialized_data['method'];
  $old_key_name = $serialized_data['key_name'];
  $old_key_array = encrypt_get_key($old_key_name);

  // The encrypted text.
  $original_encrypted_string = $serialized_data['text'];

  // Get the old encrypt key. Use the same key name, but allow for a new key
  // value to be passed in as an argument.
  $old_key = drush_get_option('old-key', $old_key_array['key']);

  // Decrypt using the old key.
  $options = array(
    'key' => $old_key,
  );
  $decrypted_string = call_user_func($methods[$old_method]['callback'], 'decrypt', $original_encrypted_string, $options);
  $decrypted_data = @unserialize($decrypted_string);
  if (empty($decrypted_data)) {
    drush_log(dt('Could not decrypt or unserialize the data on order @order_id.', array('@order_id' => $order->order_id)), 'error');
    return FALSE;
  }

  //
  // ** ENCRYPT **
  //

  // Get the new encrypt key. Use the same key name, but allow for a new key
  // value to be passed in as an argument.
  $new_key = drush_get_option('new-key', $old_key_array['key']);

  // Encrypt using the new key.
  $options = array(
    'key' => $new_key,
  );
  $encrypted_string = call_user_func($methods[$old_method]['callback'], 'encrypt', $decrypted_string, $options);

  // Add Encrypt module metadata to encrypted string.
  if (!empty($encrypted_string)) {
    $encrypted_data = array(
      'text' => $encrypted_string,
      'method' => $old_method,
      'key_name' => $old_key_name,
    );
    $encrypted_data = serialize($encrypted_data);

    if ($encrypted_data === FALSE) {
      drush_log(dt('Could not encrypt or serialize the data on order @order_id.', array('@order_id' => $order->order_id)), 'error');
      return FALSE;
    }

    // Serialize, then add the encrypted data to the order.
    if (!empty($encrypted_data)) {
      $order->data['fundraiser_commerce'] = $encrypted_data;
    }
  }

  // Save changes to the order (unless we're in pretend mode).
  if (!drush_get_option('pretend', FALSE)) {
    $result = commerce_order_save($order);
    if ($result === SAVED_NEW || $result === SAVED_UPDATED) {
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
