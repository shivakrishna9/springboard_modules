<?php

/**
 * @file
 * Install helper functions for field API fields.
 */

/**
 * Create FieldAPi fields for message actions.
 */
function _sba_social_action_create_node_fields($update = false) {

  $fields = array(
    'field_social_action_img' => array(
      'active' => 1,
      'cardinality' => 1,
      'deleted' => 0,
      'entity_types' => array('node'),
      'field_name' => 'field_social_action_img',
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

      '_instance' => array(
        'bundle' => 'sba_social_action',
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
            'label' => 'above',
            'settings' => array(),
            'type' => 'hidden',
            'weight' => 0,
          ),
        ),
        'entity_type' => 'node',
        'field_name' => 'field_social_action_img',
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
    ),
  );

  $fields['body'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(
      0 => 'node',
    ),
    'field_name' => 'body',
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
    'locked' => 0,
    'module' => 'text',
    'settings' => array(),
    'translatable' => 0,
    'type' => 'text_with_summary',
    '_instance' => array(
      'bundle' => 'sba_social_action',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'module' => 'text',
          'settings' => array(),
          'type' => 'text_default',
          'weight' => 5,
        ),
        'teaser' => array(
          'label' => 'hidden',
          'module' => 'text',
          'settings' => array(
            'trim_length' => 600,
          ),
          'type' => 'text_summary_or_trimmed',
          'weight' => 5,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'body',
      'label' => 'Body',
      'required' => 1,
      'settings' => array(
        'display_summary' => 1,
        'text_processing' => 1,
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'text',
        'settings' => array(
          'rows' => 20,
          'summary_rows' => 5,
        ),
        'type' => 'text_textarea_with_summary',
        'weight' => 5,
      ),
    ),
  );

  // Exported field_base: 'field_social_call_to_action'
  $fields['field_social_call_to_action'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_social_call_to_action',
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
    'locked' => 0,
    'module' => 'text',
    'settings' => array(
      'max_length' => 255,
    ),
    'translatable' => 0,
    'type' => 'text',
    '_instance' => array(
      'bundle' => 'sba_social_action',
      'default_value' => NULL,
      'deleted' => 0,
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'module' => 'text',
          'settings' => array(),
          'type' => 'text_default',
          'weight' => 4,
        ),
        'teaser' => array(
          'label' => 'hidden',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 4,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'field_social_call_to_action',
      'label' => 'Call to Action',
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
        'weight' => 4,
      ),
    ),
  );

  $fields['field_sba_test_mode'] = array(
    'active' => 1,
    'cardinality' => 2,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_test_mode',
    'foreign keys' => array(),
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        1 => 'Enabled',
      ),
      'allowed_values_function' => '',
    ),
    'translatable' => 0,
    'type' => 'list_text',
    '_instance' => array(
      'bundle' => 'sba_social_action',
      'deleted' => 0,
      'description' => 'Check this box to enable test mode. Messages will not be sent to any configured targets when test mode is enabled. Unpublished actions are always in test mode.',
      'display' => array(
        'default' => array(
          'label' => 'Above',
          'module' => 'list',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 3,
        ),
        'teaser' => array(
          'label' => 'hidden',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'node',
      'field_name' => 'field_sba_test_mode',
      'label' => 'Enable Test Mode',
      'required' => 0,
      'settings' => array(
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'options',
        'settings' => array(),
        'type' => 'options_buttons',
        'weight' => 31,
      ),
    ),
  );

  foreach ($fields as $key => $values) {
    $instance = $values['_instance'];
    unset($values['_instance']);
    $base = field_info_field($key);
    $node = field_info_instance('node', $key, 'sba_social_action');
    if (empty($base)) {
      field_create_field($values);
    }
    if (empty($node)) {
      field_create_instance($instance);
    }

  }
}

/**
 * Delete springboard message action fields.
 */
function _sba_social_action_delete_fields() {
  $fields = array(
    'sba_social_call_to_action',
    'field_social_action_img',
  );
  foreach ($fields as $field_name) {
    $field = field_info_instance('node', $field_name, 'sba_social_action');
    field_delete_instance($field);
    $field = field_info_field($field_name);
    field_delete_field($field);
  }
  drupal_set_message('Springboard message action fields deleted.');
}
