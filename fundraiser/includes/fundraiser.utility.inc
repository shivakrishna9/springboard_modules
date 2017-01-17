<?php
/**
 * @file
 * Utility functions for the Fundraiser module.
 */

/**
 * Helper function to determine if this node type supports donation forms.
 */
function fundraiser_is_donation_type($type) {
  $fundraiser_types = &drupal_static(__FUNCTION__, array());
  // Cache fundraiser content types during page execution.
  if (empty($fundraiser_types[$type])) {
    $fundraiser_types[$type] = variable_get('fundraiser_' . $type, FALSE);
  }
  return $fundraiser_types[$type];
}

/**
 * Helper function to determine if this node type supports donation forms.
 */
function fundraiser_get_donation_types() {
  // For all types, check if they are a fundraiser type.
  $types = node_type_get_types();
  foreach ($types as $type => $type_info) {
    $fundraiser_types[$type] = variable_get('fundraiser_' . $type, FALSE);
  }
  return $fundraiser_types;
}

/**
 * Helper function to check if the current page is SSL protected.
 */
function _fundraiser_is_secure() {
  return (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') ? TRUE : FALSE;
}

/**
 * Helper function to check if given string is an email format.
 *
 * Future plans: this may need to be moved to a toolkit module for use by all
 * our modules.
 */
function _fundraiser_validate_email($mail) {
  return preg_match("/^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,6}$/", $mail);
}

/**
 * Helper function which collects gateway information.
 *
 * Calls hook_fundraiser_gateway_info.
 */
function _fundraiser_gateway_info($id = NULL) {
  $gateways = &drupal_static(__FUNCTION__, array());
  // Cache the gateway during page execution.
  if (empty($gateways)) {
    // If not, we go asking anyone else - usually the payment glue module.
    $gateways = module_invoke_all('fundraiser_gateway_info');
    drupal_alter('fundraiser_gateway_info', $gateways);
  }

  // Look up the gateway needed.
  if (!empty($id)) {
    foreach ($gateways as $gateway) {
      if ($gateway['id'] == $id) {
        return $gateway;
      }
    }
    return array();
  }

  return (array) $gateways;
}

/**
 * Get the relevant currency data array from a donation enabled node.
 *
 * @param stdClass $node
 *   The donation enabled node.
 *
 * @param array
 *   The currency data as an associative array. Includes keys such as 'name',
 *   'code', 'symbol', and others provided by Drupal Commerce.
 */
function fundraiser_get_currency_from_node($node) {
  try {
    // See if we have the currency field on the node.
    // If we don't then entity_metadata_wrapper will throw an exception.
    $wrapper = entity_metadata_wrapper('node', $node);
    $currency_code = $wrapper->field_fundraiser_currency->value();

    $currency = commerce_currency_load($currency_code);
    // If that currency doesn't exist, use the site default.
    if (!is_array($currency)) {
      $currency = commerce_currency_load();
    }
  }
  catch (Exception $e) {
    // Load the default currency as set by Commerce.
    $currency = commerce_currency_load();
  }

  return $currency;
}

/**
 * Retrieves and formats the currencies for a form select element.
 *
 * @return array
 *   The options array for a select form element.
 */
function fundraiser_currency_field_get_currency_options() {
  $options = array();

  foreach (commerce_currencies(TRUE, TRUE) as $currency_code => $currency) {
    $options[$currency_code] = t('@code - !name', array('@code' => $currency['code'], '!name' => $currency['name']));

    if (!empty($currency['symbol'])) {
      $options[$currency_code] .= ' - ' . check_plain($currency['symbol']);
    }
  }

  return $options;
}

/**
 * Helper function to get donation IDs by user ID.
 */
function fundraiser_donation_get_donations_by_uid($uid, $limit = 0, $order_by = 'DESC', $gateway = NULL, $card_id = NULL) {
  $query = db_select('fundraiser_donation', 'fd')
    ->fields('fd')
    ->condition('uid', $uid, '=');
  if (!empty($gateway)) {
    $query->condition('gateway', $gateway, '=');
  }
  if (!empty($card_id)) {
    $query->condition('card_id', $card_id, '=');
  }
  $query = $query->range(0, $limit)
    ->orderBy('did', $order_by)
    ->execute();
  if (!$query->rowCount()) {
    return FALSE;
  }
  $donation_record = $query->fetchObject();
  return fundraiser_donation_get_donation($donation_record->did, FALSE, $donation_record);
}

/**
 * Helper function to get latest donation ID by user ID.
 */
function fundraiser_donation_get_newest_donation_by_uid($uid) {
  return fundraiser_donation_get_donations_by_uid($uid, 1);
}

/**
 * Helper function, generate fields from given field_info.
 */
function _fundraiser_create_field_from_info($field_key, $field_info, $data) {
  $field = array(
    '#type' => $field_info['#type'],
    '#title' => t('@title', array('@title' => $field_info['#title'])),
    '#required' => (isset($field_info['#required']) && $field_info['#required'] == 1) ? TRUE : FALSE,
    '#default_value' => !isset($data[$field_key]) ? '' : $data[$field_key],
  );
  if ($field_info['#type'] == 'select') {
    $options = array();
    $items = $field_info['#extra']['items'];
    $items = explode("\n", $items);
    foreach ($items as $item) {
      $items_values = explode('|', $item);
      $key = trim($items_values[0]);
      $label = isset($items_values[1]) ? trim($items_values[1]) : $key;
      if (!empty($key)) {
        $options[$key] = $label;
      }
    }
    $field['#options'] = $options;
    $field['#multiple'] = (isset($field_info['#extra']['multiple']) && $field_info['#extra']['multiple'] == 1) ? TRUE : FALSE;
  }
  return $field;
}

/**
 * Search the card on file table for a record.
 *
 * @param array $fields
 *    Array of fields to search card on file table.
 *
 * @return object
 *    Card on file card entity or FALSE if no record.
 *
 * @todo This is a copy of `_fundraiser_commerce_cardonfile_match` which needs
 * to be removed and any instances replaced with this function.
 */
function _fundraiser_cardonfile_match($fields, $active = TRUE) {
  // Query for the card on file profile using the provided values.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'commerce_cardonfile')
    ->propertyCondition('uid', $fields['uid'])
    ->propertyCondition('instance_id', $fields['instance_id'])
    ->propertyCondition('card_number', $fields['card_number']);

  if (!empty($fields['card_exp_month'])) {
    $query->propertyCondition('card_exp_month', $fields['card_exp_month']);
  }

  if (!empty($fields['card_exp_year'])) {
    $query->propertyCondition('card_exp_year', $fields['card_exp_year']);
  }

  if ($active) {
    $query->propertyCondition('status', TRUE);
  }

  $result = $query->execute();

  if (isset($result['commerce_cardonfile'])) {
    $ids = array_keys($result['commerce_cardonfile']);
    return commerce_cardonfile_load($ids[0]);
  }
  else {
    return FALSE;
  }
}

/**
 * Load the card on file card data for a donation.
 *
 * @param object $donation
 *   Fundraiser donation object.
 * @param bool $active
 *   Flag to select only active cards, defaults to TRUE.
 *
 * @return object
 *   Card on file card entity
 *
 * @todo This is just a copy of `_fundraiser_commerce_donation_cardonfile_card`
 * which needs to be removed and any instances replaced with this function.
 */
function _fundraiser_donation_cardonfile_card($donation, $active = TRUE) {
  // If this donation has the cardonfile value set in its data array, load the
  // card data from that.
  if (!empty($donation->data['cardonfile'])) {
    return commerce_cardonfile_load($donation->data['cardonfile']);
  }

  // Allow modules to provide their own card on file profile field values.
  if (!empty($donation->gateway['cardonfile callback'])) {
    $cardonfile_fields = $donation->gateway['cardonfile callback']($donation);
  }
  // If this is a credit purchase use the default credit function.
  elseif ($donation->donation['payment_method'] == 'credit') {
    module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
    $cardonfile_fields = _fundraiser_commerce_credit_card_cardonfile_fields($donation);
  }

  if (empty($cardonfile_fields)) {
    return FALSE;
  }

  return _fundraiser_cardonfile_match($cardonfile_fields, $active);
}

/**
 * Total donation amount donated across multiple forms.
 *
 * Ecommerce-agnostic lookup of total dollar amount donated via multiple
 *   donation forms.
 *
 * Looks up by multiple nids in the hopes of a single faster IN query.
 *
 * @param array $nids
 *   Node IDs.
 * @param int|bool $start
 *   Start timestamp or FALSE for no range.
 * @param int|bool $end
 *   Ending timestamp or FALSE for no range.
 *
 * @return float|bool
 *   Total successful donation amount or FALSE if none found.
 */
function fundraiser_get_total_donations_by_nids($nids, $start = FALSE, $end = FALSE) {
  $total_donations = module_invoke_all('fundraiser_get_total_donations_by_nids', $nids, $start, $end);
  $total = isset($total_donations[0]) ? $total_donations[0] : FALSE;
  return $total;
}
