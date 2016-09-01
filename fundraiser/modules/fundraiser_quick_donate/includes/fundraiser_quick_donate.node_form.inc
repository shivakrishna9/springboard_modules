<?php

/**
 * @file
 * Node form related hooks for Springboard Quick Donate.
 */

/**
 * Implements hook_form_alter().
 */
function fundraiser_quick_donate_form_node_form_alter(&$form, &$form_state, $form_id) {
  if (empty($form['#node']->type) || !fundraiser_is_donation_type($form['#node']->type)) {
    return;
  }

  // If this is an appropriate node edit/add form, add the "quick donation"
  // checkbox to the "Payment methods" fieldset.
  if (empty($form['payment_methods']['gateways']) || !element_get_visible_children($form['payment_methods']['gateways'])) {
    return;
  }

  $form['payment_methods']['quickdonate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable quick donations on this form?'),
    '#description' => t('When enabled, the "quick donation" option will be offered to users, which saves their payment information for faster payments in the future.'),
    '#default_value' => isset($form['#node']->quickdonate) ? $form['#node']->quickdonate : FALSE,
  );

  if (!count(variable_get(FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX . 'default', array()))) {
    $form['payment_methods']['quickdonate']['#suffix'] = t('<strong>Warning:</strong> since no default form is set on the !config_page, a user might not end up in the right place when coming from an HMAC URL.<br/><br/>', array(
      '!config_page' => l(t('Quick Donate configuration page'), 'springboard/options/fundraiser/fundraiser_quick_donate'),
    ));
  }

  $usable_paypment_processors = array('credit' => array(), 'bank' => array());
  if (!empty($form['payment_methods']['gateways']['credit'])) {
    foreach ($form['payment_methods']['gateways']['credit']['id']['#options'] as $gateway => $value) {
      // Check to see if the currently selected payment method is Card on
      // File enabled (via the cardonfile boolean in its info array).
      $payment_method = commerce_payment_method_instance_load($gateway);

      // Skip if payment method is not capable of card on file.
      if (!_commerce_cardonfile_capable_payment_method_check($payment_method) || !isset($payment_method['settings']['cardonfile']) || (isset($payment_method['settings']['cardonfile']) && !$payment_method['settings']['cardonfile'])) {
        continue;
      }

      // For onsite payment methods add a checkbox to the credit card
      // details container to store the credit card for future use.
      if (!$payment_method['offsite']) {
        $usable_paypment_processors['credit'][] = $gateway;
      }
    }
  }

  if (!empty($form['payment_methods']['gateways']['bank account'])) {
    foreach ($form['payment_methods']['gateways']['bank account']['id']['#options'] as $gateway => $value) {
      // Check to see if the currently selected payment method is Card on
      // File enabled (via the cardonfile boolean in its info array).
      $payment_method = commerce_payment_method_instance_load($gateway);

      // Skip if payment method is not capable of card on file.
      if (!_commerce_cardonfile_capable_payment_method_check($payment_method)) {
        continue;
      }

      // For onsite payment methods add a checkbox to the credit card
      // details container to store the credit card for future use.
      if (!$payment_method['offsite']) {
        $usable_paypment_processors['bank'][] = $gateway;
      }
    }
  }

  if (empty($usable_paypment_processors['credit']) && empty($usable_paypment_processors['bank'])) {
    // If no usable payment processors are available, disable the checkbox
    // and display a note to the user.
    $form['payment_methods']['quickdonate']['#attributes']['disabled'] = TRUE;
    $form['payment_methods']['quickdonate']['#suffix'] = '<span class="note"><strong>Note:</strong> no usable payment processors were found, so quick donations will not be available.</span>';
  }
  else {
    drupal_add_js(array(
      'fundraiser_quick_donate' => array(
        'usable_paypment_processors' => $usable_paypment_processors,
      ),
    ), 'setting');
    $form['payment_methods']['quickdonate_optin_message'] = array(
      '#type' => 'textfield',
      '#title' => t('"Quick Donation" opt-in text'),
      '#description' => t('This is the title of the opt-in checkbox. The "@type" placeholder can be used to specify the type of payment method (either credit card or bank account). Defaults to "Save this @type for future use".'),
      '#default_value' => isset($form['#node']->quickdonate_optin_message) ? $form['#node']->quickdonate_optin_message : variable_get('fundraiser_quick_donate_default_optin_message', t('Save this @type for future use')),
      '#states' => array(
        'visible' => array(
          ':input[name="quickdonate"]' => array('checked' => TRUE),
        ),
      ),
    );

    // Because a bug exists in D7 where the input filter help information
    // doesn't respect the '#states' property, we need to wrap the message
    // in a container and apply the #states property to the container
    // instead.
    // See https://www.drupal.org/node/997826 for more info.
    $form['payment_methods']['quickdonate_help_container'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="quickdonate"]' => array('checked' => TRUE),
        ),
      ),
    );
    $form['payment_methods']['quickdonate_help_container']['quickdonate_help_message'] = array(
      '#type' => 'text_format',
      '#title' => t('"Quick Donation" help text'),
      '#description' => t('This message will be shown to users to provide information about quick donations. Use !type to print the type of payment method, which will be either credit or bank account.<br/><br/>', array('!type' => '<strong>@type</strong>')),
      '#default_value' => isset($form['#node']->quickdonate_help_message) ? $form['#node']->quickdonate_help_message : variable_get('fundraiser_quick_donate_default_help_message', t('Save your @type details and donate faster next time. We may email you from time to time with quick donate opportunities. You can update your @type details or cancel your enrollment at any time.')),
      '#format' => isset($form['#node']->quickdonate_help_message_format) ? $form['#node']->quickdonate_help_message_format : 'plain_text',
    );

    $form['payment_methods']['quickdonate_login_message'] = array(
      '#type' => 'textfield',
      '#title' => t('"Quick Donation" login text'),
      '#description' => t('This is the title of the log-in checkbox. The "@type" placeholder can be used to specify the type of payment method (credit card or bank account). Defaults to "Already have a saved @type?".'),
      '#default_value' => isset($form['#node']->quickdonate_login_message) ? $form['#node']->quickdonate_login_message : variable_get('fundraiser_quick_donate_default_login_message', t('Already have a saved @type?')),
      '#states' => array(
        'visible' => array(
          ':input[name="quickdonate"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['payment_methods']['quickdonate_login_link_message'] = array(
      '#type' => 'textfield',
      '#title' => t('"Quick Donation" login link text'),
      '#description' => t('This is the title of the log-in checkbox. Defaults to "Click here to login".'),
      '#default_value' => isset($form['#node']->quickdonate_login_link_message) ? $form['#node']->quickdonate_login_link_message : variable_get('fundraiser_quick_donate_default_login_link_message', t('Click here to login')),
      '#states' => array(
        'visible' => array(
          ':input[name="quickdonate"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $form['actions']['submit']['#submit'][] = '_fundraiser_quick_donate_node_form_submit';
  $form['#attached']['js'][] = drupal_get_path('module', 'fundraiser_quick_donate') . '/js/fundraiser_quick_donate.js';
}

/**
 * Submission handler for node forms.
 *
 * Restructures array before it's passed to hook insert/update functions.
 */
function _fundraiser_quick_donate_node_form_submit($form, &$form_state) {
  if ($form_state['values']['quickdonate']) {
    // Quick Donate checkbox is checked, save QD settings.
    $fields = array(
      'fundraiser_nid' => $form_state['nid'],
      'quickdonate_optin_message' => $form_state['values']['quickdonate_optin_message'],
      'quickdonate_help_message_format' => $form_state['values']['quickdonate_help_message']['format'],
      'quickdonate_help_message' => $form_state['values']['quickdonate_help_message']['value'],
      'quickdonate_login_message' => $form_state['values']['quickdonate_login_message'],
      'quickdonate_login_link_message' => $form_state['values']['quickdonate_login_link_message'],
    );

    db_merge('fundraiser_quick_donate')
      ->key(array('fundraiser_nid' => $fields['fundraiser_nid']))
      ->fields($fields)
      ->execute();
  }
  else {
    // Not checked, delete any entries.
    db_delete('fundraiser_quick_donate')->condition('fundraiser_nid', $form_state['nid'], '=')->execute();
  }
}