<?php

/**
 * @file
 * Fields for fundraiser designations.
 */

/**
 * Creates the extra product fields.
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
//  $field_data = _fundraiser_designations_product_fields();
//
//  foreach ($field_data as $field_name => $field_details) {
//    $field_id = field_info_field($field_name);
//    $instance_id = field_info_instance('commerce_product', $field_name, 'fundraiser_designation');
//
//    if (empty($field_id)) {
//      field_create_field($field_data[$field_name]['field_config']);
//    }
//    if (empty($instance_id)) {
//      field_create_instance($field_data[$field_name]['field_instance']);
//    }
//  }
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
      'settings' => array(),
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
      'translatable' => 0,
      'type' => 'text_with_summary',
    ),
    'field_instance' => array(
      'bundle' => 'fundraiser_designation',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 2,
        ),
        'teaser' => array(
          'label' => 'hidden',
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
        'display_summary' => 0,
        'text_processing' => 1,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'rows' => 10,
          'summary_rows' => 5,
        ),
        'type' => 'text_textarea_with_summary',
        'weight' => 7,
      ),
    ),
  );

  $fields['fd_designations_image'] = array(
    'field_config' => array(
      'active' => 1,
      'cardinality' => 1,
      'deleted' => 0,
      'entity_types' => array('commerce_product'),
      'field_name' => 'fd_designations_image',
      'foreign keys' => array(
        'fid' => array(
          'columns' => array(
            'fid' => 'fid',
          ),
          'table' => 'file_managed',
        ),
      ),
      'indexes' => array(
        'fid' => array(
          0 => 'fid',
        ),
      ),
      'locked' => 0,
      'module' => 'image',
      'settings' => array(
        'default_image' => 0,
        'uri_scheme' => 'public',
      ),
      'translatable' => 0,
      'type' => 'image',
    ),
    'field_instance' => array(
      'bundle' => 'fundraiser_designation',
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'module' => 'image',
          'settings' => array(
            'image_link' => '',
            'image_style' => '',
          ),
          'type' => 'image',
          'weight' => -1,
        ),
        'teaser' => array(
          'label' => 'hidden',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'commerce_product',
      'field_name' => 'fd_designations_image',
      'label' => 'Image',
      'required' => 0,
      'settings' => array(
        'alt_field' => 1,
        'default_image' => 0,
        'file_directory' => '',
        'file_extensions' => 'png gif jpg jpeg',
        'max_filesize' => '',
        'max_resolution' => '',
        'min_resolution' => '',
        'title_field' => 0,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'image',
        'settings' => array(
          'preview_image_style' => 'thumbnail',
          'progress_indicator' => 'throbber',
        ),
        'type' => 'image_image',
        'weight' => 0,
      ),
    ),
  );
  return $fields;
}
