<?php

/**
 * @file
 * Sustainer page callbacks for the fundraiser module.
 *
 * @ingroup fundraiser
 */

/**
 * Menu callback for the my recurring payments tab of the user profile.
 */
function fundraiser_recurring_user_overview($uid) {
  // gather all information for the page and send it through a theme function
  $orders = _fundraiser_get_recurring_orders($uid);
  // grab the first order id so we can load up the page
  $order_ids = array_keys($orders);
  // redirect to the first found recurring order
  drupal_goto('user/' . $uid->uid . '/recurring_overview/' . $order_ids[0]);
}

/**
 * Loads a specific order for the my recurring payments tab.
 */
function fundraiser_recurring_user_edit_forms($uid, $order_id) {

  $base = drupal_get_path('module', 'fundraiser');
  drupal_add_js($base . '/js/sustainer.js');
  drupal_add_css($base . '/css/sustainer.css');
  // gather all information for the page and send it through a theme function
  $orders = _fundraiser_get_recurring_orders($uid);
  $order = _fundraiser_get_unprocessed_order($order_id);
  $nid = $order->products[0]->nid;

  $cancelled = _fundraiser_recurring_payment_cancelled($order_id);
  $billing_info = _fundraiser_get_billing_information($order_id, $order);
  $payment_info = _fundraiser_get_payment_information($order_id, $order);

  // get full payment schedule
  $payment_schedule = _fundraiser_recurring_payment_schedule($order_id);

  // get forms
  $donation_amount_form = drupal_get_form('fundraiser_donation_amount_form', $order_id, $nid, $payment_info['donation_amount']);
  $billing_update_form = drupal_get_form('fundraiser_billing_update_form', $order_id, $order);
  $cancel_form = drupal_get_form('fundraiser_cancel_form', $order_id);

  $output = theme('sustainer_management_page', $order_id, $orders, $payment_info, $billing_info, $donation_amount_form, $billing_update_form, $cancel_form, $payment_schedule, $cancelled);
  return $output;
}

/**
 * Gets the recurring orders for a given user id.
 */
function _fundraiser_get_recurring_orders(&$user) {
  $orders = array();
  $results = db_query('SELECT f.order_id, f.recurring_status FROM {fundraiser_webform_order} f '.
    'INNER JOIN {uc_orders} u ON u.order_id = f.order_id '.
    'WHERE recurring_status != :status AND u.uid = :uid',
    array(':status' => DONATION_NON_RECURRING, ':uid' => $user->uid));
  foreach($results as $data) {
    $created = _fundraiser_get_start_date($data->order_id);
    $next_charge = _fundraiser_get_next_charge_date($data->order_id);
    $last_charge = _fundraiser_get_last_charge_date($data->order_id);
    $amount = _fundraiser_get_current_donation_amount($data->order_id);
    $orders[$data->order_id] = array(
      'amount' => $amount,
      'start_date' => $created,
      'status' => $data->recurring_status,
      'next_charge_date' => $next_charge,
      'expire_date' => $last_charge,
    );
  }

  return $orders;
}

/**
 * Gets the payment information for a recurring donation.
 */
function _fundraiser_get_payment_information($order_id, &$order) {
  $payment_info = array(
    'orders_remaining' => _fundraiser_remaining_recurring_charge_count($order_id),
    'orders_processed' => _fundraiser_processed_recurring_charge_count($order_id),
    'donation_amount' => $order->order_total
  );
  return $payment_info;
}

/**
 * Gets the billing information for a recurring donation.
 */
function _fundraiser_get_billing_information($master_order_id, &$order) {
  $billing_info = array(
    'billing_first_name' => $order->billing_first_name,
    'billing_last_name' => $order->billing_last_name,
    'billing_street' => $order->billing_street1,
    'billing_street2' => $order->billing_street2,
    'billing_city' => $order->billing_city,
    'billing_state' => uc_get_zone_code($order->billing_zone),
    'billing_postal_code' => $order->billing_postal_code,
  );
  return $billing_info;
}


/**
 * Gets an unprocessed order in a recurring donation series.
 */
function _fundraiser_get_unprocessed_order($master_order_id) {
  $order_id = db_query('SELECT order_id FROM {fundraiser_recurring} ' .
   'WHERE master_order_id = :order_id AND (gateway_resp IS NULL OR gateway_resp = \'canceled\') LIMIT 1',
   array(':order_id' => $master_order_id))->fetchColumn();
  if ($order_id) {
    return uc_order_load($order_id);
  }
  return NULL;
}

