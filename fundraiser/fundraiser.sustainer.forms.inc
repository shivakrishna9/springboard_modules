<?php

/**
 * @file
 * Sustainer management forms for the fundraiser module.
 *
 *
 * @ingroup fundraiser
 */

/*----------------------------------------------------------------------------------
 *
 * Fundraiser billing update form
 *
 *--------------------------------------------------------------------------------*/

/**
 * Form to update billing address for a recurring donation series.
 */
function fundraiser_billing_update_form(&$form_state, $master_order_id, $order) {
  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => $order->uid,
  );

  $form['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );

  $form['billing_address'] = array(
    '#type' => 'fieldset',
    '#title' => 'Billing Address',
    '#attributes' => array('id' => 'billing-address'),
  );

  $form['billing_address']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => "First Name",
    '#required' => TRUE,
    '#default_value' => $order->billing_first_name,
  );

  $form['billing_address']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => "Last Name",
    '#required' => TRUE,  
    '#default_value' => $order->billing_last_name,
  );

  $form['billing_address']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#required' => TRUE,
    '#default_value' => $order->primary_email,
  );

  $form['billing_address']['address_1'] = array(
    '#type' => 'textfield',
    '#title' => 'Address Line 1',
    '#size' => 30,
    '#required' => TRUE,
    '#default_value' => $order->billing_street1,
  );

  $form['billing_address']['address_2'] = array(
    '#type' => 'textfield',
    '#title' => 'Address Line 2',
    '#size' => 30,
    '#default_value' => $order->billing_street2,
  );

  $form['billing_address']['city'] = array(
    '#type' => 'textfield',
    '#title' => 'City',
    '#size' => 30,
    '#required' => TRUE,
    '#default_value' => $order->billing_city,
  );

  $countries = uc_country_select(uc_get_field_name('country'), 840, NULL, 'name', uc_address_field_required('country'));
  $form['billing_address']['country'] = array(
    '#type' => 'select',
    '#title' => 'Country',
    '#options' => $countries['#options'],
    '#required' => TRUE,  
    '#default_value' => $order->billing_country,
  );

  $zones = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, 840, 'name', uc_address_field_required('zone'));
  $form['billing_address']['state'] = array(
    '#type' => 'select',
    '#title' => 'State',
    '#options' => $zones['#options'],
    '#required' => TRUE,  
    '#default_value' => $order->billing_zone,
  );

  $form['billing_address']['zipcode'] = array(
    '#type' => 'textfield',
    '#title' => 'Postal Code',
    '#size' => 5,
    '#maxlength' => 11,
    '#required' => TRUE,  
    '#default_value' => $order->billing_postal_code,
  );

  $form['cc_info'] = array(
    '#type' => 'fieldset',
    '#title' => 'Credit Card Information',
    '#attributes' => array('id' => 'credit-card-info'),
  );

  $form['cc_info']['card_num'] = array(
    '#type' => 'textfield',
    '#title' => 'Card Number',
    '#size' => 20,
    '#required' => TRUE,
    '#maxlength' => 20,
  );

  $form['cc_info']['card_exp_date'] = array(
    '#type' => 'date',
    '#required' => TRUE,
    '#title' => 'Expiration Date',
    '#default_value' => array('year' => date('Y'), 'month' => 1, 'day' => 1),
    '#process' => array('_fundraiser_expand_date'),
  );

  $form['cc_info']['card_cvv'] = array(
    '#type' => 'textfield',
    '#title' => 'Security Code',
    '#required' => TRUE,
    '#size' => 5,
    '#maxlength' => 5,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

/**
 * Validation for billing update form.
 */
function fundraiser_billing_update_form_validate($form, &$form_state) {
  // validate credit card information
  $cc_number = $form_state['values']['card_num'];
  $cc_expiration_month =  $form_state['values']['card_exp_date']['month'];
  $cc_expiration_year = $form_state['values']['card_exp_date']['year'];
  $cc_cvv = $form_state['values']['card_cvv'];

  //validate cc number
  if ((variable_get('uc_credit_validate_numbers', TRUE) && !_valid_card_number($cc_number))
     || !ctype_digit($cc_number)) {
    form_set_error('cc_info][card_num', "You have entered an invalid credit card number.");
  }

  // Validate the card expiration date.
  if (!_valid_card_expiration($cc_expiration_month, $cc_expiration_year)) {
    form_set_error('cc_info][card_exp_date', t('The credit card you entered has expired.'));
  }

  // Validate the CVV Number
  if (variable_get('uc_credit_cvv_enabled', TRUE) && !_valid_cvv($cc_cvv)) {
    form_set_error('cc_info][card_cvv', t('You have entered an invalid CVV number.'));
  }
}

/**
 * Updates all pending donations with new billing information
 */
function fundraiser_billing_update_form_submit($form, &$form_state) {
  include_once 'fundraiser.billing.inc';
  $orders_changed = array();

  $master_order_id = $form_state['values']['master_order_id'];
  $payment_details = _fundraiser_get_payment_details($master_order_id);
  $exp_month = $form_state['values']['card_exp_date']['month'];
  $exp_year = $form_state['values']['card_exp_date']['year'];

  // we can only continue if we have a gateway and reference_id
  if ($payment_details['gateway'] && $payment_details['reference_id']) {
    $result = fundraiser_update_billing_info($payment_details['gateway']['id'], $payment_details['reference_id'], $form_state['values']);
    if ($result) {
      watchdog('fundraiser', 'Billing information was updated for profile @profile', array('@profile' => $payment_details['reference_id']), WATCHDOG_INFO));
      $orders = _get_pending_recurring_orders($master_order_id);
      while ($row = db_fetch_object($orders)) {
        $order = _update_order_billing_info($row->order_id, $form_state);
        // put order back in queue
        if (module_exists('sf_donation') && module_exists('queue_api')) {
          sf_queue_insert($order->order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
        }
        $orders_changed[] = $order->order_id;
      }

      // create new orders based on expiration date
      fundraiser_extend_future_orders($master_order_id, $exp_month, $exp_year);

      drupal_set_message(t('Billing information successfully updated'));
    }
    else {
      drupal_set_message(t('Unable to update billing information at this time'));
    }

  }
}


/*----------------------------------------------------------------------------------
 *
 * Fundraiser donation amount update form
 *
 *--------------------------------------------------------------------------------*/

/**
 * Creates a form for updating donation amount
 */
function fundraiser_donation_amount_form(&$form_state, $master_order_id, $nid, $donation_amount) {
  $form['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid,
  );

  $amount = money_format('%i', $donation_amount);
  $minimum = money_format('%i', _minimum_donation_amount($nid));
  $form['fee_amount'] = array(
    '#type' => 'textfield',
    '#title' => "Donation Amount",
    '#default_value' => $amount,
    '#size' => 10,
    '#required' => TRUE,
    '#description' => "<span id='current'>Your current donation amount is $$amount</span> <span id='minimum'>Minimum donation $$minimum</span>",
    '#field_prefix' => '$',
  );

  $form['submit'] = array(
    '#type' => 'submit',
     '#value' => t('Update donation amount'),
  );

  return $form;
}

/**
 * Valdation for amount update form.
 */
function fundraiser_donation_amount_form_validate($form, &$form_state) {
  $form_state['values']['fee_amount'] = preg_replace("/[^\d\.]/i", "", $form_state['values']['fee_amount']);
  if (!is_numeric($form_state['values']['fee_amount'])) {
    form_set_error('amount', t('Donation amount must be a valid number.'));
  }

  // get minimum amount
  $minimum = _minimum_donation_amount($form_state['values']['nid']);
  if ($form_state['values']['fee_amount'] < $minimum) {
    $minimum = money_format('%i', $minimum);
    form_set_error('amount', t("Donation amount must be greater than $$minimum"));
  }
}

/**
 * Submit handler for donation amount update form.
 */
function fundraiser_donation_amount_form_submit($form, &$form_state) {
  global $user;
  $amount = $form_state['values']['fee_amount'];
  $master_order_id = $form_state['values']['master_order_id'];

  $orders = _get_pending_recurring_orders($master_order_id);
  $orders_changed = array();

  while ($row = db_fetch_object($orders)) {
    // update ubercart orders first
    $order = uc_order_load($row->order_id);
    $old_amount = $order->order_total;
    $orders_changed[] = $row->order_id;

    // update the order total
    $order->order_total = $amount;
    // update the product price
    $order->products[0]->price = $amount;
    // save the order
    uc_order_save($order);
    uc_order_comment_save($order->order_id, $user->uid, "Donation amount changed from
      $old_amount to $amount by an administrator.", 'admin');

    // put order back in queue so that amount gets updated in Salesforce
    if (module_exists('sf_donation') && module_exists('queue_api')) {
      sf_queue_insert($order->order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
    }
  }

  drupal_set_message(t('The amount of all future donations has been updated to @amount.', array('@amount' => $amount));
}

/*----------------------------------------------------------------------------------
 *
 * Recurring donation cancellation form
 *
 *--------------------------------------------------------------------------------*/

/**
 * Form for cancelling all pending donations.
 */
function fundraiser_cancel_form(&$form_state, $master_order_id) {
  // Only allow certain users to cancel future donations.
  $form['#access'] = user_access('cancel future donations');

  $form['reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason'),
    '#default_value' => t('Please enter a brief description of why you are cancelling. This will help us better serve you in the future.'),
    '#maxlength' => 500,
    '#cols' => 70,
    '#rows' => 3,
    '#resizable' => FALSE,
  );

  $form['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel all future payments'),
  );

  return $form;
}

/**
 * Submit handler for cancel donation form.
 */
function fundraiser_cancel_form_submit($form, &$form_state) {
  global $user;

  $master_order_id = $form_state['values']['master_order_id'];
  $reason = $form_state['values']['reason'];

  db_query("UPDATE {fundraiser_webform_order} SET recurring_status = %d WHERE order_id = %d", DONATION_CANCELLED, $master_order_id);

  // get the ids of the child orders
  $result = _get_pending_recurring_orders($master_order_id);

  // cancel all child orders that are still pen
  $sql = "UPDATE {uc_orders} u inner join {fundraiser_recurring} f on f.order_id = u.order_id
  SET u.order_status = 'canceled' where f.master_order_id = %d and u.order_status = 'pending_future_payment'";
  db_query($sql, $master_order_id);

  // delete recurring information
  $sql = "update {fundraiser_recurring} set gateway_resp = 'canceled', cancellation_reason = '%s' where master_order_id = %d AND (gateway_resp IS NULL OR gateway_resp = 'failed')";
  db_query($sql, $reason, $master_order_id);

  $order_ids = array();
  // add a comment to the canceled orders
  while ($data = db_fetch_object($result)) {
    $order_ids[] = $data->order_id;
    uc_order_comment_save($data->order_id, $user->uid, 'Order canceled by an administrator. Reason: ' . $reason, 'order', 'canceled', FALSE);
    // put order back in queue so that amount gets updated in Salesforce
    if (module_exists('sf_donation') && module_exists('queue_api')) {
      sf_queue_insert($data->order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
    }
  }



  // call out to saleforce module if installed
  //if (module_exists('sf_donation')) {
  //  sf_donation_canel_recurring_donations($master_order_id);
  //}

  drupal_set_message(t('All future payments cancelled.'));
}

/*----------------------------------------------------------------------------------
 *
 * Helper functions
 *
 *--------------------------------------------------------------------------------*/

/**
 * Gets the minimum donation amount for a given donation form
 */
function _minimum_donation_amount($nid) {
  return db_result(db_query("SELECT minimum_donation_amount FROM {fundraiser} where nid = %d", $nid));
}

/**
 * Loads an order and updates it's billing information from a form
 */
function _update_order_billing_info($order_id, &$form_state) {
  global $user;

  $order = uc_order_load($order_id);
  $order->primary_email = $form_state['values']['email'];
  $order->billing_first_name =  $form_state['values']['first_name'];
  $order->billing_last_name = $form_state['values']['last_name'];
  $order->billing_city = $form_state['values']['city'];
  $order->billing_street1 = $form_state['values']['address_1'];
  $order->billing_street2 = $form_state['values']['address_2'];
  $order->billing_postal_code = $form_state['values']['zipcode'];
  $order->billing_zone = $form_state['values']['state'];
  $order->billing_country = $form_state['values']['country'];
  $order->payment_method = 'credit';
  $order->payment_details = array(
    'cc_type' => _fundraiser_get_cc_type($form_state['values']['card_num']),
    'cc_owner' => '',
    'cc_number' => $form_state['values']['card_num'],
    'cc_start_month' => '',
    'cc_start_year' => '',
    'cc_exp_month' => $form_state['values']['card_exp_date']['month'],
    'cc_exp_year' => $form_state['values']['card_exp_date']['year'],
    'cc_issue' => '',
    'cc_cvv' => $form_state['values']['card_cvv'],
    'cc_bank' => '',
  );

  uc_credit_cache('save', $order->payment_details, FALSE);
  uc_order_save($order);
  uc_credit_cache('clear');
  uc_order_comment_save($order->order_id, $user->uid, "Billing information updated.");
  return $order;
}


/**
 * Returns the payment gateway and transaction reference used for a recurring donation
 */
function _fundraiser_get_payment_details($order_id) {
  $order = uc_order_load($order_id);
  $details = array();
  $gateway = '';

  // first, try to get the gateway from individual order
  if ($order->data['gateway']) {
    $gateway = $order->data['gateway'];
  }
  else {
    // next, revert to the donation form that created the order
    $donation_form_nid = $order->products[0]->nid;
    $gateway = db_result(db_query("SELECT gateway FROM {fundraiser} WHERE nid = %d", $donation_form_nid));
  }

  // get the gateway's details
  $details['gateway'] = _fundraiser_lookup_gateway_details($gateway);

  // get the txn reference from the order
  $references = array_keys($order->data['cc_txns']['references']);
  $details['reference_id'] = array_shift($references);

  return $details;
}

/**
 * Look up a payment gateway's details based on it's id
 */
function _fundraiser_lookup_gateway_details($gateway) {
  $gateways = _payment_gateway_list('', TRUE);

  // use the name to look up the gateway
  foreach ($gateways as $k => $v) {
    if ($v['id'] == $gateway) {
      return $v;
    }
  }
  return FALSE;
}

/**
 * Gets pending orders in the recurring donations series.
 */
function _get_pending_recurring_orders($order_id) {
  // get the ids of the child orders
  return db_query("SELECT order_id FROM {fundraiser_recurring} WHERE master_order_id = %d AND (gateway_resp IS NULL OR gateway_resp = 'failed')", $order_id);
}
