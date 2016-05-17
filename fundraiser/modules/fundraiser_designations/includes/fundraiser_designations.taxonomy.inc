<?php

/**
 * @file
 * Install helper functions for Fundraiser Designations taxonomy.
 */

/**
 * Install Fundraiser  Designations categories vocabulary.
 *
 */
function _fundraiser_designations_install_vocab_fd_designation_categories($product_type) {
  $already_exists = FALSE;
  foreach (taxonomy_get_vocabularies() as $vocab) {
    if ($vocab->machine_name == 'fd_designation_categories') {
      $already_exists = TRUE;
      break;
    }
  }
  if (!$already_exists) {
    $t = get_t();
    $status = taxonomy_vocabulary_save((object) array(
      'name' => t("Fundraiser Designation Categories"),
      'machine_name' => 'fd_designation_categories',
      'description' => $t("Categories for Fund Designations."),
    ));
    if ($status == SAVED_NEW) {
      $vocab = taxonomy_vocabulary_machine_name_load('fd_designation_categories');
      // Populate with Springboard standard terms.
      $weight = 0;
      foreach (_fundraiser_designations_default_categories() as $parent => $children) {
        taxonomy_term_save((object) array(
          'vid' => $vocab->vid,
          'name' => $t($parent),
          'weight' => $weight++,
          'vocabulary_machine_name' => $vocab->machine_name,
        ));
        $parent_terms = taxonomy_term_load_multiple(array(), array('name' => $t($parent), 'vid' => $vocab->vid));
        if (!empty($parent_terms)) {
          $parent_term = reset($parent_terms);
          foreach ($children as $child) {
            taxonomy_term_save((object) array(
              'vid' => $vocab->vid,
              'name' => $t($child),
              'weight' => $weight++,
              'parent' => array($parent_term->tid),
              'vocabulary_machine_name' => $vocab->machine_name,
            ));
          }
        }
      }
    }
    _fundraiser_designations_install_taxonomy_fields($product_type);
  }
}

/**
 * Define legislative issues terms.
 *
 * @return array
 */
function _fundraiser_designations_default_categories() {
  $t = get_t();
  return array(
    "General" => array(),
  );
}

/**
 * Install the legislative issue and org issue taxonomy fields on a node type.
 *
 * @param $product_type
 * @throws \Exception
 * @throws \FieldException
 */
function _fundraiser_designations_install_taxonomy_fields($product_type) {
  module_load_include('inc', 'field', 'field.crud');
  module_load_include('inc', 'field', 'field.info');

  if (function_exists('_fundraiser_designations_install_taxonomy_field_definitions')) {
    foreach (_fundraiser_designations_install_taxonomy_field_definitions($product_type) as $field_name => $field) {
      $exists = field_info_field('field_' . $field_name);
      if (empty($exists)) {
        field_create_field($field['field_config']);
      }
      // Before adding the field, check that it doesn't exist on the node type.
      if (!field_info_instance('node', 'field_' . $field_name, $product_type)) {
        field_create_instance($field['field_instance']);
      }
    }
  }
}

/**
 * Taxonomy field definitions.
 *
 * @return mixed
 */
function _fundraiser_designations_install_taxonomy_field_definitions($product_type) {
  $t = get_t();

  $fields['fd_designation_categories'] = array(
    'field_config' => array(
      'field_name' => 'field_fd_designation_categories',
      'type' => 'taxonomy_term_reference',
      'translatable' => '0',
      'cardinality' => '-1',
      'foreign keys' => array(
        'tid' => array(
          'columns' => array(
            'tid' => 'tid',
          ),
          'table' => 'taxonomy_term_data',
        ),
      ),
      'indexes' => array(
        'tid' => array(
          0 => 'tid',
        ),
      ),
      'locked' => '1',
      'module' => 'taxonomy',
      'settings' => array(
        'allowed_values' => array(
          0 => array(
            'vocabulary' => 'fd_designation_categories',
            'parent' => '0',
          ),
        ),
      ),
    ),
    'field_instance' => array(
      'field_name' => 'field_fd_designation_categories',
      'entity_type' => 'commerce_product',
      'bundle' => 'fundraiser_designation',
      'label' => $t("Fund Categories"),
      'default_value' => NULL,
      'required' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => '4',
        ),
        'teaser' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'options',
        'settings' => array(),
        'type' => 'options_buttons',
        'weight' => 5,
      ),
    ),
  );

  return $fields;
}
