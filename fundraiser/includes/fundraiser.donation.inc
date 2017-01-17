<?php
/**
 * @file
 * Donation form functions.
 */

/**
 * Default front end validation rules.
 */
function _fundraiser_donation_form_js_validation_config($node) {
  // jQuery Validate rules are objects, typecast here to recreate the format.
  // Zipcode.
  $zip = (object) array(
    'required' => TRUE,
    'zipcode' => TRUE,
    'messages' => (object) array(
      'required' => t('This field is required.'),
      'zipcode' => t('Enter a valid zipcode.'),
    ),
  );

  // CVV.
  $card_cvv = (object) array(
    'required' => TRUE,
    'number' => TRUE,
    'minlength' => 3,
    'maxlength' => 4,
    'messages' => (object) array(
      'required' => t('This field is required.'),
      'number' => t('Must be a number.'),
      'minlength' => t('Minimum of 3 characters.'),
      'maxlength' => t('Maximum of 4 characters.'),
    ),
  );

  // Credit card.
  $card_number = (object) array(
    'required' => TRUE,
    'number' => TRUE,
    'creditcard' => TRUE,
    'messages' => (object) array(
      'required' => t('This field is required.'),
      'number' => t('Must be a number.'),
      'creditcard' => t('Enter a valid credit card number.'),
    ),
  );

  $settings = array(
    'zip' => $zip,
    'card_cvv' => $card_cvv,
    'card_number' => $card_number,
  );

  // Add validation to the amount textfield, which is only present
  // when no donation amounts are configured.
  if ($node->show_other_amount && count($node->donation_amounts) <= 1) {
    $settings['amount'] = array(
      'required' => TRUE,
      'number' => TRUE,
      'messages' => array(
        'required' => t('This field is required.'),
        'number' => t('Enter a valid amount.'),
      ),
    );
  }

  // Give 3rd party modules an opportunity to alter.
  drupal_alter('fundraiser_donation_form_js_validation', $settings, $node);

  return $settings;
}

/**
 * Donation form, display routine on data returned from glue display module.
 *
 * Must be called from glue display module when displaying the donation form.
 */
function fundraiser_donation_form(&$form, &$form_state) {
  $node = $form['#node'];

  // Attach the js files.
  $path = drupal_get_path('module', 'fundraiser');
  $form['#attached']['js'][] = $path . '/js/jquery.alphanumeric.min.js';
  $form['#attached']['js'][] = $path . '/js/jquery.validate.min.js';
  $form['#attached']['js'][] = $path . '/js/donation_validation.js';

  // Add the validation configuration settings array.
  $form_validation_js = _fundraiser_donation_form_js_validation_config($node);
  if (!empty($form_validation_js)) {
    $form['#attached']['js'][] = array(
      'data' => array('fundraiser' => array('js_validation_settings' => $form_validation_js)),
      'type' => 'setting',
    );
  }

  if (!empty($node->minimum_donation_amount)) {
    $min = array('minimum_donation_amount' => $node->minimum_donation_amount);
    $form['#attached']['js'][] = array(
      'data' => array('fundraiserWebform' => $min),
      'type' => 'setting',
    );
  }

  // Check to see if the form is being accessed over HTTPS.
  $is_secure = _fundraiser_is_secure();
  $development_mode = variable_get('fundraiser_development_mode', FALSE);
  if (!$is_secure && !$development_mode) {
    // Return a 404 if the page isn't secure and should be.
    watchdog('fundraiser', 'The donation form <em>@title</em> is not protected with SSL.',
      array('@title' => $node->title), WATCHDOG_CRITICAL, l(t('View the donation form'), 'node/' . $node->nid));
    $message = variable_get('fundraiser_http_error_message', t("We're sorry, but this donation form is experiencing a temporary problem. Please try again later."));
    // Remove all other form elements.
    foreach (element_children($form) as $child) {
      unset($form[$child]);
    }
    // Display the message.
    $form['fundraiser_http_error'] = array(
      '#type' => 'item',
      '#markup' => $message,
    );
  }
  else {
    // Prevent caching on this form, this will keep the back button operational.
    drupal_add_http_header('Cache-Control', 'no-cache, no-store, must-revalidate, post-check=0, pre-check=0');
    $form['#attributes']['class'][] = 'fundraiser-donation-form';
    // Add validation and submit routines for fundraiser to get called during
    // submission. Otherwise, do nothing, rely on the glue modules to provide
    // the form we need to the user. We add these first to make sure they're
    // available for the hook call.
    $form['#validate'][] = 'fundraiser_donation_validate';
    $form['#submit'][] = 'fundraiser_donation_submit';

    // When confirmations are moved, this goes elsewhere.
    $form['#submit'][] = 'fundraiser_donation_post_submit';

    // Make the currency available as a JS setting. Used by quantity to set a
    // total value.
    $currency = fundraiser_get_currency_from_node($node);
    $symbol = check_plain($currency['symbol']);
    $form['#attached']['js'][] = array(
      'data' => array('fundraiser' => array('currency' => $currency)),
      'type' => 'setting',
    );

    // Add class to quantity field (for legacy forms).
    dpm($form);
    if (!empty($form['submitted']['donation']['quantity'])) {
      $form['submitted']['donation']['quantity']['#attributes']['class'][] = 'input-medium';
    }

    // Add class to zip code field.
    if (!empty($zip)) {
      $form['submitted']['billing_information']['zip']['#attributes']['class'][] = 'input-medium';
    }

    // Change ask amount labels.
    if (!empty($form['submitted']['donation']['amount'])) {
      $amount = $form['submitted']['donation']['amount'];
      if (!empty($amount['#options'])) {
        foreach ($amount['#options'] as $val => $label) {
          $amount['#options'][$val] = str_replace('$', $symbol, $label);
        }
      }
      else {
        $amount['#field_prefix'] = $symbol;
      }
    }

    // Call hook to allow sub modules to add / manipulate the form.
    // NOTE: we are not using module_invoke_all on purpose here.
    // Node objects (common on webform objs) flag as recursive warnings.
    foreach (module_implements('fundraiser_donation_form') as $module) {
      $result = module_invoke($module, 'fundraiser_donation_form', $form, $form_state);
      if (isset($result) && is_array($result)) {
        $form = array_merge($form, $result);
      }
    }
  }
}

