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
function fundraiser_show_refund_form($order) {
  $module_path = drupal_get_path('module', 'fundraiser') . '/';
  drupal_add_css($module_path . 'refund.css');
  drupal_add_js($module_path . 'refund.js');
  return drupal_get_form('fundraiser_refund', $order);
}


/**
 * Refund form
 */
function fundraiser_refund(&$form_state, $order) {
  $form = array();
  $form['#order_id'] = $order->order_id;

  // Determine how much has been paid, less previous refunds
  $sql = 'SELECT SUM(amount) FROM {uc_payment_receipts} WHERE order_id = %d';
  $total_paid = db_result(db_query($sql, array($order->order_id)));
  
  $context = array(
    'revision' => 'formatted-original',
    'type' => 'amount',
  );
  
  // Get payment info
  $payment_info = db_fetch_array(db_query('SELECT u.receipt_id, u.amount, u.received, f.txn_id FROM {uc_payment_receipts} u LEFT JOIN {fundraiser_webform_order} f ON u.order_id = f.order_id WHERE u.amount > 0 AND u.order_id=%d', $order->order_id));

  // If not payment info was found, check if it was a recurring order
  if (!$payment_info) {
    $payment_info = db_fetch_array(db_query('SELECT u.receipt_id, u.amount, u.received, f.txn_id FROM {uc_payment_receipts} u LEFT JOIN {fundraiser_recurring} f ON u.order_id = f.order_id WHERE u.amount > 0 AND u.order_id=%d', $order->order_id));  
  }
  
  $sql = 'SELECT SUM(amount) FROM {uc_payment_receipts} WHERE order_id = %d AND amount < 0';
  $previous_refunds = db_result(db_query($sql, array($order->order_id)));
  
  $form['payment_summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Summary'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  if (!$payment_info) {
    $form['payment_summary']['no_payments'] = array(
      '#value' => 'No payments have been made on this order yet',
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
    
  }
  else {
    $form['payment_summary']['payment_number'] = array(
      '#type' => 'item',
      '#title' => t('Payment #'),
      '#value' => $payment_info['receipt_id'],
    );
    $form['payment_summary']['payment_date'] = array(
      '#type' => 'item',
      '#title' => t('Payment Date'),
      '#value' => format_date($payment_info['received']),
    );
    $form['payment_summary']['payment_total'] = array(
      '#type' => 'item',
      '#title' => t('Payment Total'),
      '#value' => uc_price($payment_info['amount'], $context),
    );
    if ($previous_refunds < 0) {
      $form['payment_summary']['total_refunded'] = array(
        '#type' => 'item',
        '#title' => t('Total Previously Refunded'),
        '#value' => uc_price($previous_refunds, $context),
      );    
    }
  }
  
  
  $form['refund_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Refund Options'),
    '#description' => t('Note that refunds can only be applied to payments that have been settled in the payment processor. This typically happens every night; if the payment was made today, you will probably need to wait until tomorrow to issue a refund.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  
  // Only show the refund form if the payment was made through a payment gateway that can handle refunds
  $can_refund = fundraiser_gateway_can_refund_order($order);
  if (!$can_refund || !$payment_info) {
    // They've made a payment, but it cannot be refunded
    if ($payment_info) {
      $no_refund_text = 'Refunds cannot be made on this order. The payment was made on a payment gateway that does not support refunds.';
    }
    // They have NOT made any payment yet, so there's nothing to refund
    else {
      $no_refund_text = 'No payments have been processed on this order, so no refunds can be made yet.';
      // Even once the payment is made, they won't be able to refund it
      if (!$can_refund) {
        $no_refund_text .= ' However, because this order was made on a payment gateway that does not support refunds, you will not be able to make refunds after payments are processed on this order.';
      }
    }
    $form['refund_options']['no_refund_explanation'] = array(
      '#value' => $no_refund_text,
      '#prefix' => '<strong>',
      '#suffix' => '</strong>',
    );
  }
  // Refund options
  else {
    $form['refund_options']['full_refund'] = array(
      '#type' => 'radio',
      '#title' => t('Full Refund (%amount)', array('%amount' => uc_price($total_paid, $context))),
      '#return_value' => 'full',
      '#parents' => array('refund_type'),
    );
    $form['refund_options']['partial_refund'] = array(
      '#type' => 'radio',
      '#title' => t('Partial Refund'),
      '#return_value' => 'partial',
      '#parents' => array('refund_type'),
    );
  
    $form['refund_options']['amount'] = array(
      '#type' => 'textfield',
      '#title' => t('Amount to Refund'),
      '#size' => 40,
      '#maxlength' => 255,
    );
    $form['refund_options']['refund_notes'] = array(
      '#type' => 'textarea',
      '#title' => t('Refund Notes'),
      '#description' => t('Please provide reference information about this refund (Requestor, reason, etc.)'),
      '#cols' => 60,
      '#rows' => 5,
      '#required' => TRUE,
    );
    $form['refund_options']['issue_refund'] = array(
      '#type' => 'submit',
      '#value' => t('Issue Refund'),
      '#required' => TRUE,
    );
  
    $form['total_paid'] = array(
      '#type' => 'hidden',
      '#value' => number_format($total_paid, 2),
    );
  }
  
  return $form;
}


/**
 * Validate the form
 */
function fundraiser_refund_validate($form, &$form_state) {
  // Make sure they chose a refund type
  if (empty($form_state['values']['refund_type'])) {
    form_set_error('refund_type', t('Choose a refund type.'));
  }
  // If it isn't a full refund, make sure they entered a valid value
  elseif ($form_state['values']['refund_type'] != 'full') {
    $amount = (float)$form_state['values']['amount'];
    if (!($amount > 0)) {
      form_set_error('amount', t('Enter a refund amount.'));
    }
    else {
      // Make sure they don't try to refund more than was paid
      $order_id = $form['#order_id'];
      // Determine how much has been paid, less previous refunds
      $sql = 'SELECT SUM(amount) FROM {uc_payment_receipts} WHERE order_id = %d';
      $total_paid = db_result(db_query($sql, array($order_id)));
      if ($amount > $total_paid) {
        $context = array(
          'revision' => 'formatted-original',
          'type' => 'amount',
        );
        form_set_error('amount', t('The refund amount entered is too high. Only @paid in payments have been made, and no more than that can be refunded.', array('@paid' => uc_price($total_paid, $context))));
      }
    }
    
    
  }
}


/**
 * Submit the refund form
 */
function fundraiser_refund_submit($form, &$form_state) {
  global $user;
  
  $order_id = $form['#order_id'];
  $order = uc_order_load($order_id);
  
  // Determine how much has been paid, less previous refunds
  $sql = 'SELECT SUM(amount) FROM {uc_payment_receipts} WHERE order_id = %d';
  $total_paid = db_result(db_query($sql, array($order_id)));
  
  
  // Partial refund
  if ($form_state['values']['refund_type'] != 'full') {
    $new_order_status = 'partially_refunded';
    $amount = (float)$form_state['values']['amount'];
    // Can't refund more than they've paid
    // Use >= so that the status is set properly if they choose Partial Refund
    // but refund the entire amount
    if ($amount >= $total_paid) {
      $amount = $total_paid;
      $new_order_status = 'refunded';
    }
  }
  // Full refund
  else {
    $new_order_status = 'refunded';
    $amount = $total_paid;
  }

  // Make the charge amount negative
  $amount = abs($amount) * -1;

  // Get details on the original payment so we can load the payment gateway details
  $payment_details = db_fetch_object(db_query('SELECT gateway, txn_id FROM {fundraiser_webform_order} WHERE order_id = %d UNION SELECT gateway, txn_id FROM {fundraiser_recurring} WHERE order_id = %d', $order_id, $order_id));
  
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
		
		// Add the comment
	  $context = array(
      'revision' => 'formatted-original',
      'type' => 'amount',
    );
    
    // Make the charge amount positive again, for nice display in the admin comment
    $amount = abs($amount);

    $reason = check_plain($form_state['values']['refund_notes']);

  	$amount_formatted = uc_price($amount, $context);
		uc_order_comment_save($order_id, $user->uid, t('%amount refund issued by %user. Refund notes: %reason', array('%amount' => $amount_formatted, '%user' => $user->name, '%reason' => $reason)), 'admin');
		
		// get the receipt_id that belongs to this refund
		$receipt_id = NULL;
		$payments = uc_payment_load_payments($order_id);
		foreach ($payments as $payment) {
		  $data = unserialize($payment->data);
		  if ($data['txn_id'] == $result['data']['txn_id']) {
		    $receipt_id = $payment->receipt_id;
		  }
		}
		
		// Save refund data
		$refund = new stdClass();
		$refund->order_id = $order_id;
		$refund->receipt_id = $receipt_id;
		$refund->amount = $amount;
		$refund->txn_id = $result['data']['txn_id'];
		$refund->reason = $reason;
		drupal_write_record('fundraiser_refund', $refund);
	
		// Set order status to Refunded or Partially Refunded
		db_query("update {uc_orders} set order_status='%s' where order_id = %d", $new_order_status, $order_id);
    
 		// If using the Salesforce queue, queue up the updates
 		if (module_exists('queue_api')) {
 		  // Update the original order
 		  sf_queue_insert($order->order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
 		  // Create the refund
 		  sf_queue_insert($refund->refund_id, FUNDRAISER_DONATION_REFUND_TYPE, 'create');
 		}
 		
 		drupal_set_message(t('Refund has been issued.'),'status');
 		drupal_goto('admin/store/orders/' . $order_id);
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