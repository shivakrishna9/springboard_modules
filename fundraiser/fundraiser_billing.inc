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


/**
 * Refund tab on Orders
 */
function fundraiser_refund(&$form_state, $order) {
  $form = array();
  $form['#order_id'] = $order->order_id;
  $form['explanation'] = array(
    '#value' => t('Note that refunds can only be applied to payments that have been settled in the payment processor. This typically happens every night; if the payment was made today, you will probably need to wait until tomorrow to issue a refund.'),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  
  $form['full_refund'] = array(
    '#type' => 'radio',
    '#title' => t('Full Refund'),
    '#return_value' => 'full',
    '#parents' => array('refund_type'),
  );
  $form['partial_refund'] = array(
    '#type' => 'radio',
    '#title' => t('Partial Refund'),
    '#return_value' => 'partial',
    '#parents' => array('refund_type'),
  );
  
  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount to Refund'),
    '#size' => 40,
    '#maxlength' => 255,
  );
  $form['refund_notes'] = array(
    '#type' => 'textarea',
    '#title' => t('Refund Notes'),
    '#description' => t('Please provide reference information about this refund (Requestor, reason, etc.)'),
    '#cols' => 60,
    '#rows' => 5,
  );
  $form['issue_refund'] = array(
    '#type' => 'submit',
    '#value' => t('Issue Refund'),
  );
  
  return $form;
}


/**
 * Validate the form
 */
function fundraiser_refund_validate($form, &$form_state) {
  if ($form_state['values']['refund_type'] == 'partial') {
    $amount = $form_state['values']['amount'];
    if (!($amount > 0)) {
      form_set_error('amount', t('Enter a refund amount.'));
    }
  }
}


/**
 * Submit the refund form
 */
function fundraiser_refund_submit($form, &$form_state) {
  $order_id = $form['#order_id'];
  $order = uc_order_load($order_id);
  
  // Determine how much has been paid, less previous refunds
  $sql = 'SELECT SUM(amount) FROM {uc_payment_receipts} WHERE order_id = %d';
  $total_paid = db_result(db_query($sql, array($order_id)));
  
  if ($form_state['values']['refund_type'] == 'partial') {
    $amount = (float)$form_state['values']['amount'];
    // Can't refund more than they've paid
    if ($amount > $total_paid) {
      $amount = $total_paid;
    }
  }
  else {
    $amount = $total_paid;
  }
  // Make the charge amount negative
  if ($amount > 0) {
    $amount = $amount * -1;
  }
  
  
  $payment_details = db_fetch_object(db_query('SELECT gateway, txn_id FROM {fundraiser_webform_order} WHERE order_id = %d', array($order_id)));
  
  if (module_exists('gateway_manager')) {
 	  $info = gateway_manager_gateway_info($payment_details->gateway);
 	  $refund_function = $info['refund_function'];
 	  $data['config'] = $info['config'];
 	}
 	else {
 	  $refund_function = fundraiser_refund_function($node->gateway);
 	}
 	
 	// call the refund function and store the result
  $result = $refund_function($order->order_id, $amount, $data);
  
  if ($result['success']) {
		// enter the payment
		uc_payment_enter($order->order_id, 'fundraiser', $amount, 0, array('txn_type' => UC_CREDIT_CREDIT, 'txn_id' => $result['data']['txn_id']), 'Refund processed by the fundraiser module.');
	
		// mark payment as received
		// @todo - what should this be?
		//db_query("update {uc_orders} set order_status='payment_received' where order_id = %d", $order->order_id);
    
 		// determine if the donation should be added to the queue
 		$add_to_queue = (module_exists('queue_api')) ? TRUE : FALSE;
 		
 		drupal_set_message($result['message'],'status');
 	}
 	else { 
		// failed to process credit cart
		$log_error = 'Refund on order ' . $order->order_id . ' failed gateway validation. Reason: ' . $result['message'];
		watchdog('fundraiser', $log_error, NULL, WATCHDOG_DEBUG, NULL);
		drupal_set_message($result['message']);
		$form_state['rebuild'] = TRUE;
		$form_state['values']['abort'] = TRUE;
 	}
}