/**
 * Counts the number of processed recurring orders.
 */
function _fundraiser_processed_recurring_charge_count($order_id) {
  return db_query('SELECT count(order_id) from {fundraiser_recurring} '.
   'WHERE master_order_id = :order_id AND gateway_resp = \'success\'',
   array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Counts the number of unprocessed orders remaining.
 */
function _fundraiser_remaining_recurring_charge_count($order_id) {
  return db_query('SELECT count(order_id) from {fundraiser_recurring} '.
   'WHERE master_order_id = :order_id AND (gateway_resp IS NULL or gateway_resp = \'failed\')',
   array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Returns the entire payment schedule for a recurring donation series.
 */
function _fundraiser_recurring_payment_schedule($master_order_id) {
  $orders = array();

  $results = db_query('SELECT u.order_total AS amount, f.next_charge AS charge_date, f.gateway_resp AS status FROM {fundraiser_recurring} f '.
    'INNER JOIN {uc_orders} u ON u.order_id = f.order_id '.
    'WHERE f.master_order_id = :order_id ' .
    'ORDER BY f.next_charge ASC',
    array(':order_id' => $master_order_id));

  foreach($results as $result) {
    $data = (array) $result;
    $orders[] = $data;
  }
  return $orders;
}

/**
 * Determines whether or not a recurring donation series is cancelled.
 */
function _fundraiser_recurring_payment_cancelled($order_id) {
  $status = db_query('SELECT recurring_status FROM {fundraiser_webform_order} WHERE order_id =:order_id', array(':order_id' => $order_id))->fetchColumn();
  if ($status == DONATION_CANCELLED) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Gets the current amount of a recurring donation. Gets an unprocessed order as the amount may have changed in the past.
 */
function _fundraiser_get_current_donation_amount($order_id) {
  // get amound from the next unprocessed recurring donation
  return db_query('SELECT u.order_total FROM {uc_orders} u '. 
    'INNER JOIN {fundraiser_recurring} f ON f.order_id = u.order_id '.
    'WHERE (f.gateway_resp IS NULL OR f.gateway_resp = \'canceled\') AND f.master_order_id = :order_id '.
    'ORDER BY next_charge LIMIT 1',
    array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Gets the next charge date of a recurring donation.
 */
function _fundraiser_get_next_charge_date($order_id) {
  return db_query('SELECT min(next_charge) FROM {fundraiser_recurring} WHERE master_order_id = :order_id',
    array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Gets the last payment date of a recurring donation.
 */
function _fundraiser_get_last_charge_date($order_id) {
  return db_query('SELECT max(next_charge) FROM {fundraiser_recurring} WHERE master_order_id = :order_id',
    array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Gets the start date of a recurring donation.
 */
function _fundraiser_get_start_date($order_id) {
  return db_query('SELECT created FROM {uc_orders} WHERE order_id = :order_id',
    array(':order_id' => $order_id))->fetchColumn();
}

/**
 * Override of Drupal core expand_date found in includes/form.inc
 */
function _fundraiser_expand_date($element) {
  // Default to current date
  if (empty($element['#value'])) {
    $element['#value'] = array(
      'day' => format_date(time(), 'custom', 'j'),
      'month' => format_date(time(), 'custom', 'n'),
      'year' => format_date(time(), 'custom', 'Y')
    );
  }

  $element['#tree'] = TRUE;

  // Determine the order of day, month, year in the site's chosen date format.
  $format = variable_get('date_format_short', 'm/d/Y - H:i');
  $sort = array();
  $sort['day'] = max(strpos($format, 'd'), strpos($format, 'j'));
  $sort['month'] = max(strpos($format, 'm'), strpos($format, 'M'));
  $sort['year'] = strpos($format, 'Y');
  asort($sort);
  $order = array_keys($sort);

  $current_year = date('Y');

  // Output multi-selector for date.
  foreach ($order as $type) {
    switch ($type) {
      case 'day':
        $options = drupal_map_assoc(range(1, 31));
        break;
      case 'month':
        $options = drupal_map_assoc(range(1, 12), 'map_month');
        break;
      case 'year':
        $options = drupal_map_assoc(range($current_year, $current_year + 10));
        break;
    }
    $parents = $element['#parents'];
    $parents[] = $type;
    $element[$type] = array(
      '#type' => 'select',
      '#value' => $element['#value'][$type],
      '#attributes' => $element['#attributes'],
      '#options' => $options,
    );
  }

  return $element;
}

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
      foreach ($orders as $row) {
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
    //TODO this should be t, with values inserted.
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

  foreach ($orders as $row) {
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

  db_query("UPDATE {fundraiser_webform_order} SET recurring_status = :recurring_status WHERE order_id = :order_id",
    array(':recurring_status' => DONATION_CANCELLED, ':order_id' => $master_order_id));

  // get the ids of the child orders
  $orders = _get_pending_recurring_orders($master_order_id);

  // cancel all child orders that are still pen
  db_query('UPDATE {uc_orders} u ' .
    'INNER JOIN {fundraiser_recurring} f ON f.order_id = u.order_id '.
    'SET u.order_status = \'canceled\' ' .
    'WHERE f.master_order_id = :order_id and u.order_status = \'pending_future_payment\'',
    array(':order_id' => $master_order_id));

  // delete recurring information
  db_query('UPDATE {fundraiser_recurring} '.
    'SET gateway_resp = \'canceled\', cancellation_reason = :reason '.
    'WHERE master_order_id = :order_id AND (gateway_resp IS NULL OR gateway_resp = \'failed\')',
    array(':reason' => $reason, ':order_id' => $master_order_id));

  $order_ids = array();
  // add a comment to the canceled orders
  foreach ($orders as $data) {
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
  return db_query('SELECT minimum_donation_amount FROM {fundraiser} where nid = :nid', array(':nid' => $nid))->fetchColumn();
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
    $gateway = db_query('SELECT gateway FROM {fundraiser} WHERE nid = :nid', array(':nid' => $donation_form_nid))->fetchColumn();
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
  $gateways = _fundraiser_get_gateway_info();
  // Use the name to look up the gateway.
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
  return db_query('SELECT order_id FROM {fundraiser_recurring} WHERE master_order_id = :order_id AND (gateway_resp IS NULL OR gateway_resp = \'failed\')',
   array(':order_id' => $order_id));
}


/**
 * Includes code for updating billing information for recurring donations.
 * Each gateway will require it's own function named GATEWAY_ID_update_billing_information
 * that accepts a payment reference id and billing information.
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
    watchdog('fundraiser', 'Billing information for recurring donations cannot be updated because the authorize_cim module is missing.', NULL, WATCHDOG_CRITICAL);
    return NULL;
  }
}

/**
 * Sage billing information update function.
 */
function sage_update_billing_information($reference_id, $billing_info) {
  $vault_id = db_query("SELECT vault_id from {uc_sage_vault} WHERE guid = :guid", array(':guid' => $reference_id))->fetchColumn();
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
  return TRUE;
}


// Called in fundraiser/fundraiser.sustainer.inc:
/**
 * Extends a current recurring donation series out to a new credit card expiration date.
 */
function fundraiser_extend_future_orders($order_id, $exp_month, $exp_year) {
  // determine the date of the last order, and the number of orders to create out to the new expiration date
  $exp = $exp_year . sprintf("%02d", $exp_month)  ;
  $results = db_query("SELECT max(order_id) as last_order_id, max(next_charge) as last_charge_date, period_diff(:exp, date_format(from_unixtime(max(next_charge)),'%Y%m')) as new_order_count FROM {fundraiser_recurring} WHERE master_order_id = :order_id",
    array(':exp' => $exp, ':order_id' => $order_id));

  foreach ($results as $data) {
    $last_order_id = $data->last_order_id;
    $last_charge_date = $data->last_charge_date;
    $new_order_count = $data->new_order_count;
  }

  // load up the last order since it will have the most recent changes
  $order = uc_order_load($last_order_id);
  $gateway = db_query("SELECT gateway FROM {fundraiser_recurring} WHERE order_id = :order_id", array(':order_id' => $last_order_id))->fetchColumn();
  for ($i = 1; $i <= $new_order_count; $i++) {
    $new_order_id = fundraiser_clone_order($order);
    $charge_date = strtotime("+$i months", $last_charge_date);
    $recurring = array('master_order_id' => $order_id, 'order_id' => $new_order_id, 'next_charge' => $charge_date, 'gateway' => $gateway);
    drupal_write_record('fundraiser_recurring', $recurring);
    uc_order_comment_save($new_order_id, 0, t('Payment will be processed on !date.', array('!date' => date('n/j/Y', $charge_date))), 'admin');
    // add these new orders to the queue
    if (module_exists('sf_donation') && module_exists('queue_api')) {
      sf_queue_insert($new_order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'create');
      sf_queue_insert($order_id, FUNDRAISER_RECURRING_DONATION_TYPE, 'update');
    }
  }
}
