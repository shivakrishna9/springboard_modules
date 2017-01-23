<?php
/**
 * @file
 * Configuration for fields that are added to fundraiser enabled content types.
 */

/**
 * Add fields needed by fundraiser to a node type.
 *
 * @param string $type
 *   The content type to check if the fundraiser fields exist,
 *   and if not the fields are added to the type.
 */
function _fundraiser_add_default_fields($type) {
  $fields = _fundraiser_default_fields_settings($type);
  foreach ($fields as $field_name => $field) {
    // These should be added on each page request, but to be sure do it again.
    module_load_include('inc', 'field', 'field.crud');
    module_load_include('inc', 'field', 'field.info');

    // If the field doesn't exist create it.
    if (!field_info_field($field_name)) {
      field_create_field($field['field_config']);
    }
    // Before adding the field check that it does not exist on the node type.
    if (!field_info_instance('node', $field_name, $type)) {
      field_create_instance($field['field_instance']);
    }
  }
}

/**
 * Returns an array of field configurations.
 *
 * This is copied from a features export.
 *
 * @param string $type
 *    The type of node these fields will be added to,
 *    used to populate the bundle property.
 */
function _fundraiser_default_fields_settings($type) {
  $fields = array();

  $fields['field_fundraiser_internal_name'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_fundraiser_internal_name',
      'foreign keys' => array(
        'format' => array(
          'columns' => array(
            'format' => 'format',
          ),
          'table' => 'filter_format',
        ),
      ),
      'indexes' => array(
        'format' => array(
          0 => 'format',
        ),
      ),
      'locked' => '0',
      'module' => 'text',
      'settings' => array(
        'max_length' => '255',
      ),
      'translatable' => '0',
      'type' => 'text',
    ),
    'field_instance' => array(
      'bundle' => $type,
      'default_value' => NULL,
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => '2',
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'field_fundraiser_internal_name',
      'label' => 'Internal Name',
      'required' => 1,
      'settings' => array(
        'text_processing' => '0',
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'size' => '60',
        ),
        'type' => 'text_textfield',
        'weight' => '0',
      ),
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Internal Name');

  $fields['field_fundraiser_currency'] = array(
    'field_config' => fundraiser_currency_field_definition(),
    'field_instance' => fundraiser_currency_field_instance($type),
  );

  t('Default currency');

  return $fields;
}

/**
 * The currency field definition.
 *
 * @return array
 *   The field definition for the currency fundraiser field.
 */
function fundraiser_currency_field_definition() {
  return array(
    'translatable' => '0',
    'entity_types' => array(),
    'settings' => array(
      'allowed_values_function' => 'fundraiser_currency_field_get_currency_options',
    ),
    'storage' => array(
      'type' => 'field_sql_storage',
      'settings' => array(),
      'module' => 'field_sql_storage',
      'active' => '1',
      'details' => array(
        'sql' => array(
          'FIELD_LOAD_CURRENT' => array(
            'field_data_field_default_currency' => array(
              'value' => 'field_fundraiser_currency_value',
            ),
          ),
          'FIELD_LOAD_REVISION' => array(
            'field_revision_field_default_currency' => array(
              'value' => 'field_fundraiser_currency_value',
            ),
          ),
        ),
      ),
    ),
    'foreign keys' => array(),
    'indexes' => array(
      'value' => array('value'),
    ),
    'field_name' => 'field_fundraiser_currency',
    'type' => 'list_text',
    'module' => 'list',
    'active' => '1',
    'locked' => '0',
    'cardinality' => '1',
    'deleted' => '0',
    'columns' => array(
      'value' => array(
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
      ),
    ),
  );
}

/**
 * Provides the currency field instance configuration.
 *
 * @param string $bundle_name
 *   The name of the bundle to add the instance to.
 *
 * @return array
 *   The currency field instance array with included bundle name.
 */
