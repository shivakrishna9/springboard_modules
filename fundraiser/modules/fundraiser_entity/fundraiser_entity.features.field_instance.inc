<?php
/**
 * @file
 * fundraiser_entity.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function fundraiser_entity_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_cc_exp_month'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_cc_exp_month'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'number',
        'settings' => array(
          'decimal_separator' => '.',
          'prefix_suffix' => TRUE,
          'scale' => 0,
          'thousand_separator' => ' ',
        ),
        'type' => 'number_integer',
        'weight' => 9,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_cc_exp_month',
    'label' => 'Credit Card Exp Month',
    'required' => 0,
    'settings' => array(
      'max' => '',
      'min' => '',
      'prefix' => '',
      'suffix' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'number',
      'settings' => array(),
      'type' => 'number',
      'weight' => 13,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_cc_exp_year'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_cc_exp_year'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'number',
        'settings' => array(
          'decimal_separator' => '.',
          'prefix_suffix' => TRUE,
          'scale' => 0,
          'thousand_separator' => ' ',
        ),
        'type' => 'number_integer',
        'weight' => 8,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_cc_exp_year',
    'label' => 'Credit Card Exp Year',
    'required' => 0,
    'settings' => array(
      'max' => '',
      'min' => '',
      'prefix' => '',
      'suffix' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'number',
      'settings' => array(),
      'type' => 'number',
      'weight' => 14,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_cc_number'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_cc_number'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 4,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_cc_number',
    'label' => 'Credit Card Number',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 12,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_cc_type'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_cc_type'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 6,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_cc_type',
    'label' => 'Credit Card Type',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 16,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_donor_address'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_donor_address'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => array(
      0 => array(
        'element_key' => 'fundraiser_donation|fundraiser_donation|field_fde_donor_address|und|0',
        'thoroughfare' => '',
        'premise' => '',
        'locality' => '',
        'administrative_area' => '',
        'postal_code' => '',
        'country' => 'US',
        'first_name' => '',
        'last_name' => '',
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'administrator' => array(
        'label' => 'above',
        'module' => 'addressfield',
        'settings' => array(
          'format_handlers' => array(
            'address' => 'address',
            'name-full' => 'name-full',
          ),
          'use_widget_handlers' => 0,
        ),
        'type' => 'addressfield_default',
        'weight' => 1,
      ),
      'default' => array(
        'label' => 'above',
        'module' => 'addressfield',
        'settings' => array(
          'format_handlers' => array(
            'address' => 'address',
            'name-full' => 'name-full',
          ),
          'use_widget_handlers' => 0,
        ),
        'type' => 'addressfield_default',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_donor_address',
    'label' => 'Donor Address',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'addressfield',
      'settings' => array(
        'available_countries' => array(),
        'format_handlers' => array(
          'address' => 'address',
          'name-full' => 'name-full',
          'address-hide-country' => 0,
          'organisation' => 0,
          'name-oneline' => 0,
        ),
      ),
      'type' => 'addressfield_standard',
      'weight' => 9,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_eft_account_number'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_eft_account_number'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 3,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_eft_account_number',
    'label' => 'Account Number',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 11,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_eft_routing_number'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_eft_routing_number'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_eft_routing_number',
    'label' => 'Routing Number',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 10,
    ),
  );

  // Exported field_instance: 'fundraiser_donation-fundraiser_donation-field_fde_payment_method'
  $field_instances['fundraiser_donation-fundraiser_donation-field_fde_payment_method'] = array(
    'bundle' => 'fundraiser_donation',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 10,
      ),
    ),
    'entity_type' => 'fundraiser_donation',
    'field_name' => 'field_fde_payment_method',
    'label' => 'Payment Method',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 17,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Account Number');
  t('Credit Card Exp Month');
  t('Credit Card Exp Year');
  t('Credit Card Number');
  t('Credit Card Type');
  t('Donor Address');
  t('Payment Method');
  t('Routing Number');

  return $field_instances;
}
