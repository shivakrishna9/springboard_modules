<?php
/**
 * @file
 * Various functions for the Amount field.
 */

/**
 * Callback to update a field with fundraiser specific settings during creation.
 *
 * @param object $fundraiser
 *   The fundraiser object.
 * @param array $field
 *   The field array.
 */
function _fundraiser_amount_field_create($fundraiser, $field) {
  $currency = fundraiser_get_currency_from_node($fundraiser);
  $currency_symbol = check_plain($currency['symbol']);

  // If donation amounts don't exist, use amount as is.
  if (empty($fundraiser->donation_amounts) || !is_array($fundraiser->donation_amounts)) {
    // Show the minimum amount when only displaying an amount text box.
    $field['#extra']['description'] = t('Minimum payment @symbol@amount.', array(
      '@symbol' => $currency_symbol,
      '@amount' => number_format($fundraiser->minimum_donation_amount, 2),
    ));
    return $field;
  }

  // Otherwise we want a select field of donation amounts.
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
      // Strip out other other|Other combiantions and make sure this is at the
      // end.
      $formatted_amounts = array_diff($formatted_amounts, array('other|Other'));
      $formatted_amounts[] = 'other|Other';
    }

    $formatted_amounts = implode("\n", $formatted_amounts);
    $field += array(
      '#title' => t('Please select your tax-deductible gift amount below'),
      '#type' => 'select',
      '#extra' => array(
        'description' => '',
        'items' => $formatted_amounts,
        'multiple' => 0,
      ),
    );

    // Choose the default value that will be checked.
    $default = $keys[0];
    if (empty($fundraiser->default_amount)) {
      $default = '';
    }
    elseif (in_array($fundraiser->default_amount, $keys)) {
      // Default is one of the defined ask amounts. Use it, but make sure the
      // formatting matches the ask amount exactly.
      foreach ($keys as $ask_amount) {
        if ($ask_amount == $fundraiser->default_amount) {
          $default = $ask_amount;
          break;
        }
      }
    }
    $field['#value'] = $default;

    // Extra settings may get clobbered in fundraiser_webform. if we don't flag
    // them.
    // See "extra config" comment under _fundraiser_webform_update_component().
    if (!empty($field['#extra'])) {
      $field['#node_extra_settings'] = $field['#extra'];
    }
  }

  return $field;
}
