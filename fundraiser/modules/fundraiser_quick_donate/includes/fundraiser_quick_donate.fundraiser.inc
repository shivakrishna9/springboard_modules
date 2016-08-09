<?php
/**
 * @file
 * Fundraiser related hooks for Fundraiser Quick Donate.
 */

/**
 * Implements hook_fundraiser_donation_form().
 *
 * Adds "quick pay"-related fields to the form.
 */
function fundraiser_quick_donate_fundraiser_donation_form($form, $form_state) {
  // Exit if this isn't the right node type.
  if (empty($form['#node']->type) || !fundraiser_is_donation_type($form['#node']->type)) {
    return;
  }

  // Exit if the quick donation option isn't enabled on this form.
  if (empty($form['#node']->quickdonate) || !$form['#node']->quickdonate) {
    return;
  }

  // Exit if no payment method instance id.
  if (empty($form['#node']->gateways)) {
    return;
  }

  global $user;
  // Allow other modules to alter the user object before we load their stored
  // cards.
  drupal_alter('fundraiser_quick_donate_fundraiser_donation_form_user', $user);

  // Load user's stored cardonfile cards for each gateway.
  foreach ($form['#node']->gateways as $gateway_type => $gateway) {
    if (empty($gateway['status']) || empty($gateway['id'])) {
      continue;
    }

    // Extract payment method instance id.
    $instance_id = $gateway['id'];

    // Check to see if the currently selected payment method is Card on File
    // enabled (via the cardonfile boolean in its info array).
    $payment_method = commerce_payment_method_instance_load($instance_id);

    // Skip if payment method is not capable of card on file.
    if (!_commerce_cardonfile_capable_payment_method_check($payment_method)) {
      continue;
    }

    // This variable stores the type of payment method, and will be shown to the
    // end user.
    // @todo Wrap in t()?
    $type = 'card';
    if ($gateway_type == 'bank account') {
      // @todo What should this be?
      $type = 'bank account';
    }

    $form['submitted']['payment_information']['payment_fields'][$gateway_type]['cardonfile_store'] = array(
      '#type' => 'value',
      '#value' => 'true',
    );
    $form['submitted']['payment_information']['payment_fields'][$gateway_type]['quickdonate'] = array(
      '#type' => 'checkbox',
      '#title' => isset($form['#node']->quickdonate_optin_message) ? format_string($form['#node']->quickdonate_optin_message, array('@type' => $type)) : format_string(variable_get('fundraiser_quick_donate_default_optin_message', 'Save this @type for future use?'), array('@type' => $type)),
      '#default_value' => FALSE,
      '#description' => isset($form['#node']->quickdonate_help_message) ? check_markup(format_string($form['#node']->quickdonate_help_message, array('@type' => $type)), $form['#node']->quickdonate_help_message_format) : format_string(variable_get('fundraiser_quick_donate_default_help_message', 'Save your @type details and donate faster next time. We may email you from time to time with quick donate opportunities. You can update your @type details or cancel your enrollment at any time.'), array('@type' => $type)),
    );

    // Load existing active cards for the payment method instance and user.
    $stored_cards = commerce_cardonfile_load_multiple_by_uid($user->uid, $payment_method['instance_id']);

    // The ID of the cardonfile record specified for quick donations.
    $quick_donate_card = _fundraiser_quick_donate_get_payment_info_for_uid($user->uid);

    // Build options form.
    $cardonfile_options_form = array();
    $instance_default_card_id = NULL;

    // If have stored cards ...
    if (!empty($stored_cards)) {
      _fundraiser_quick_donate_checkout_form_hide_payment_fields($form['submitted']['payment_information']['payment_fields'][$gateway_type]);

      $valid_cards = array_filter($stored_cards, 'commerce_cardonfile_validate_card_expiration');

      // If have un-expired cards...
      if (!empty($valid_cards)) {
        // Get options list with labels.
        $card_option_element_type = variable_get('commerce_cardonfile_selector', 'radios');
        $card_options = commerce_cardonfile_element_options_list($valid_cards, $card_option_element_type);

        // Determine default option.
        $card_options_default_value = key($card_options);
        foreach (array_keys($card_options) as $card_id) {
          if (isset($valid_cards[$card_id]) &&
            // If this card is the quick donate card.
            ($quick_donate_card && $quick_donate_card == $card_id)
            // Or if the quick donate card wasn't set and this is the default
            // cardonfile card.
            || (!$quick_donate_card && !empty($valid_cards[$card_id]->instance_default))
          ) {
            $card_options_default_value = $instance_default_card_id = $card_id;

            // Move instance default to the top of the list.
            $card_option_label = $card_options[$card_id];
            unset($card_options[$card_id]);
            $card_options = array($card_id => $card_option_label) + $card_options;
            break;
          }
        }

        // Create options element.
        $cardonfile_options_form = array(
          '#type' => $card_option_element_type,
          '#title' => t('Select a stored card'),
          '#options' => $card_options,
          '#default_value' => $card_options_default_value,
          '#weight' => -10,
        );
      }
    }

    // If the quick donation cardonfile ID returned was FALSE, set it to the
    // default instance.
    if (!$quick_donate_card) {
      $quick_donate_card = $instance_default_card_id;
    }

    // Update form with options.
    if (!empty($cardonfile_options_form)) {
      $form['submitted']['payment_information']['payment_fields'][$gateway_type]['cardonfile'] = $cardonfile_options_form;

      // Add the CSS to hide a sole credit card icon if specified.
      if (variable_get('commerce_cardonfile_hide_cc_radio_button', TRUE)) {
        if (count($form['submitted']['payment_information']['payment_method']['#options']) == 1) {
          $form['submitted']['payment_information']['payment_method']['#attached']['css'][] = drupal_get_path('module', 'commerce_cardonfile') . '/theme/commerce_cardonfile.checkout.css';
        }
      }

      // If the current value for the card selection element is not to use
      // a different credit card, then hide the credit card form elements.
      if (empty($form_state['values']['submitted_tree']['payment_information']['payment_fields'][$gateway_type]['cardonfile']) || $form_state['values']['commerce_payment']['payment_details']['cardonfile'] !== 'new') {
        $form['submitted']['payment_information']['payment_fields'][$gateway_type]['credit_card']['#access'] = FALSE;
      }
    }
    else {
      $form['submitted']['payment_information']['payment_fields'][$gateway_type]['cardonfile'] = array(
        '#type' => 'value',
        '#value' => 'new',
      );
    }

    // Add mark as default element.
    $instance_default_default_value = 0;
    if (!empty($instance_default_card_id)) {
      if (empty($form_state['values']['submitted_tree']) ||
        (!empty($form_state['values']['submitted_tree']['payment_information']['payment_fields'][$gateway_type]['cardonfile']) &&
        $form_state['values']['submitted_tree']['payment_information']['payment_fields'][$gateway_type]['cardonfile'] == $instance_default_card_id)) {
        $instance_default_default_value = 1;
      }
    }

    // If the following variable is TRUE, the following form element will be
    // checked and disabled (ie it WILL be set as default).
    $force_instance_default = empty($stored_cards);
    $form['submitted']['payment_information']['payment_fields'][$gateway_type]['cardonfile_instance_default'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set as your default payment method for all future donations?'),
      '#default_value' => $instance_default_default_value || $force_instance_default,
      '#disabled' => $force_instance_default,
      '#states' => array(
        'visible' => array(
          ':input[name$="[' . $gateway_type . '][cardonfile]"]' => array('value' => 'new'),
          ':input[name$="[' . $gateway_type . '][quickdonate]"]' => array('checked' => TRUE),
        ),
      ),
    );

    // If the user isn't logged in, present a message asking if they already
    // have an account and stored card, and to log in.
    if (!user_is_logged_in()) {
      $login_text = isset($form['#node']->quickdonate_login_message) ? $form['#node']->quickdonate_login_message : variable_get('fundraiser_quick_donate_default_login_message', t('Already have a saved @type?'));
      $login_link_text = isset($form['#node']->quickdonate_login_link_message) ? $form['#node']->quickdonate_login_link_message : variable_get('fundraiser_quick_donate_default_login_link_message', t('Click here to login'));
      $form['submitted']['payment_information']['payment_fields'][$gateway_type]['quickdonate']['#suffix'] = format_string($login_text, array(
        '@type' => $type,
      )) . ' ' . l($login_link_text, '/user', array(
        'query' => array(
          'destination' => current_path(),
        ),
      ));
    }
  }

  // Add submit handler.
  $form['#submit'][] = '_fundraiser_quick_donate_checkout_form_submit';

  // Allow others to alter this form.
  drupal_alter('fundraiser_quick_donate_fundraiser_donation_form', $form, $form_state);

  return $form;
}

