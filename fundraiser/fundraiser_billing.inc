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
  $vault_id = db_result(db_query("select vault_id from {uc_sage_vault} where guid = '%s'", $reference_id));
	$data = array(
		'billing_name' => $billing_info['first_name'] . ' ' . $billing_info['last_name'],
		'billing_street1' => $billing_info['address_1'],
		'billing_street2' => $billing_info['address_2'],
		'billing_city' => $billing_info['city'],
		'billing_zone' => $billing_info['state'],
		'billing_postal_code' => $billing_info['zipcode'],
		'billing_country' => $billing_info['country'],
		'cc_number' => $billing_info['card_num'],
		'cc_exp_month' => sprintf("%02d", $billing_info['card_exp_date']['month']),
		'cc_exp_year' => $billing_info['card_exp_date']['year'],
	);

  $response = uc_sage_vault_update($vault_id, $data);
  // TODO: Need to add some better returns to the uc_sage module
  return true;
}

