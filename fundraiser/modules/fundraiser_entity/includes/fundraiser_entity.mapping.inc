<?php

/**
 * Menu callback form, provides a user interface to set up donation entity fields to data source mappings. 
 */
function fundraiser_entity_default_mapping_form($form, &$form_state) {
  $form['info'] = array(
    '#value' => t('This form allows you to map the submitted form values to Donation entity fields. ' .
      'When the webform form is submitted, a donation entity will be created with the values from the form.'),
    '#weight' => 0,
  );

  // Select when the values should be mapped to the entity
  $form['fundraiser_entity_map_point'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select when the donation values should be mapped to the entity.'),
    '#options' => array(
      'create' => t('Create'),
      'update' => t('Update'),
      'success' => t('On successful donation'),
    ),
    '#default_value' => variable_get('fundraiser_entity_map_point', array()),
  );

  $header = array(
    t('Donation Field'),
    t('Data Source'),
    t('Optional token or text'),
  );

  // Table element.
  $form['mapping'] = array(
    '#prefix' => '<div id="fields">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => array(),
  );

  // Load the default mapping.
  $default_map = variable_get('fundraiser_entity_default_map', array());

  $donation_fields = _fundraiser_entity_get_entity_fields();

  // Protect fields that are set in code.
  $protect = array(
    'did',
    'nid',
    'sid',
    'uid',
    'status',
    'created',
    'changed',
    'amount',
    'currency',
    'txn_id',
    'gateway',
    'form_url'
  );

  // Don't show these fields.
  $remove = array(
    'url',
  );

  // Track all the fields in the form.
  $form['#donation_fields'] = $donation_fields;

  // Create an array of options from the related entities.
  $entity_types = array(
    'node' => 'Node',
    'user' => 'User',
    'commerce_order' => 'Order',
    'commerce_payment_transaction' => 'Transaction',
    'commerce_customer_profile' => 'Customer',
  );

  $options[] = '--select--';
  // Add the option for entering a fixed text or token value.
  $options['other'] = 'Token or text';

  foreach ($entity_types as $entity_type => $opt_name) {
    $entity_fields = _fundraiser_entity_get_entity_fields($entity_type, NULL);
    foreach ($entity_fields as $name => $label) {
      $options[$opt_name][ $entity_type . '--' . $name ] = $label;
    }
    asort($options[$opt_name]);
  }

  // Create an array of options from the webform fields.
  foreach ($donation_fields as $name => $label) {
    if (in_array($name, $remove)) {
      continue;
    }
    elseif (in_array($name, $protect)) {
      $form['mapping']['#rows'][] = array(
        array('data' => $label),
        array('data' => 'This value is set in code.'),
        '',
      );
    }
    else {
      $default_source = isset($default_map[$name]) ? $default_map[$name]['source'] : '';
      $default_other =  isset($default_map[$name]) ? $default_map[$name]['other'] : '';

      // Create a select field for this entity field.
      $field_id = drupal_clean_css_identifier($name);
      $source = array(
        '#id' => 'donation-field-' . $field_id . '-source',
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $default_source,
      );

      $other = array(
        '#id' => 'donation-field-' . $field_id . '-other',
        '#type' => 'textfield',
        '#default_value' => $default_other,
        '#states' => array(
          'visible' => array(
            '[id="donation-field-' . $field_id . '-source"]' => array('value' => 'other'),
          ),
        )
      );

      // Add the field to the form.
      $form['mapping'][$name] = array(
        'source' => &$source,
        'other' => &$other,
      );

      // Add the rows to the table.
      $form['mapping']['#rows'][] = array(
        array('data' => $label),
        array('data' => &$source),
        array('data' => &$other),
      );
 
      unset($source, $other);
    }
  }

  $token_set = array(
    'donation',
    'node',
    'user',
    'commerce-order',
    'commerce-payment-transaction',
    'webform-tokens',
    'commerce-customer-profile'
  );

  $form['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 9,
  );

  // Then add any other token set as needed.
  $form['tokens']['tokens'] = array(
    '#type' => 'item',
    '#title' => t('Drupal tokens'),
    '#description' => theme('token_tree', array('token_types' => $token_set, 'recursion_limit' => 2, 'click_insert' => FALSE)),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;

}

/**
 * Validate handler for mapping form.
 */
function fundraiser_entity_default_mapping_form_validate($form, $form_state) {
  foreach ($form_state['values']['mapping'] as $name => $values) {
    if (
      !empty($form['#donation_fields'][$name])
      && !empty($values['source'])
      && $values['source'] == 'other'
      && empty($values['other'])
    ) {
      $field_name = $form['#donation_fields'][$name];
      form_set_error('mapping[' . $name . '][other]', t("You did not enter a token or text value for the @field", array('@field' => $field_name)));
    }
  }
}

/**
 * Submit handler for mapping form.
 */
function fundraiser_entity_default_mapping_form_submit($form, $form_state) {
  variable_set('fundraiser_entity_map_point', $form_state['values']['fundraiser_entity_map_point']);

  // Save the map values to the setting
  $mapped = array();

  foreach ($form_state['values']['mapping'] as $name => $values) {
    if (!empty($form['#donation_fields'][$name]) && !empty($values['source'])) {
      $mapped[$name] = array(
        'source' => $values['source'],
        'other' => $values['other'],
      );
    }
  }

  variable_set('fundraiser_entity_default_map', $mapped);
}