/**
 * Implements hook_fundraiser_quick_donate_fundraiser_donation_form_alter().
 */
function fundraiser_quick_donate_fundraiser_quick_donate_fundraiser_donation_form_alter(&$form, &$form_state) {
  $parameters = drupal_get_query_parameters();
  if (isset($parameters['amount'])) {
    $amount = $parameters['amount'] / 100;
    $donation_field = &$form['submitted']['donation'];
    if (array_key_exists($amount, $donation_field['amount']['#options'])) {
      $donation_field['amount']['#default_value'] = $amount;
    }
    else {
      $donation_field['amount']['#default_value'] = 'other';
      $donation_field['other_amount']['#value'] = $amount;
    }
  }

  if (isset($parameters['ms'])) {
    $form['submitted']['ms']['#default_value'] = $form['submitted']['ms']['#webform_component']['value'] = $parameters['amount'];
  }

  if (isset($parameters['cid'])) {
    $form['submitted']['cid']['#default_value'] = $form['submitted']['cid']['#webform_component']['value'] = $parameters['cid'];
  }

  if (!empty($form['submitted']['payment_information']['payment_fields']['credit'])) {
    $form['submitted']['payment_information']['payment_fields']['credit']['quickdonate']['#value'] = TRUE;
  }
  if (!empty($form['submitted']['payment_information']['payment_fields']['bank'])) {
    $form['submitted']['payment_information']['payment_fields']['bank account']['quickdonate']['#value'] = TRUE;
  }
}