function fundraiser_currency_field_instance($bundle_name) {
  return array(
    'label' => 'Default currency',
    'widget' => array(
      'weight' => '2',
      'type' => 'options_select',
      'module' => 'options',
      'active' => 1,
      'settings' => array(),
    ),
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'display' => array(
      'default' => array(
        'type' => 'hidden',
        'label' => 'above',
        'settings' => array(),
        'weight' => 4,
      ),
      'teaser' => array(
        'type' => 'hidden',
        'label' => 'above',
        'settings' => array(),
        'weight' => 0,
      ),
    ),
    'required' => 1,
    'description' => 'The default currency for this donation form.',
    'default_value' => array(
      array('value' => commerce_default_currency()),
    ),
    'field_name' => 'field_fundraiser_currency',
    'entity_type' => 'node',
    'bundle' => $bundle_name,
    'deleted' => '0',
  );
}

/**
 * Provides a list of fields to be defined by the display module.
 * Requests additional field information from other modules that need to define them.
 */
function _fundraiser_field_info($refresh = FALSE) {
  $fields = &drupal_static(__FUNCTION__, array());
  // Cache fundraiser content types during page execution.
  if (empty($fields) || $refresh == TRUE) {
    // Request additional fields from other modules.
    $fields = module_invoke_all('fundraiser_field_info');
    // Allow for others to modify as needed.
    drupal_alter('fundraiser_field_info', $fields);
  }
  // And return.
  return $fields;
}

/**
 * Implements hook_fundraiser_field_info().
 */