/**
 * Donation form, validation routine on data returned from glue display module.
 * Called from glue display module to validate additional data.
 */
function fundraiser_donation_validate(&$form, &$form_state) {
  // Call hook to validate donation values against fundraiser needs and sub module needs.
  // Each is meant to return a not ok response or nothing.
  // No response = we're all good, otherwise we return an array of values for handling.
  drupal_alter('fundraiser_donation_validate', $form, $form_state);
  $errors = module_invoke_all('fundraiser_donation_validate', $form, $form_state);
  // Then handle our own business.
  if (!empty($errors)) {
    // Call the related donation process function.
    fundraiser_donation_failed_validation($errors);
  }
}

/**
 * Donation form, submission routine on data returned from glue display module.
 * Called from glue display module to submit additional data.
 * NOTE: This is the right function to call repeatedly (with different data as needed)
 * if you need to submit n+ donations on any single submission.
 * Assumption in all of the fundraiser system is that one submission of a form = one donation.
 * So keep that in mind as you work, if you need to make more than one donation at a time
 * then it is better to falsify multiple submissions to the form than to try and shove more than
 * one donation processed per submission.
 */
function fundraiser_donation_submit(&$form, &$form_state) {
  // Create the donation object and load it with the submitted information for the donation.
  $donation = new stdClass();
  // Using user load to avoid user object being carried by ref around.
  global $user;
  $donation->uid = $user->uid;
  $donation->user = user_load($user->uid);
  // Load the node from the form.
  $donation->nid = $form['#node']->nid;
  $donation->node = $form['#node'];

  // Since this is a form submission set the reference charge value to FALSE
  $donation->reference_charge = FALSE;

  // Call hook to gather submission donation values for the donation.
  module_invoke_all('fundraiser_donation_submit', $form, $form_state, $donation);
  // Call hook to allow modules to alter the form or donation.
  drupal_alter('fundraiser_donation_submit', $form, $form_state, $donation);
  // $donation->donation now contains the submitted values from the form.
  // Once we have a submission we can get to the real meat of things and attempt to create it.
  // This calls our set of functions to process the donation from there.
  if (!empty($donation->donation)) {
    _fundraiser_donation_submit_to_process($form, $form_state, $donation);
    _fundraiser_donation_submit_after_process($donation);
  }
  else {
    // No submission was able to be found. So we really have nothing to go on.
    // Mark this as a failure and move on.
    $donation->result['success'] = FALSE;
  }
  // Add the donation and result info to the form_state so we can reference it later.
  $form_state['#donation'] = $donation;
}

