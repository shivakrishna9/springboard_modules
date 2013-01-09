<?php

/**
 * @file
 * Commerce based hook for commerce_payment_example.
 */

/**
 * Implements hook_fundraiser_commerce_execute_gateway().
 */
function commerce_payment_example_fundraiser_commerce_execute_gateway($method_instance, $donation) {
  $result = array(
    'success' => FALSE,
    'message' => 'This payment failed during gateway execution.'
  );
  // Translate from donation settings to the correct values for the plugin.
  // And execute the false form submit to make the charge.
  // If a user is created in this process, make sure to map the order ownership.
  // If a transaction, make sure to map those values into result array.
  // And update the result array success field.
  // Return the result array.
  return $result;
}