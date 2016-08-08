<?php
/**
 * @file
 * Various functions for the Payment Method field.
 */

/**
 * Callback to update a field with fundraiser specific settings during creation.
 */
function _fundraiser_payment_method_field_create($fundraiser, $field) {
  // If donation amounts don't exist, use amount as is.
  if (empty($fundraiser->gateways)) {
    // Leave it alone, do nothing else, don't overwrite existing values.
    $field['#allow_update'] = 0;
    return $field;
  }
  // Set all the fields for the gateways based on the configuration.
  $field['#extra'] = array(
    'payment_methods' => unserialize($fundraiser->gateways),
  );
  return $field;
}