/**
 * Returns a user's quick donation default payment information if it exists.
 *
 * @param int $uid
 *   The user's ID.
 *
 * @return int
 *   The user's quick donation payment information.
 */
function _fundraiser_quick_donate_get_payment_info_for_uid($uid) {
  $query = db_select('fundraiser_quick_donate_cardonfile', 'fqdc')
    ->fields('fqdc', array('card_id'))
    ->condition('uid', $uid, '=')
    ->execute();
  if (!$query->rowCount()) {
    return FALSE;
  }

  return $query->fetchField();
}

/**
 * Helper function to traverse a form array and set #states for the QD form.
 */
function _fundraiser_quick_donate_checkout_form_hide_payment_fields(&$element) {
  $children = element_get_visible_children($element);
  foreach ($children as $child) {
    if (!isset($element[$child]['#states'])) {
      $element[$child]['#states'] = array();
    }

    $element[$child]['#states']['visible'][':input[name$="[cardonfile]"]'] = array('value' => 'new');

    $grandchildren = element_get_visible_children($element[$child]);
    if (count($grandchildren)) {
      _fundraiser_quick_donate_checkout_form_hide_payment_fields($element[$child]);
    }
  }
}

/**
 * Implements hook_fundraiser_donation_submit_alter().
 */
function fundraiser_quick_donate_fundraiser_donation_submit_alter($form, $form_state, &$donation) {
  $payment_info = $form_state['values']['submitted_tree']['payment_information'];
  $payment_fields = $payment_info['payment_fields'];
  if (isset($payment_fields['token']) && $payment_fields['token']) {
    $donation->data['cardonfile'] = $payment_fields[$payment_info['payment_method']]['cardonfile'];
    $card = commerce_cardonfile_load($donation->data['cardonfile']);
    $donation->data['is_quick_donate'] = TRUE;
    if (!empty($card)) {
      $donation->donation['payment_fields'][$payment_info['payment_method']]['card_number'] = $card->card_number;
      $donation->donation['payment_fields'][$payment_info['payment_method']]['card_type'] = $card->card_type;
      $donation->donation['payment_fields'][$payment_info['payment_method']]['card_expiration_month'] = $card->card_exp_month;
      $donation->donation['payment_fields'][$payment_info['payment_method']]['card_expiration_year'] = $card->card_exp_year;
      $donation->donation['payment_fields'][$payment_info['payment_method']]['quickdonate'] = TRUE;
    }
  }
}

/**
 * Implements hook_fundraiser_commerce_submit_handler_pane_values_alter().
 *
 * Sets the appropriate 'cardonfile' element to the card id value if set, which
 * is necessary for payment gateways to determine when to use cardonfile
 * functionality.
 */