/**
 * Helper function, given a donation object, central function to create, process, and manage results.
 * Provided for other modules to use as needed for redirection style processes.
 */
function _fundraiser_donation_submit_to_process($form, $form_state, $donation) {
  // Create the donation to process, allows other modules to modify results by hooks.
  fundraiser_donation_create($donation);
  $did = $donation->did;
  // Caching form values are provided ONLY for the purpose of handling redirect submissions.
  // It is intended to allow redirects to pick back up with form submission after returning to site.
  cache_set('fundraiser-form-id-' . $did, $form_state['values']['form_build_id'], 'cache', strtotime('+15 minutes'));
  // Cache the CURRENT version of the forms as well, as they have been processed up to this point.
  cache_set('fundraiser-form-' . $did, $form, 'cache_form', strtotime('+15 minutes'));
  cache_set('fundraiser-form-state-' . $did, $form_state, 'cache_form', strtotime('+15 minutes'));
  // The created donation information should be at at $donation->donation, ready to process.
  fundraiser_donation_process($donation);
}

/**
 * Helper function, given a donation object, central function to create, process, and manage results.
 * Provided for other modules to use as needed for redirection style processes.
 */
function _fundraiser_donation_submit_after_process($donation) {
  // Clear caches.
  cache_clear_all('fundraiser-form-id-' . $donation->did, 'cache');
  cache_clear_all('fundraiser-form-' . $donation->did, 'cache_form');
  cache_clear_all('fundraiser-form-state-' . $donation->did, 'cache_form');

  // Finish donation processing.
  if (!isset($donation->result['message'])) {
    $donation->result['message'] = '';
  }
  // The results should be at $donation->result, ready to respond to the aftermath.
  if (isset($donation->result['success']) && $donation->result['success']) {
    fundraiser_donation_success($donation);
  }
  else {
    $donation->result['success'] = FALSE;
    fundraiser_donation_decline($donation);
  }
}

/**
 * Submit callback after donation submitted, but before confirmation.
 *
 * Provides a hook alter to allow for sub modules to change behavior after
 * submission. Such as in cases of failures or the like.
 */
function fundraiser_donation_post_submit(&$form, &$form_state) {
  drupal_alter('fundraiser_donation_post_submit', $form, $form_state, $form_state['#donation']);
}

/**
 * Donation processing functions.
 */

/**
 * Donation processing, a given donation needs to be commented on.
 */
function fundraiser_donation_comment($donation, $comment, $variables = array(), $severity = WATCHDOG_NOTICE, $link = NULL) {
  // Make a log entry
  if (variable_get('fundraiser_log_comments', 0)) {
    watchdog('fundraiser_comment', $comment, $variables, $severity, $link);
  }
  // Allow other modules to respond to the cancel. Including Ubercart, etc.
  module_invoke_all('fundraiser_donation_comment', $donation, $comment, $variables, $severity, $link);
}

/**
 * Given a donation id, gather information to display for that donation.
 * To be displayed when needed by other modules.
 */
function fundraiser_donation_information_table($did) {
  $rows = array();
  $donation = fundraiser_donation_get_donation($did);
  if (!$donation) {
    return;
  }

  $rows[] = array('Fundraiser user', t('User #@uid - !name',
    array('@uid' => $donation->uid, '!name' => l($donation->user->name, 'user/' . $donation->user->uid))));
  if (isset($donation->node->nid)) {
    $rows[] = array('Fundraiser node', t('Node #@nid - !title',
      array('@nid' => $donation->nid, '!title' => l($donation->node->title, 'node/' . $donation->node->nid))));
  }
  else {
    $rows[] = array('Fundraiser node', t('Node #@nid - !title',
      array('@nid' => '---', '!title' => t('No fundraiser node found.'))));
  }
  $variables = array(
    'header' => array(),
    'rows' => $rows,
  );
  drupal_alter('fundraiser_donation_information_table', $variables, $donation);
  return theme('table', $variables);
}

