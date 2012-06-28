<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Form builder. Configure fundraiser module.
 */
function fundraiser_admin_settings($form, &$form_state) {
  $form['fundraiser_development_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Development mode'),
    '#default_value' => variable_get('fundraiser_development_mode', 0),
    '#description' => t('Development mode allows you to access donation forms over HTTP.'),
  );

  $form['fundraiser_default_minimum'] = array(
    '#type' => 'textfield',
    '#title' => t('Default minimum donation amount'),
    '#default_value' => variable_get('fundraiser_default_minimum', 10.00),
    '#description' => t('Enter the default minimum donation amount that will be used on new donation forms.'),
  );

  // TODO perhaps this should move to another module and be added here with form alter?
  $description = 'Use this option if you want donations to be exported to Salesforce.';
  $attributes = array();
  if (!module_exists('sf_donation')) {
    $description .= ' <span class="admin-disabled">The Salesforce Management Donations module must be enabled in order to use this option.</span>';
    $attributes['disabled'] = 'disabled';
  }
  $form['fundraiser_salesforce_integration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Integrate with Salesforce'),
    '#default_value' => variable_get('fundraiser_salesforce_integration', 0),
    '#description' => t($description),
    '#attributes' => $attributes,
  );

  $form['fundraiser_all_countries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Insert full list of countries into new donation forms'),
    '#default_value' => variable_get('fundraiser_all_countries', 0),
    '#description' => t('When checked, all countries will be inserted into the billing country field on new donations forms. When unchecked, on U.S and Canda will be inserted.'),
  );

  $form['fundraiser_gateway_messages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display gateway messages'),
    '#default_value' => variable_get('fundraiser_gateway_messages', 1),
    '#description' => t('When checked, the message that is returned from the payment gateway will be displayed in the Drupal status message area of the theme.'),
  );

  $form['fundraiser_receipt_bcc'] = array(
    '#type' => 'textfield',
    '#title' => t('Donation receipt bcc email address'),
    '#default_value' => variable_get('fundraiser_receipt_bcc', ''),
    '#description' => t('The email address of the user that will receive a bcc copy of all donation receipt emails.'),
  );

  // TODO Check this to make sure these are the right tokens to display given the radical changes in this part of D7. - SeH
  $form['fundraiser_cc_expiration_message']['tokens']['user'] = array(
    '#markup' => theme('token_tree', array('token_types' => array('user'))),
  );


  // TODO perhaps this should move to another module and be added here with form alter?
  if (module_exists('form_layouts')) {
    $form['fundraiser_form_layouts'] = array(
      '#type' => 'fieldset',
      '#title' => t('Form layouts settings'),
      '#description' => t('Configure form layouts.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form_layouts = array();
    foreach (_form_layouts_template_list() as $template) {
      $form_layouts[$template['theme']] = $template['name'];
    }
    $form['fundraiser_form_layouts']['fundraiser_form_layouts_default'] = array(
      '#type' => 'select',
      '#title' => t('Default form layout'),
      '#description' => t('Select the default form layout.'),
      '#default_value' => variable_get('fundraiser_form_layouts_default', 'one_column'),
      '#options' => $form_layouts,
    );
  }
  return system_settings_form($form);
}

/**
 * Refund tab on Orders
 */
function fundraiser_show_refund_form($order_id) {
  $base = drupal_get_path('module', 'fundraiser');
  drupal_add_css($base . '/css/refund.css');
  drupal_add_js($base . '/js/refund.js');
  return drupal_get_form('fundraiser_refund', $order_id);
}

/**
 * Refund form
 */
function fundraiser_refund(&$form_state, $order_id) {
  $form = array();
  $order = module_invoke_all('fundraiser_get_order', $order_id);
  $order = $order[0];
  $form['#order_id'] = $order->order_id;

  // Determine how much has been paid, less previous refunds.
  $total_paid = module_invoke_all('fundraiser_get_order_paid', $order->order_id);
  $total_paid = $total_paid[0];

  // Get payment info
  $payment_info = module_invoke_all('fundraiser_get_order_payment_info', $order->order_id);
  $payment_info = $payment_info[0];

  // Get refund info
  $previous_refunds = module_invoke_all('fundraiser_get_order_refund_info', $order->order_id);
  $previous_refunds = $previous_refunds[0];

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
    $formated_price = module_invoke_all('fundraiser_format_price', $payment_info['received']);
    $form['payment_summary']['payment_date'] = array(
      '#type' => 'item',
      '#title' => t('Payment Date'),
      '#value' => $formated_price[0],
    );
    $formated_price = module_invoke_all('fundraiser_format_price', $payment_info['amount']);
    $form['payment_summary']['payment_total'] = array(
      '#type' => 'item',
      '#title' => t('Payment Total'),
      '#value' => $formated_price[0],
    );
    if ($previous_refunds < 0) {
      $formated_price = module_invoke_all('fundraiser_format_price', $previous_refunds);
      $form['payment_summary']['total_refunded'] = array(
        '#type' => 'item',
        '#title' => t('Total Previously Refunded'),
        '#value' => $formated_price[0],
      );
    }
  }

  $form['refund_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Refund Options'),
    '#description' => t('Note that refunds can only be applied to payments that have been settled in the payment processor. This typically ' .
     'happens every night; if the payment was made today, you will probably need to wait until tomorrow to issue a refund.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Only show the refund form if the payment was made through a payment gateway that can handle refunds
  $gateway = _fundraiser_get_donation_gateway($order->order_id);
  $can_refund = isset($gateway['credit_refund']) ? $gateway['credit_refund'] : NULL;
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
        $no_refund_text .= ' However, because this order was made on a payment gateway that does not support refunds, you will not be able to ' .
          'make refunds after payments are processed on this order.';
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
    $formated_price = module_invoke_all('fundraiser_format_price', $total_paid);
    $form['refund_options']['full_refund'] = array(
      '#type' => 'radio',
      '#title' => t('Full Refund (%amount)', array('%amount' => $formated_price[0])),
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
    $amount = (float) $form_state['values']['amount'];
    if (!($amount > 0)) {
      form_set_error('amount', t('Enter a refund amount.'));
    }
    else {
      // Make sure they don't try to refund more than was paid
      $order_id = $form['#order_id'];
      // Determine how much has been paid, less previous refunds
      $total_paid = module_invoke_all('fundraiser_get_order_paid', $order_id);
      $total_paid = $total_paid[0];
      if ($amount > $total_paid) {
        $context = array(
          'revision' => 'formatted-original',
          'type' => 'amount',
        );
        $formated_price = module_invoke_all('fundraiser_format_price',$total_paid);
        form_set_error('amount', t('The refund amount entered is too high. Only @paid in payments have been made, and no ' .
          'more than that can be refunded.', array('@paid' => $formated_price[0])));
      }
    }
  }
}

/**
 * Submit the refund form
 */
function fundraiser_refund_submit($form, &$form_state) {
  // Get the order.
  $order_id = $form['#order_id'];

  // Determine how much has been paid, less previous refunds
  $total_paid = module_invoke_all('fundraiser_get_order_paid', $order_id);
  $total_paid = $total_paid[0];

  // Partial refund
  if ($form_state['values']['refund_type'] != 'full') {
    $new_order_status = 'partially_refunded';
    $amount = (float) $form_state['values']['amount'];
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
  $payment_details = db_query('SELECT gateway, txn_id FROM {fundraiser_webform_order} ' .
    'WHERE order_id = :order_id ' .
    'UNION '.
    'SELECT gateway, txn_id FROM {fundraiser_recurring} '. 
    'WHERE order_id = :order_id', array(':order_id' => $order_id))->fetchObject();

  if (module_exists('gateway_manager')) {
    $info = gateway_manager_gateway_info($payment_details->gateway);
    $refund_function = $info['refund_function'];
    $data['config'] = $info['config'];
  }
  else {
    $refund_function = fundraiser_refund_function($node->gateway);
  }

  // call the refund function and store the result
  $result = $refund_function($order_id, $amount, $data);

  if ($result['success']) {

    $receipt_id = module_invoke_all('fundraiser_order_refund', $order_id, $amount, $new_order_status, $result['data']['txn_id'], $form_state['values']['refund_notes']);
    $receipt_id = $receipt_id[0];

    // Save refund data for Fundraiser.
    $refund = new stdClass();
    $refund->order_id = $order_id;
    $refund->receipt_id = $receipt_id;
    $refund->amount = $amount;
    $refund->txn_id = $result['data']['txn_id'];
    $refund->reason = $reason;
    drupal_write_record('fundraiser_refund', $refund);

    // If using the Salesforce queue, queue up the updates
    // This could be on hook couldn't it? - SeH
    if (module_exists('queue_api')) {
      // Update the original order
      sf_queue_insert($order_id, FUNDRAISER_SINGLE_DONATION_TYPE, 'update');
      // Create the refund
      sf_queue_insert($refund->refund_id, FUNDRAISER_DONATION_REFUND_TYPE, 'create');
    }

    drupal_set_message(t('Refund has been issued.'), 'status');
    drupal_goto('admin/store/orders/' . $order_id);
  }
  else {
    // failed to process credit cart
    $log_error = 'Refund on order ' . $order_id . ' failed gateway validation. Reason: ' . $result['message'];
    watchdog('fundraiser', $log_error, NULL, WATCHDOG_DEBUG, NULL);
    drupal_set_message($result['message']);
    $form_state['rebuild'] = TRUE;
    $form_state['values']['abort'] = TRUE;
  }
}