function fundraiser_quick_donate_fundraiser_commerce_submit_handler_pane_values_alter(&$pane_values, &$donation) {
  if (isset($donation->data['cardonfile']) && $donation->data['cardonfile'] != 'new' && $donation->data['cardonfile'] > 0) {
    $pane_values['cardonfile'] = $donation->data['cardonfile'];
  }
  if (isset($_SESSION['springboard_hmac']['quickdonate']) && $_SESSION['springboard_hmac']['quickdonate']) {
    unset($_SESSION['springboard_hmac']['quickdonate']);
  }
}

/**
 * Submission handler for fundraiser_quick_donate_fundraiser_donation_form().
 */
function _fundraiser_quick_donate_checkout_form_submit($form, &$form_state) {
  if (empty($form_state['#donation'])) {
    return;
  }

  $values = &$form_state['values']['submitted_tree']['payment_information'];
  $method = $values['payment_method'];
  $payment_fields = $values['payment_fields'];
  $quickdonate = !empty($payment_fields[$method]['quickdonate']);

  // Exit if no card selection.
  if (!$quickdonate || empty($values['payment_fields'][$method]['cardonfile'])) {
    // They've elected not to opt-in to quick donations.
    return;
  }

  $donation = $form_state['#donation'];
  $account = $donation->user;

  $card_id = 0;

  if (!empty($payment_fields[$method]['cardonfile_instance_default'])) {
    if ($payment_fields[$method]['cardonfile'] != 'new') {
      // Get card on file value.
      $card_id = intval($payment_fields[$method]['cardonfile']);
      // Submit actions for card selected.
      if (!empty($card_id)) {
        // Mark as default.
        commerce_cardonfile_set_default_card($card_id);
      }
    }
    else {
      $card_id = _fundraiser_quick_donate_users_latest_cardid($account->uid);
      commerce_cardonfile_set_default_card($card_id);
    }
  }

  // In the case that the card id is still set to 0, retrieve user's latest
  // card on file.
  if ($card_id == 0) {
    $card_id = _fundraiser_quick_donate_users_latest_cardid($account->uid);
    // If no card was found, return early.
    if ($card_id === FALSE) {
      return;
    }
  }

  db_merge('fundraiser_quick_donate_cardonfile')
    ->key(array('uid' => $account->uid))
    ->fields(array(
      'card_id' => $card_id,
      'optin_date' => time(),
      'uid' => $account->uid,
      'did' => $donation->did,
      'nid' => $donation->nid,
      'gateway' => $donation->gateway['id'],
      'ms' => $form_state['values']['submitted_tree']['ms'],
      'cid' => $form_state['values']['submitted_tree']['cid'],
      'status' => 1,
    ))
    ->execute();

  module_invoke_all('springboard_quick_donate_optin_create', $account, $donation, $card_id);

  if (empty($account->pass)) {
    // Assuming this user account was just created since the password is empty.
    drupal_mail('user', 'register_no_approval_required', $account->mail, LANGUAGE_NONE, array('account' => $account), NULL);
  }
}

/**
 * Implements hook_fundraiser_donation_validate_alter().
 *
 * Used to add a new array element 'token' if a tokenized payment method (like
 * cardonfile) is being used.
 */
function fundraiser_quick_donate_fundraiser_donation_validate_alter($form, &$form_state) {
  $payment_info = &$form_state['values']['submitted']['payment_information'];
  if (isset($payment_info['payment_fields'][$payment_info['payment_method']]['cardonfile']) && $payment_info['payment_fields'][$payment_info['payment_method']]['cardonfile'] != 'new') {
    $payment_info['payment_fields']['token'] = TRUE;
  }
}

/**
 * Helper function to retrieve the newest card id belonging to specific user.
 */
function _fundraiser_quick_donate_users_latest_cardid($uid = NULL) {
  if (empty($uid)) {
    global $user;
    $uid = $user->uid;
  }

  $query = db_select('commerce_cardonfile', 'cc')
    ->fields('cc', array('card_id'))
    ->condition('uid', $uid, '=')
    ->orderBy('card_id', 'DESC')
    ->range(0, 1)
    ->execute();
  if ($query->rowCount() == 0) {
    return FALSE;
  }

  return $query->fetchField();
}