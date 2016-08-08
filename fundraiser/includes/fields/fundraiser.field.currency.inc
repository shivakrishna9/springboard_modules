<?php
/**
 * @file
 * Various functions for the Currency field.
 */

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