function fundraiser_fundraiser_field_info() {
  // Donation amounts are handled in a special case.
  $fields['donation'] = array(
    '#title' => t('Donation'),
    '#type' => 'fieldset',
    '#required' => 0,
  );
  $fields['donation']['amount'] = array(
    '#title' => t('Amount'),
    '#type' => 'textfield',
    '#required' => 1,
    '#extra' => array(
      'description' => '',
      'width' => 10,
      'field_prefix' => '$',
    ),
    '#allow_update' => 1,
    '#reset_value' => 1,
    '#create_callback' => '_fundraiser_amount_field_create',
  );
  $fields['donation']['other_amount'] = array(
    '#title' => t('Other'),
    '#type' => 'textfield',
    '#required' => 0,
    '#extra' => array(
      'description' => t('Minimum payment $ @min.', array(
        '@min' => number_format(variable_get('fundraiser_default_minimum', 10.00), 2),
      )),
      'width' => 10,
      'field_prefix' => '$',
    ),
    '#allow_update' => 1,
    '#create_callback' => '_fundraiser_other_amount_field_create',
    '#validate_callback' => '_fundraiser_other_amount_field_validate',
    '#display_callback' => '_fundraiser_other_amount_field_display',
  );

  $fields['donation']['currency'] = array(
    '#title' => 'Currency',
    '#type' => 'hidden',
    '#value' => '',
    '#required' => 1,
    '#allow_update' => 1,
    '#reset_value' => 1,
    '#create_callback' => '_fundraiser_currency_field_create',
    '#display_callback' => '_fundraiser_currency_field_display',
  );

  $fields['donor_information'] = array(
    '#title' => t('Your Information'),
    '#type' => 'fieldset',
    '#required' => 0,
  );
  $fields['donor_information']['first_name'] = array(
    '#title' => t('First Name'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['donor_information']['last_name'] = array(
    '#title' => t('Last Name'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['donor_information']['mail'] = array(
    '#title' => t('E-mail address'),
    '#type' => 'email',
    '#required' => 1,
    '#validate_callback' => '_fundraiser_email_field_validate',
  );
  $fields['billing_information'] = array(
    '#title' => t('Billing Information'),
    '#type' => 'fieldset',
    '#required' => 0,
  );
  $fields['billing_information']['address'] = array(
    '#title' => t('Address'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['billing_information']['address_line_2'] = array(
    '#title' => t('Address Line 2'),
    '#type' => 'textfield',
    '#required' => 0,
  );
  $fields['billing_information']['city'] = array(
    '#title' => t('City'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['billing_information']['country'] = array(
    '#title' => t('Country'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['billing_information']['state'] = array(
    '#title' => t('State/Province'),
    '#type' => 'textfield',
    '#required' => 1,
  );
  $fields['billing_information']['zip'] = array(
    '#title' => t('ZIP/Postal Code'),
    '#type' => 'textfield',
    '#required' => 1,
    '#extra' => array(
      'description' => '',
      'width' => 10,
      'maxlength' => 10,
    ),
  );
  $fields['payment_information'] = array(
    '#title' => t('Payment Information'),
    '#type' => 'fieldset',
    '#required' => 0,
  );

  /**
   * These two fields are intended to hold the payment method information and
   * the array of user entered data for the payment method. They replace
   * previous implementations relying on CC fields.
   */
  $fields['payment_information']['payment_method'] = array(
    '#title' => t('Payment Method'),
    '#type' => 'payment_method',
    '#required' => 1,
    '#allow_update' => 1,
    '#create_callback' => '_fundraiser_payment_method_field_create',
  );
  $fields['payment_information']['payment_fields'] = array(
    '#title' => t('Payment Details'),
    '#type' => 'payment_fields',
    '#required' => 0,
    /**
     * Instead of returning the value of this field it's treated more like a
     * variable fieldset. The values returned are dependant on the value
     * returned by payment method by gateway.
     * In the format:
     * 'payment_fields' => array(
     *   'credit' => array(
     *     'card_number' => '',
     *     'card_expiration_month' => '',
     *     'card_expiration_year' => '',
     *     'card_cvv' => '',
     *   )
     *   'paypal' => array(
     *     'paypal_email' => '',
     *   )
     *   'debit' => array(
     *     'debit_routing' => '',
     *     'debit_account' => '',
     *   )
     * );
     */
  );
  return $fields;
}

/**
 * Field callback to update the field with fundraiser specific settings.
 */
function _fundraiser_amount_field_create($fundraiser, $field) {
  $currency = fundraiser_get_currency_from_node($fundraiser);
  $currency_symbol = check_plain($currency['symbol']);

  // If donation amounts don't exist, use amount as is.
  if (empty($fundraiser->donation_amounts) || !is_array($fundraiser->donation_amounts)) {
    // Show the minimum amount when only displaying an amount text box.
    $field['#extra']['description'] = 'Minimum payment ' . $currency_symbol . number_format($fundraiser->minimum_donation_amount, 2) . '.';
    return $field;
  }
  // Else we want a select field.
  if (is_array($fundraiser->donation_amounts)) {
    // Since we have an array of donation_amounts, set the field data to match.
    $keys = array_keys($fundraiser->donation_amounts);
    // Construct a set of options based on the configured amounts.
    $formatted_amounts = '';
    foreach ($fundraiser->donation_amounts as $value => $label) {
      if (empty($label)) {
        $label = $currency_symbol . $value;
      }
      $formatted_amounts[] = implode('|', array($value, $label));
    }
    if ($fundraiser->show_other_amount && count($fundraiser->donation_amounts)) {
      // Strip out other other|Other combiantions and make sure this is at the end.
      $formatted_amounts = array_diff($formatted_amounts, array('other|Other'));
      $formatted_amounts[] = 'other|Other';
    }
    $formatted_amounts = implode("\n", $formatted_amounts);
    $field['#title'] = t('Please select your tax-deductible gift amount below');
    $field['#type'] = 'select';
    $field['#extra'] = array(
      'description' => '',
      'items' => $formatted_amounts,
      'multiple' => 0,
    );
    $default = $keys[0];
    if (empty($fundraiser->default_amount)) {
      $default = '';
    }
    else if (in_array($fundraiser->default_amount, $keys)) {
      // Default is one of the defined ask amounts. Use it, but make sure the formatting matches the ask amount exactly.
      foreach ($keys as $ask_amount) {
        if ($ask_amount == $fundraiser->default_amount) {
          $default = $ask_amount;
          break;
        }
      }
    }

    // Extra settings may get clobbered in fundraiser_webform. if we don't flag them.
    // See "extra config" comment under _fundraiser_webform_update_component().
    if (!empty($field['#extra'])) {
      $field['#node_extra_settings'] = $field['#extra'];
    }

    $field['#value'] = $default;

  }

  return $field;
}

/**
 * Field callback, called at runtime to update the field with fundraiser specific settings during creation.
 */
function _fundraiser_other_amount_field_create($fundraiser, $field) {
  // If donation amounts don't exist, drop the amount_other field, and use amount as is.
  if (empty($fundraiser->donation_amounts) || !is_array($fundraiser->donation_amounts)) {
    return FALSE; // FALSE as opposed to empty, means we delete this field if it exists.
  }
  // If the show other options is disallowed, unset it.
  if (!$fundraiser->show_other_amount) {
    return FALSE; // FALSE as opposed to empty, means we delete this field if it exists.
  }
  // Update the min with fundraiser's min.
  $field['#extra']['description'] = $field['#node_extra_settings']['description'] =
    'Minimum payment [currency-symbol]' . number_format($fundraiser->minimum_donation_amount, 2) . '.';

  // Return the field to be created.
  return $field;
}

/**
 * Fundraiser field display callback for the other amount field.
 */
function _fundraiser_other_amount_field_display($form, $form_state, $field) {
  // Get the currency, either from a current donation or the node.
  if (!empty($form['#donation'])) {
    $currency = $form['#donation']->donation['currency'];
  }
  else {
    $currency = fundraiser_get_currency_from_node($form['#node']);
  }

  $field['#attributes']['class'][] = 'input-medium';
  $field['#field_prefix'] = $currency['symbol'];

  // Replace the currency-symbol token.
  if (isset($field['#description'])) {
    $field['#description'] = str_replace('[currency-symbol]', $currency['symbol'], $field['#description']);
  }

  return $field;
}

/**
 * Validation callback for field other_amount. Called during form validation.
 */
function _fundraiser_other_amount_field_validate($form, $form_state, $submission_fields, $value) {
  // Check for confusing selection.
  if (!empty($submission_fields['amount']) && $submission_fields['amount'] != "other" && !empty($submission_fields['other_amount'])) {
    return array(
      'amount' => t('You have entered a custom amount and selected a set amount. Please clarify which amount you ' .
        'intend to give, if you want to give the amount that appears in the other box, please select Other from ' .
        'the radio buttons.'),
      'other_amount' => '',
    );
  }
  $value = $submission_fields['amount'];
  // Look for other amount if set.
  if ($submission_fields['amount'] == 'other') {
    $value = $submission_fields['other_amount'];
  }
  // Check for valid value.
  if (!preg_match('/^\d*(\.\d*)?$/', $value)) {
    return array('other_amount' => t('Donation amount must be in a valid number format. No commas and only one decimal point.'));
  }
  // Check for minimum amount.
  $fundraiser = _fundraiser_get_fundraiser_by_nid($form['#node']->nid);
  $minimum_donation_amount = isset($fundraiser->minimum_donation_amount) ?
    $fundraiser->minimum_donation_amount :  variable_get('fundraiser_default_minimum', 10.00);
  $minimum_donation_amount = number_format($minimum_donation_amount, 2);
  if ($value < $minimum_donation_amount) {
    return array('other_amount' => t('Your donation amount must be greater than or equal to @min_amount.',
      array('@min_amount' => $minimum_donation_amount)));
  }
}

/**
 * Fundraiser field create callback.
 *
 * Sets the currency value from the node.
 */
function _fundraiser_currency_field_create($fundraiser, $field) {
  $currency = fundraiser_get_currency_from_node($fundraiser);
  $field['#value'] = $currency['code'];

  return $field;
}

/**
 * Fundraiser field display callback.
 *
 * Note this field is a value type, so not really displayed.
 */
function _fundraiser_currency_field_display($form, $form_state, $field) {
  // Get the currency, either from a current donation or the node.
  if (!empty($form['#donation'])) {
    $currency = $form['#donation']->donation['currency'];
  }
  else {
    $currency = fundraiser_get_currency_from_node($form['#node']);
  }

  $field['#value'] = $currency['code'];
  return $field;
}

/**
 * Validation callback for field email. Called during form validation.
 */
function _fundraiser_email_field_validate($form, $form_state, $submission_fields, $value) {
  if (!_fundraiser_validate_email(trim($value))) {
    return array('mail' => t('You must enter a valid email address.'));
  }
}

/**
 * Field callback, called at runtime to update the field with fundraiser specific settings during creation.
 */
function _fundraiser_payment_method_field_create($fundraiser, $field) {
  // If donation amounts don't exist, use amount as is.
  if (empty($fundraiser->gateways)) {
    // Leave it alone, do nothing else, don't overwrite existing values.
    $field['#allow_update'] = 0;
    return $field;
  }
  // Set all the fields for the gateways based on the configuration.
  $field['#extra'] = array(
    'payment_methods' => unserialize($fundraiser->gateways),
  );
  return $field;
}

/**
 * Asks the display glue module(s) for field information.
 * This allows other modules to find a field as needed in a form array.
 * For example - for retrieveing the AJAX return values of a form.
 */
function fundraiser_get_form_field($calling_module, $form, $field_key) {
  // Get the form element from the given form.
  if (module_hook($calling_module, 'fundraiser_get_form_field')) {
    return module_invoke($calling_module, 'fundraiser_get_form_field', $form, $field_key);
  }
  else {
    // Assume the form is a regularly constructed form and find it in a child somewhere.
    return _fundraiser_get_form_field($form, $field_key);
  }
}

/**
 * Asks the display glue module(s) to update field information.
 * This allows other modules to update a field as needed in a form array.
 */
function fundraiser_update_form_field($calling_module, $form, $field_key, $new_field) {
  // Save the changed form element.
  if (module_hook($calling_module, 'fundraiser_update_form_field')) {
    return module_invoke($calling_module, 'fundraiser_update_form_field', $form, $field_key, $new_field);
  }
  else {
    return _fundraiser_update_form_field($form, $field_key, $new_field);
  }
}

/**
 * There is no need for a creation function here to handle creating fields in a hook.
 * That is something the display glue module should be able to handle on it's own
 * given access to the callback information. No reason to put that in the central module.
 */

/**
 * Process through given submission fields to change their display form on the fly. Recursive.
 * Calling module provides the identity of the display glue module. Usually webform, could be sustainers.
 */
function fundraiser_display_form_fields($calling_module, &$form, $form_state, $field_info = NULL) {
  if ($field_info == NULL) {
    $field_info = _fundraiser_field_info();
  }
  $children = element_children($field_info);
  foreach ($children as $child) {
    if (isset($field_info[$child]['#display_callback']) && function_exists($field_info[$child]['#display_callback'])) {
      // Get the field, update it, and put it back.
      $this_field = fundraiser_get_form_field($calling_module, $form, $child);
      $new_field = call_user_func($field_info[$child]['#display_callback'], $form, $form_state, $this_field);
      $form = fundraiser_update_form_field($calling_module, $form, $child, $new_field);
    }
    // Check for any further children.
    fundraiser_display_form_fields($calling_module, $form, $form_state, $field_info[$child]);
  }
}

/**
 * Process through given submission fields to validate on the fly. Recursive.
 * This handles the calls to additional validation callbacks as defined in the field_info array.
 * May be called by any module that needs to perform validation on a submitted array. See sustainers.
 *
 * @return bool
 *   TRUE if there are validation errors.
 */
function fundraiser_validate_form_fields($form, $form_state, $submission_fields, $field_info = NULL, $errors = FALSE) {
  // Grab field info, we'll use this all the way down.
  if ($field_info == NULL) {
    $field_info = _fundraiser_field_info();
    drupal_alter('fundraiser_field_info_validate', $field_info, $form);
  }
  // Check for errors by field info array.
  $children = element_children($field_info);
  foreach ($children as $child) {
    $keys[$child] = '';
    // Check only if the submission field is set.
    // And the additional validate callback exists.
    if (isset($submission_fields[$child]) && isset($field_info[$child]['#validate_callback'])
       && function_exists($field_info[$child]['#validate_callback'])) {
      $error_messages = call_user_func($field_info[$child]['#validate_callback'], $form, $form_state,
        $submission_fields, $submission_fields[$child]);
      if (isset($error_messages) && !empty($error_messages)) {
        foreach ($error_messages as $form_field => $error_message) {
          // Set the error.
          $this_field = _fundraiser_get_form_field($form, $form_field);
          $parents = implode('][', $this_field['#parents']);
          form_set_error($parents, $error_message); // Coder: This is ok.
          $errors = TRUE;
        }
      }
    }
    // Recurse down.
    $errors = fundraiser_validate_form_fields($form, $form_state, $submission_fields, $field_info[$child], $errors);
  }
  return $errors;
}

/**
 * Given a standard form array, recursively locate a given key and return it.
 */
function _fundraiser_get_form_field($form, $field_key) {
  foreach (element_children($form) as $child) {
    if ($child == $field_key) {
      return $form[$child];
    }
    else {
      // Check this child for other children.
      $found = _fundraiser_get_form_field($form[$child], $field_key);
      if (!empty($found)) {
        return $found;
      }
    }
  }
}

/**
 * Given a standard (non webform, nothing fancy, form array. Locate a given key and update it.
 * Recursive.
 */
function _fundraiser_update_form_field($form, $field_key, $new_field) {
  // Walks a given form looking for the given key. Returns it when found.
  foreach (element_children($form) as $child) {
    if ($child == $field_key) {
      // Update the array.
      $form[$child] = array_merge($form[$child], $new_field);
    }
    else {
      // Check this child for other children.
      $form[$child] = _fundraiser_update_form_field($form[$child], $field_key, $new_field);
    }
  }
  return $form;
}

/**
 * Helper functions.
 */

/**
 * Helper function, given the array from field_info - get the listed keys.
 * WARNING: Recursion is here to walk the field info array to gather form keys.
 */
function _fundraiser_get_field_keys($field_info = NULL, $keys = array()) {
  if ($field_info == NULL) {
    $field_info = _fundraiser_field_info();
  }
  $children = element_children($field_info);
  $keys = array_merge($keys, $children);
  foreach ($children as $child) {
    if ($field_info[$child] != NULL && !empty($field_info[$child])) {
      $keys = _fundraiser_get_field_keys($field_info[$child], $keys);
    }
  }
  return $keys;
}

/**
 * Helper function, given the array from field_info - get the listed fields.
 * WARNING: Recursion is here to walk the field info array to gather fields.
 */
function _fundraiser_get_field_fields($field_info = NULL, $fields = array()) {
  if ($field_info == NULL) {
    $field_info = _fundraiser_field_info();
  }
  $children = element_children($field_info);
  foreach ($children as $child) {
    $fields[$child] = $field_info[$child];
  }
  foreach ($children as $child) {
    if ($field_info[$child] != NULL && !empty($field_info[$child])) {
      $fields = _fundraiser_get_field_fields($field_info[$child], $fields);
    }
  }
  return $fields;
}
