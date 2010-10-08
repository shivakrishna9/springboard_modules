<?php
// $Id$

/**
 * @file
 * Includes code for updating billing information for recurring donations.
 *
 * Each gateway will require it's own function named GATEWAY_ID_update_billing_information
 * that accepts a payment reference id and billing information.
 *
 * The individual billing update function should return a boolean indicating success or failure
 */

/** 
 * Generic function that is called by the fundraiser module.
 */
function fundraiser_update_billing_info($gateway_id, $reference_id, $billing_info) {
  $function = $gateway_id . '_update_billing_information';
  return $function($reference_id, $billing_info);
}
 
/** 
 * Authorize.net billing information update function.
 */
function authorizenet_update_billing_information($reference_id, $billing_info) {
  if (module_exists('authorizenet_cim')) {
    return authorizenet_cim_update($reference_id, $billing_info);
  }
  else {
    drupal_set_message(t('Unable to update billing information at this time'));
    watchdog('fundraiser', t('Billing information for recurring donations cannot be updated because the authorize_cim module is missing.'), NULL, WATCHDOG_CRITICAL);
    return null;    
  }
}

/** 
 * Sage billing information update function.
 */
function sage_update_billing_information($reference_id, $billing_info) {
  print 'sage vault specific code will go here';
}

