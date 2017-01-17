<?php
/**
 * @file
 * Refund donation form functions.
 */

/**
 * Menu callback for refund tab on donations.
 */
function fundraiser_show_refund_form($did) {
  if (!user_access('refund donations')) {
    drupal_set_message(t('You do not have access to refund donations.'));
    return '';
  }
  drupal_add_css(drupal_get_path('module', 'fundraiser') . '/css/refund.css');
  drupal_add_js(drupal_get_path('module', 'fundraiser') . '/js/refund.js');
  return drupal_render(drupal_get_form('_fundraiser_refund', $did));
}

/**
 * Refund form.
 */
function _fundraiser_refund_form($form, &$form_state, $did) {
  $form = array();

  // Given a donation ID, grab all relevant information to create the form.
  $donation = fundraiser_donation_get_donation($did);
  $form_state['donation_id'] = $donation->did;

  $form['payment_summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Summary'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Since we'll be doing math on these numbers we'll use the whole number values.
  $order_wrapper = entity_metadata_wrapper('commerce_order', commerce_order_load($donation->did));
  $order_total = $order_wrapper->commerce_order_total->value();

  $donation_amount = $order_total['amount'];
  $refund_summary = array();
  $refunded_total = 0;

  if (is_array($donation->refunds)) {
    foreach ($donation->refunds as $refund) {
      if ($refund->status == 'refunded' || $refund->status == 'partially_refunded') {
        $refunded_total += abs($refund->amount);
      }
      $refund_summary[] = t('%status #%id for $%amount on %date.',
        array(
        '%id' => $refund->rid,
        '%amount' => abs($refund->amount),
        '%date' => date('m/d/y', isset($refund->created) ? $refund->created : 0),
        '%status' => $refund->status,
      )
      ); //TODO fix up value display for international.
    }

    $refund_summary = !empty($refund_summary) ? theme('item_list', array('items' => $refund_summary)) : t('No refunds have been made.');
    $refunded_total = $refunded_total;
  }
  // Display form.
  if ($donation->refunds == FALSE) {
    $form['payment_summary']['no_payments'] = array(
      '#type' => 'item',
      '#markup' => t('No refunds have been made on this order yet.'),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
  }
  $form['payment_summary']['payment_total'] = array(
    '#type' => 'item',
    '#title' => t('Payment Total'),
    '#markup' => $donation->donation['amount_formatted'],
  );
  $form['payment_summary']['refunded_total'] = array(
    '#type' => 'item',
    '#title' => t('Refunded Total'),
    '#markup' => commerce_currency_format($refunded_total, $donation->currency, NULL, FALSE),
  );
  if (!empty($refund_summary)) {
    $form['payment_summary']['refunded_summary'] = array(
      '#type' => 'item',
      '#title' => t('Previously Refunded'),
      '#markup' => $refund_summary,
    );
  }

  // Form for submitting a refund.
  $form['refund_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Refund Options'),
    '#description' => t('Note that refunds can only be applied to payments that have been settled ' .
     'in the payment processor. This typically happens every night; if the payment was made today, ' .
     'you will probably need to wait until tomorrow to issue a refund.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Only show the refund form if the payment was made through a payment gateway that can handle refunds
  if (isset($donation->gateway)) { // Set during fundraiser_donation_get_donation().
    $can_refund = FALSE;
    if (!empty($donation->gateway['allow_refund']) && in_array($donation->donation['payment_method'], $donation->gateway['allow_refund'])) {
      $can_refund = TRUE;
    }
    $able_to_refund = $donation->donation['amount'] - $refunded_total;
    if (!$can_refund || $able_to_refund <= 0) {
      // They've made a payment, but it cannot be refunded
      if ($able_to_refund <= 0) {
        $no_refund_text = t('As many refunds as can be made on this donation have been made.');
      }
      elseif (!$can_refund) {
        $no_refund_text = t('Refunds cannot be made on this order. The payment was made on a payment ' .
         'gateway that does not support refunds.');
      }
      // They have NOT made any payment yet, so there's nothing to refund
      else {
        $no_refund_text = t('No payments have been processed on this order, so no refunds can be made yet.');
        // Even once the payment is made, they won't be able to refund it
        if (!$can_refund) {
          $no_refund_text .= ' ' . t('However, because this order was made on a payment gateway that ' .
            'does not support refunds, you will not be able to make refunds after payments are processed ' .
            'on this order.');
        }
      }
      $form['refund_options']['no_refund_explanation'] = array(
        '#markup' => $no_refund_text,
        '#prefix' => '<strong>',
        '#suffix' => '</strong>',
      );
    }
    // Provide refund options
    else {
      $form['able_to_refund'] = array(
        '#type' => 'value',
        '#value' => $able_to_refund,
      );
      $form['refunded_total'] = array(
        '#type' => 'value',
        '#value' => $refunded_total,
      );
      $form['refund_options']['refund_type'] = array(
        '#title' => t('Refund options'),
        '#type' => 'radios',
        '#options' => array(
          'full' => t('Full Refund ( %amount )', array('%amount' => commerce_currency_format(commerce_currency_decimal_to_amount($able_to_refund, $donation->currency), $donation->currency))),
          'partial' => t('Partial Refund'),
        ),
        '#default_value' => isset($form_state['input']['refund_type']) ? $form_state['input']['refund_type'] : 'full',
      );
      $form['refund_options']['amount'] = array(
        '#type' => 'textfield',
        '#title' => t('Amount to Refund'),
        '#size' => 40,
        '#maxlength' => 255,
        '#states' => array(
          'required' => array(
            ':input[name="refund_type"]' => array('value' => 'partial'),
          ),
          'visible' => array(
            ':input[name="refund_type"]' => array('value' => 'partial'),
          ),
        ),
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
      $form['able_to_refund'] = array(
        '#type' => 'hidden',
        '#value' => number_format($able_to_refund, 2),
      );
      $form['refunded_total'] = array(
        '#type' => 'hidden',
        '#value' => number_format($refunded_total, 2),
      );
    }
  }
  else {
    $form['refund_options']['no_refund_explanation'] = array(
      '#markup' => t('The gateway for this refund is no longer available. A refund cannot be offered on this transaction.'),
      '#prefix' => '<strong>',
      '#suffix' => '</strong>',
    );
  }

  return $form;
}

/**
 * Validate the form
 */
function _fundraiser_refund_form_validate($form, &$form_state) {
  // Get the donation.
  $donation = fundraiser_donation_get_donation($form_state['donation_id']);

  // Variables for tracking the refund type and amounts.
  $refund_type = $form_state['values']['refund_type'];
  $able_to_refund = $form_state['values']['able_to_refund'];
  $refund_amount = 0;

  if (empty($refund_type)) {
    form_set_error('refund_type', t('Select a type for the refund.'));
  }

  // A full refund uses the $able_to_refund amount, a partial refund needs to validate the entered amount.
  if ($refund_type != 'full') {
    // Convert the entered amount and refund amount to whole numbers.
    $amount = commerce_currency_decimal_to_amount(_fundraiser_amount_value_sanitize($form_state['values']['amount']), $donation->currency);
    $able_to_refund = commerce_currency_decimal_to_amount(_fundraiser_amount_value_sanitize($able_to_refund), $donation->currency);
    if ($amount <= 0) {
      form_set_error('amount', t('Enter a positive numerical refund amount.'));
    }
    else {
      // Check if the amount set to refund is less than the amount left to refund.
      if ($amount > $able_to_refund) {
        $context = array(
          'revision' => 'formatted-original',
          'type' => 'amount',
        );
        form_set_error('amount', t('The refund amount entered is too high. Only @paid in payments have been made, and no ' .
          'more than that can be refunded.', array('@paid' => commerce_currency_format($able_to_refund, $donation->currency))));
      }
      else {
        // Add the amount to refund to the form_state values.
        $refund_amount = $amount;
      }
    }
  }
  else {
    // Add the full amount to the form_state values.
    $refund_amount = $able_to_refund;
  }

  $form_state['values']['refund_amount'] = commerce_currency_amount_to_decimal($refund_amount, $donation->currency);

  $refund_notes = $form_state['values']['refund_notes'];
  if (empty($refund_notes)) {
    form_set_error('refund_notes', t('Please enter a reason for the refund.'));
  }
}

/**
 * Submit the refund form
 */
function _fundraiser_refund_form_submit($form, &$form_state) {
  // Get the donation.
  $donation = fundraiser_donation_get_donation($form_state['donation_id']);

  // Determine how much has been paid, less previous refunds
  $refunded_total = (float) $form_state['values']['refunded_total'];
  $able_to_refund = (commerce_currency_decimal_to_amount($donation->donation['amount'], $donation->currency) - $refunded_total);

  // Partial refund.
  if ($form_state['values']['refund_type'] != 'full') {
    $amount = commerce_currency_decimal_to_amount($form_state['values']['refund_amount'], $donation->currency);

    $new_status = 'partially_refunded';
    // Prevent a refund of more than the amount paid.
    if ($amount >= $able_to_refund) {
      $amount = $able_to_refund;
      $new_status = 'refunded';
    }
  }
  // Full refund.
  else {
    $amount = $able_to_refund;
    $new_status = 'refunded';
  }

  // Set up the refund.
  $refund = new stdClass();
  $refund->did = $donation->did;
  $refund->amount = commerce_currency_amount_to_decimal($amount, $donation->currency);
  $refund->currency = $donation->currency;
  $refund->reason = $form_state['values']['refund_notes'];
  $refund->rid = time();  // Use a timestamp to avoid PayPal error 11607 (duplicate MSGSUBID value)
  $refund = fundraiser_refund_create($refund);
  // Then process it.
  $refund->new_status = $new_status;
  fundraiser_refund_process($refund);
  // At this point $donation->result is set and responses can be handled.
  if (!isset($refund->result['message'])) {
    $refund->result['message'] = '';
  }
  // The results should be at $donation->result, ready to respond to the aftermath.
  if (isset($refund->result['success']) && $refund->result['success'] == TRUE) {
    fundraiser_refund_success($refund);
  }
  else {
    $donation->result['success'] = FALSE;
    drupal_set_message(t('Your refund failed to be processed.'));
    fundraiser_refund_decline($refund);
    $form_state['rebuild'] = TRUE;
    $form_state['values']['abort'] = TRUE;
  }
}
/**
 * Refund processing functions.
 */

/**
 * Utility function, given a donation id - get all refunds.
 */
function fundraiser_refund_get_refunds_by_did($did, $refresh = FALSE) {
  // Pull from cached donation unless we are refreshing the cache.
  static $refunds;
  if (!$refresh && isset($refunds[$did])) {
    return $refunds[$did];
  }
  $refund_set = array();
  $found_refunds = _fundraiser_get_refunds_by_did($did);
  foreach ($found_refunds as $found_refund) {
    $refund_set[] = fundraiser_refund_get_refund($found_refund->rid);
  }
  // Store the refunds information into the cache by did.
  $refunds[$did] = $refund_set;
  return $refund_set;
}

/**
 * Create a refund object.
 */
function fundraiser_refund_create($refund) {
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  module_invoke_all('fundraiser_refund_create', $refund);
  $refund->receipt_id = 'no_receipt';
  $refund->status = 'unknown';
  $refund->txn_id = '';
  $refund->created = REQUEST_TIME;
  $refund->changed = REQUEST_TIME;
  // Create the record
  $refund = _fundraiser_create_refund($refund);
  // Refresh the cache. No need to change the donation object passed in, but stored values should be zapped.
  $refund = fundraiser_refund_get_refund($refund->rid, TRUE);
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_refund_post_create', $refund);
  return $refund;
}

/**
 * Update a refund object.
 */
function fundraiser_refund_update($refund) {
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  module_invoke_all('fundraiser_refund_update', $refund);
  $refund->receipt_id = isset($refund->receipt_id) ? $refund->receipt_id : 'no_receipt';
  $refund->changed = REQUEST_TIME;
  $refund = _fundraiser_update_refund($refund);
  // Refresh the cache. No need to change the donation object passed in, but stored values should be zapped.
  $refund = fundraiser_refund_get_refund($refund->rid, TRUE);
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_refund_post_update', $refund);
}

/**
 * Donation processing, a given donation needs to be refunded.
 */
function fundraiser_refund_process($refund) {
  // Pass this down to the processing module.
    module_invoke_all('fundraiser_refund_process', $refund);
  // And save the refund as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_refund_update($refund, FALSE);
  if (!empty($refund->donation)) {
    fundraiser_donation_update($refund->donation, FALSE);
  }
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_refund_post_process', $refund);
}

/**
 * Donation processing, a given refund was successful.
 */
function fundraiser_refund_success($refund) {
  drupal_set_message(t('Refund has been issued.'));
  // Allow other modules to respond to the refund success. Such as SF modules.
  module_invoke_all('fundraiser_refund_success', $refund);
  fundraiser_refund_update($refund, FALSE);
  if (!empty($refund->donation)) {
    fundraiser_donation_update($refund->donation, FALSE);
  }
}

/**
 * Donation processing, a given refund was declined.
 */
function fundraiser_refund_decline($refund) {
  watchdog('fundraiser', 'Refund on donation #%did failed gateway validation. Reason: %reason.',
    array('%did' => $refund->did, '%reason' => $refund->result['message']), WATCHDOG_DEBUG);
  drupal_set_message($refund->result['message'], 'error');
  // Allow other modules to respond to the refund decline.
  module_invoke_all('fundraiser_refund_decline', $refund);
  fundraiser_refund_update($refund, FALSE);
  if (!empty($refund->donation)) {
    fundraiser_donation_update($refund->donation, FALSE);
  }
}

/**
 * Utility function, given a refund id - get everything that goes with it.
 */
function fundraiser_refund_get_refund($rid, $refresh = FALSE) {
  // Pull from cached donation unless we are refreshing the cache.
  static $refunds;
  if (!$refresh && isset($refunds[$rid])) {
    return $refunds[$rid];
  }
  // If refreshing, or first load, pull all data from database.
  $refund = (object) array();
  $refund->rid = $rid;
  // Grab basic data from our own systems.
  $refund_record = _fundraiser_get_refund_by_rid($rid);
  // Store the refund information into the cache.
  $refunds[$rid] = $refund_record;
  return $refund_record;
}
