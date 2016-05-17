<?php

/**
 * @file
 */

/**
 *
 */
function _fundraiser_designations_create_fundraiser_designation_product() {
  // First check if the product type exists already and if not add.
  $product_entities = commerce_product_entity_info();
  if (!isset($product_entities['commerce_product']['bundles']['fundraiser_designation'])) {
    $product_type = commerce_product_ui_product_type_new();

    $product_type['type'] = 'fundraiser_designation';
    $product_type['name'] = t('Fundraiser Designations');
    $product_type['description'] = t('Designations for a fundraiser donation.');
    $product_type['is_new'] = TRUE;

    commerce_product_ui_product_type_save($product_type, FALSE);
    commerce_price_create_instance('commerce_price', 'commerce_product', 'fundraiser_designation', st('Price'), 0, 'calculated_sell_price', array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 2,
    ));
  }

  // Now check for fields and if not there add them.
  $field_data = _fundraiser_designations_product_fields();

  foreach ($field_data as $field_name => $field_details) {
    $field_id = field_info_field($field_name);
    $instance_id = field_info_instance('commerce_product', $field_name, 'fundraiser_designation');

    if (empty($field_id)) {
      field_create_field($field_data[$field_name]['field_config']);
    }
    if (empty($instance_id)) {
      field_create_instance($field_data[$field_name]['field_instance']);
    }
  }
}

/**
 * Implements hook_commerce_product_type_info().
 */
function _fundraiser_designations_product_fields() {

  $fields = array();

  $fields['fd_designations_description'] = array(
    'field_config' => array(
      'active' => 1,
      'cardinality' => 1,
      'deleted' => 0,
      'entity_types' => array(),
      'field_name' => 'fd_designations_description',
      'locked' => 0,
      'module' => 'text',
      'settings' => array(
        'max_length' => 255,
      ),
      'translatable' => 0,
      'type' => 'text',
    ),
    'field_instance' => array(
      'bundle' => 'fundraiser_designation',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 2,
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'commerce_product',
      'field_name' => 'fd_designations_description',
      'label' => 'Description',
      'required' => 0,
      'settings' => array(
        'text_processing' => 0,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'size' => 128,
        ),
        'type' => 'text_textfield',
        'weight' => 2,
      ),
    ),
  );

  return $fields;
}