/**
 * Donation processing, a given donation failed to validate.
 */
function fundraiser_donation_failed_validation($errors) {
  // Handle our own tracking.
  _fundraiser_update_tracking_value($errors['nid'], 'local_failures');
  // Call hook to allow sub modules to respond to the failure as well.
  module_invoke_all('fundraiser_donation_failed_validation', $errors);
}

/**
 * Donation processing, a donation needs to be created prior to processing.
 */
function fundraiser_donation_create($donation) {
  // The gateway is selected by the user, was set during submission. Same as nid, uid, sid etc.
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  // This is a good location to update $donation->data for processing.
  module_invoke_all('fundraiser_donation_create', $donation);
  // For now, our creation system is responsible for creating a did.
  // Track the did, nid, uid internally so we can keep things in order.
  // Grab the submission path, if there is no alias, use the standard node path.
  global $base_url;
  $alias = drupal_lookup_path('alias', $_GET['q']);
  $form_url = '';
  if (!empty($alias)) {
    $form_url = $base_url . '/' . $alias;
  }
  else {
    $form_url = $base_url . '/node/' . $donation->nid;
  }

  $donation_record = array(
    // Donation id.
    'did' => $donation->did,
    // Fundraiser node id.
    'nid' => $donation->nid,
    // Submission user id.
    'uid' => $donation->uid,
    // Submission id.
    'sid' => $donation->sid,
    'status' => $donation->status,
    'amount' => $donation->amount,
    'currency' => $donation->currency,
    'txn_id' => '',
    'form_url' => $form_url,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
    'card_id' => !empty($donation->data['cardonfile']) ? $donation->data['cardonfile'] : NULL,
  );

  if (is_string($donation->gateway)) {
    $donation_record['gateway'] = $donation->gateway;
  }
  elseif (is_array($donation->gateway) && !empty($donation->gateway['id'])) {
    $donation_record['gateway'] = $donation->gateway['id'];
  }

  _fundraiser_create_donation($donation_record);

  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was created on @date by @username.', array('@date' => format_date(strtotime('now')), '@username' => $username));

  // Respond after the creation of a donation. (Insert essentially)
  // At this point an order has been created by the charging module and etc.
  module_invoke_all('fundraiser_donation_post_create', $donation);
}

/**
 * Donation processing, a donation needs to be updated with new information prior to processing.
 * For example, in the course of updating a sustainers information.
 */
function fundraiser_donation_update($donation, $vocal_mode = TRUE) {
  // Call hooks to create the donation. The glue module handles everything else once handed fundraisers info.
  module_invoke_all('fundraiser_donation_update', $donation);
  // The additional fields $donation->additional, ready to process.
  // For now, our creation system is responsible for creating a did.
  // Track the did, nid, uid internally so we can keep things in order.
  $fundraiser_node = node_load($donation->nid);
  $donation_record = array('did' => $donation->did);
  if (isset($donation->nid) && !empty($donation->nid)) {
    $donation_record['nid'] = $donation->nid;
  }
  if (isset($donation->uid) && !empty($donation->uid)) {
    $donation_record['uid'] = $donation->uid;
  }
  if (isset($donation->sid) && !empty($donation->sid)) {
    $donation_record['sid'] = $donation->sid;
  }
  if (isset($donation->status) && !empty($donation->status)) {
    $donation_record['status'] = $donation->status;
  }
  if (isset($donation->amount) && !empty($donation->amount)) {
    $donation_record['amount'] = $donation->amount;
  }
  elseif (!empty($donation->donation['amount'])) {
    $donation_record['amount'] = $donation->donation['amount'];
    // was a quantity (of > 1) specified? (legacy forms)
    if (isset($donation->donation['quantity']) && $donation->donation['quantity'] > 1) {
      // multiply base donation amount by quantity to get the donation total
      $donation_record['amount'] = number_format($donation->donation['amount'] * $donation->donation['quantity'], 2);
    }
  }
  if (isset($donation->currency) && !empty($donation->currency)) {
    $donation_record['currency'] = $donation->currency;
  }
  if (isset($donation->gateway) && !empty($donation->gateway)) {
    if (is_string($donation->gateway)) {
      $donation_record['gateway'] = $donation->gateway;
    }
    elseif (is_array($donation->gateway) && isset($donation->gateway['id']) && !empty($donation->gateway['id'])) {
      $donation_record['gateway'] = $donation->gateway['id'];
    }
  }
  if (isset($donation->txn_id) && !empty($donation->txn_id)) {
    $donation_record['txn_id'] = $donation->txn_id;
  }
  if (isset($donation->form_url) && !empty($donation->form_url)) {
    $donation_record['form_url'] = $donation->form_url;
  }
  if (!empty($donation->data['cardonfile'])) {
    $donation_record['card_id'] = $donation->data['cardonfile'];
  }
  $donation_record['changed'] = REQUEST_TIME;
  _fundraiser_update_donation($donation_record);
  // Refresh the cache. No need to change the donation object passed in, but stored values should be zapped.
  fundraiser_donation_get_donation($donation->did, TRUE);
  // Add a comment.
  if ($vocal_mode) {
    global $user;
    $username = isset($user->name) ? $user->name : 'Anonymous';
    fundraiser_donation_comment($donation, 'This donation was updated on @date by @username.',
      array('@date' => format_date(strtotime('now')), '@username' => $username));
  }
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_donation_post_update', $donation);
}

