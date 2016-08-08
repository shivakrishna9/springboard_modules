<?php
/**
 * @file
 * Various field functions for the Other Amount field.
 */

/**
 * Callback to update a field with fundraiser specific settings during creation.
 */
function _fundraiser_other_amount_field_create($fundraiser, $field) {
  // If donation amounts don't exist, drop the amount_other field, and use
  // amount as is.
  if (empty($fundraiser->donation_amounts) || !is_array($fundraiser->donation_amounts)) {
    // FALSE as opposed to empty, means we delete this field if it exists.
    return FALSE;
  }
  // If the show other options is disallowed, unset it.
  if (!$fundraiser->show_other_amount) {
    // FALSE as opposed to empty, means we delete this field if it exists.
    return FALSE;
  }
  // Update the min with fundraiser's min.
  $field['#extra']['description'] = $field['#node_extra_settings']['description'] = t('Minimum payment [currency-symbol]@amount.', array(
    '@amount' => number_format($fundraiser->minimum_donation_amount, 2),
  ));

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
      'amount' => t('You have entered a custom amount and selected a set amount. Please clarify which amount you intend to give, if you want to give the amount that appears in the other box, please select Other from the radio buttons.'),
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
  $minimum_donation_amount = isset($fundraiser->minimum_donation_amount) ? $fundraiser->minimum_donation_amount : variable_get('fundraiser_default_minimum', 10.00);
  $minimum_donation_amount = number_format($minimum_donation_amount, 2);
  if ($value < $minimum_donation_amount) {
    return array(
      'other_amount' => t('Your donation amount must be greater than or equal to @min_amount.', array(
        '@min_amount' => $minimum_donation_amount,
      )),
    );
  }
}
