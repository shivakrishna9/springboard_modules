<?php
/**
 * @file
 * Functions for building out donation forms.
 */

/**
 * Donation form, display routine on data returned from glue display module.
 *
 * Must be called from glue display module when displaying the donation form.
 */
function fundraiser_donation_form(&$form, &$form_state) {
  $node = $form['#node'];
  // Load any custom configuration options.
  $config = db_query('SELECT * FROM {fundraiser_webform_messages} f WHERE f.nid = :nid', array(':nid' => $form['#node']->nid))->fetchAssoc();

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

  if (isset($node->minimum_donation_amount)) {
    $min = array('minimum_donation_amount' => $node->minimum_donation_amount);
    $form['#attached']['js'][] = array(
      'data' => array('fundraiserWebform' => $min),
      'type' => 'setting',
    );
  }

  // Check to see if the form is being accessed over HTTPS.
  global $is_https;
  if ((!$is_https && !variable_get('fundraiser_development_mode', 0))) {
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

    // Apply currency data to the form.
    $currency = fundraiser_get_currency_from_node($node);
    $symbol = check_plain($currency['symbol']);

    // Make the currency available as a JS setting.
    // Used by quantity to set a Total value.
    $form['#attached']['js'][] = array(
      'data' => array('fundraiser' => array('currency' => $currency)),
      'type' => 'setting',
    );

    // Add class to quantity field (for legacy forms).
    $quantity = _fundraiser_get_form_field($form, 'quantity');
    if (!empty($quantity)) {
      $quantity['#attributes']['class'][] = 'input-medium';
      $form = _fundraiser_update_form_field($form, 'quantity', $quantity);
    }

    // Add class to zip code field.
    $zip = _fundraiser_get_form_field($form, 'zip');
    $zip['#attributes']['class'][] = 'input-medium';
    $form = _fundraiser_update_form_field($form, 'zip', $zip);

    // Change ask amount labels.
    $amount = _fundraiser_get_form_field($form, 'amount');
    if (isset($amount) && !empty($amount)) {
      if (isset($amount['#options'])) {
        foreach ($amount['#options'] as $val => $label) {
          $amount['#options'][$val] = str_replace('$', $symbol, $label);
        }
      }
      else {
        $amount['#field_prefix'] = $symbol;
      }
    }
    $form = _fundraiser_update_form_field($form, 'amount', $amount);

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
 * Default front end validation rules.
 */
function _fundraiser_donation_form_js_validation_config($node) {
  // jQuery Validate rules are objects, typecast here to recreate the format.
  // Zipcode validate.
  $zip = (object) array(
    'required' => TRUE,
    'zipcode' => TRUE,
    'messages' => (object) array(
      'required' => t('This field is required.'),
      'zipcode' => t('Enter a valid zipcode.'),
    ),
  );

  // CVV validation.
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

  // Credit card validation.
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
  // Add validation to the amount textfield, which is only present when no
  // donation amounts are configured.
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
 * Donation form, validation routine on data returned from glue display module.
 *
 * Called from glue display module to validate additional data.
 */
function fundraiser_donation_validate(&$form, &$form_state) {
  // Call hook to validate donation values against fundraiser needs and sub
  // module needs. Each is meant to return a not ok response or nothing. No
  // response means we're all good, otherwise we return an array of values for
  // handling.
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
  // Create the donation object and load it with the submitted information for
  // the donation.
  $donation = new stdClass();
  // Using user load to avoid user object being carried by ref around.
  global $user;
  $donation->uid = $user->uid;
  $donation->user = user_load($user->uid);
  // Load the node from the form.
  $donation->nid = $form['#node']->nid;
  $donation->node = $form['#node'];

  // Since this is a form submission set the reference charge value to FALSE.
  $donation->reference_charge = FALSE;

  // Call hook to gather submission donation values for the donation.
  module_invoke_all('fundraiser_donation_submit', $form, $form_state, $donation);
  // Call hook to allow modules to alter the form or donation.
  drupal_alter('fundraiser_donation_submit', $form, $form_state, $donation);
  // $donation->donation now contains the submitted values from the form.
  // Once we have a submission we can get to the real meat of things and attempt
  // to create it. This calls our set of functions to process the donation from
  // there.
  if (!empty($donation->donation)) {
    _fundraiser_donation_submit_to_process($form, $form_state, $donation);
    _fundraiser_donation_submit_after_process($donation);
  }
  else {
    // No submission was able to be found. So we really have nothing to go on.
    // Mark this as a failure and move on.
    $donation->result['success'] = FALSE;
  }
  // Add the donation and result info to the form_state so we can reference it
  // later.
  $form_state['#donation'] = $donation;
}

/**
 * Helper function to create, process, and manage results.
 *
 * Provided for other modules to use as needed for redirection style processes.
 */
function _fundraiser_donation_submit_to_process($form, $form_state, $donation) {
  // Create the donation to process, allows other modules to modify results by
  // hooks.
  fundraiser_donation_create($donation);
  $did = $donation->did;
  // Caching form values are provided ONLY for the purpose of handling redirect
  // submissions. It is intended to allow redirects to pick back up with form
  // submission after returning to site.
  cache_set('fundraiser-form-id-' . $did, $form_state['values']['form_build_id'], 'cache', strtotime('+15 minutes'));
  // Cache the CURRENT version of the forms as well, as they have been processed
  // up to this point.
  cache_set('fundraiser-form-' . $did, $form, 'cache_form', strtotime('+15 minutes'));
  cache_set('fundraiser-form-state-' . $did, $form_state, 'cache_form', strtotime('+15 minutes'));
  // The created donation information should be at at $donation->donation, ready
  // to process.
  fundraiser_donation_process($donation);
}

/**
 * Helper function to create, process, and manage results.
 *
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
  // The results should be at $donation->result, ready to respond to the
  // aftermath.
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