/**
 * Copy donation object without references to the original object
 */
function _fundraiser_donation_copy($donation) {
  return unserialize(serialize($donation));
}

/**
 * Donation processing, a given donation needs to be processed.
 */
function fundraiser_donation_process($donation) {
  // Create an array of values for the log.
  $log = array(
    'start' => microtime(TRUE),
    'transaction_type' => 'charge',
  );

  // Pass this down to the processing module.
  // Note, if an implementation of this hook redirects users offsite (like
  // PayPal WPS), the lines below this one may not execute.
  module_invoke_all('fundraiser_donation_process', $donation);

  // Mark the end time of the processing.
  $log['end'] = microtime(TRUE);

  // Log the processing details.
  fundraiser_log_donation_processing($donation, $log);

  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Respond after the update of a donation.
  module_invoke_all('fundraiser_donation_post_process', $donation);
}

/**
 * Donation processing, a given donation was successful.
 */
function fundraiser_donation_success($donation) {
  // Track the success.
  _fundraiser_update_tracking_value($donation->nid, 'conversions');
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message && isset($donation->result['message'])) {
    drupal_set_message($donation->result['message']);
  }
  // Allow other modules to respond to the success.
  module_invoke_all('fundraiser_donation_success', $donation);
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was successfully completed on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
}

/**
 * Donation processing, a given donation was declined.
 */
function fundraiser_donation_decline($donation) {
  // Track this failure.
  _fundraiser_update_tracking_value($donation->nid, 'gateway_failures');
  // Make a log entry.
  watchdog('fundraiser', 'Donation @id was declined by the payment gateway. Reason: @message',
    array('@id' => $donation->did, '@message' => $donation->result['message']), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message($donation->result['message']);
  }
  // Allow other modules to respond to the decline.
  module_invoke_all('fundraiser_donation_decline', $donation);
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was declined at the gateway on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
}

// TODO how does this even get called? Is this functionally different from a decline?
/**
 * Donation processing, a given donation experienced an exception.
 */
function fundraiser_donation_exception($donation) {
  // Make a log entry.
  watchdog('fundraiser', 'An error occurred while processing donation @id. Error: @message',
    array('@id' => $donation->did, '@message' => $donation->result['message']), WATCHDOG_ERROR, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message($donation->result['message']);
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation encountered an exception on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Allow other modules to respond to the exception.
  module_invoke_all('fundraiser_donation_exception', $donation);
}

/**
 * Donation processing, a given donation needs to be cancelled.
 * Generally, only available for Authorize.net gateway donations. For example, during sustainer call.
 */
function fundraiser_donation_cancel($donation) {
  // Make a log entry
  watchdog('fundraiser', 'Donation @id was canceled by the user.',
    array('@id' => $donation->did), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message(t('Your payment #:did has been canceled.', array(':did' => $donation->did)));
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was cancelled on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));

  // Allow other modules such as Ubercart to respond to cancellation and
  // and update the commernce order status to reflect cancellation so that
  // the fundraiser_donation table's status will be correct when it is updated.
  module_invoke_all('fundraiser_donation_cancel', $donation);

  // Finally, save changes to the donation, updating this donation's status in the
  // fundraiser_donation table to reflect the status set within the commerce_order table:
  fundraiser_donation_update($donation, FALSE);
}

/**
 * Donation processing, a given donation needs to be deleted.
 * Generally, not called, but it does happen.
 */
function fundraiser_donation_delete($donation) {
  // Make a log entry
  watchdog('fundraiser', 'Donation @id was deleted.',
    array('@id' => $donation->did), WATCHDOG_DEBUG, NULL);
  // Show gateay message if appropriate.
  $display_message = variable_get('fundraiser_gateway_messages', 1);
  if ($display_message) {
    drupal_set_message(t('Your payment #:did has been deleted.', array(':did' => $donation->did)));
  }
  // Add a comment.
  global $user;
  $username = isset($user->name) ? $user->name : 'Anonymous';
  fundraiser_donation_comment($donation, 'This donation was deleted on @date by @username.',
    array('@date' => format_date(strtotime('now')), '@username' => $username));
  // And save the donation as it is at this stage since it was created.
  // This makes sure that any changes made stay in place.
  fundraiser_donation_update($donation, FALSE);
  // Allow other modules to respond to the cancel. Including Ubercart, etc.
  module_invoke_all('fundraiser_donation_delete', $donation);
}

/**
 * Utility function, given a donation id - get everything that goes with it.
 *
 * @param int $did
 *   The ID of the donation
 * @param bool $refresh
 *   If TRUE rebuild the entire donation even if it's statically cached.
 *
 * @return object|bool
 *    The fully loaded donation object or FALSE if no donation is found
 */
function fundraiser_donation_get_donation($did, $refresh = FALSE, $donation_record = NULL) {
  // Pull from cached donation unless we are refreshing the cache.
  static $donations;
  if (!$refresh && isset($donations[$did])) {
    return $donations[$did];
  }

  // Grab basic data from our own systems.
  if (empty($donation_record)) {
    $donation_record = _fundraiser_get_donation_by_did($did);
  }
  if (!$donation_record) {
    return FALSE;
  }

  // If refreshing, or first load, pull all data from database.
  $donation = (object) array();
  $donation->did = $did;
  $donation->nid = $donation_record->nid;
  $donation->node = node_load($donation_record->nid);
  $donation->uid = $donation_record->uid;
  $donation->user = user_load($donation_record->uid);
  $donation->sid = $donation_record->sid;
  // Additional fields.
  $donation->status = $donation_record->status;
  $donation->amount = $donation_record->amount;
  $donation->currency = $donation_record->currency;
  $donation->created = $donation_record->created;
  $donation->changed = $donation_record->changed;

  // Grab gateway information and replace it in the node too if found.
  $gateway = _fundraiser_get_donation_gateway($did);
  if (isset($gateway['id'] )) {
    $donation->gateway = $gateway;
  }
  // Grab the transaction id if it exists, add it here.
  $donation->txn_id = isset($donation_record->txn_id) ? $donation_record->txn_id : '';
  $donation->form_url = $donation_record->form_url;
  // And has refund grab them too
  $donation->refunds = FALSE;
  $donation->refunds = fundraiser_refund_get_refunds_by_did($did, TRUE);

  // Call hooks to re-create the donation info.
  // The glue modules handle everything else once handed fundraisers info.
  module_invoke_all('fundraiser_donation_get_donation', $donation);
  // Store the donation information into the cache.
  $donations[$did] = $donation;

  return $donation;
}

/**
 * Provide a menu router autoloader wrapper for fundraiser_donation_get_donation().
 */
function fundraiser_donation_load($did) {
  return fundraiser_donation_get_donation($did);
}

/**
 * Get the first donation ID that matches the given submission ID.
 *
 * Be aware that multiple donations may have the same submission ID.
 * For example, recurring donations will have the initial sid.
 *
 * @param int $sid
 *   The submission ID.
 *
 * @return int|bool
 *   The donation ID, or FALSE.
 */
function fundraiser_donation_get_did_by_sid($sid) {
  return db_query("SELECT MIN(did) FROM {fundraiser_donation} WHERE sid = :sid", array(':sid' => $sid))
    ->fetchField();
}
