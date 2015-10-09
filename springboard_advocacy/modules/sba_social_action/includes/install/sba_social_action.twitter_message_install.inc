<?php

function sba_social_action_create_twitter_message_entity() {

  entity_save('sba_message',
    entity_create('sba_message_type',
      array(
        'label'=> 'SBA Social Action Twitter Message',
        'type' => 'sba_social_action_twitter_message'
      )
    )
  );

  $field_bases = array();

// Exported field_base: 'field_sba_twitter_message'
  $field_bases['field_sba_twitter_editable'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_twitter_editable',
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
        'Not editable' => 'Not editable',
        'Editable' => 'Editable',
        'Editing is required' => 'Editing is required',
      ),
      'allowed_values_function' => '',
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );


// Exported field_base: 'field_sba_twitter_message'
  $field_bases['field_sba_twitter_message'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_twitter_message',
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
    'type' => 'text_long',
  );


// Exported field_base: 'field_sba_action_id'
  $field_bases['field_sba_action_id'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_action_id',
    'foreign keys' => array(),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'base',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'sort' => array(
          'type' => 'none',
        ),
        'target_bundles' => array(
          'sba_social_action' => 'sba_social_action',
        ),
      ),
      'target_type' => 'node',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  $field_instances = array();

// Exported field_instance: 'sba_message-sba_message_action_message-field_sba_twitter_message'
  $field_instances['field_sba_twitter_message'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_plain',
        'weight' => 9,
      ),
    ),
    'entity_type' => 'sba_message',
    'field_name' => 'field_sba_twitter_message',
    'label' => 'Default text',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 9,
    ),
  );


// Exported field_instance: 'sba_message-sba_message_action_message-field_sba_twitter_message'
  $field_instances['field_sba_twitter_editable'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => array(
      0 => array(
        'value' => 'Not editable',
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 7,
      ),
    ),
    'entity_type' => 'sba_message',
    'field_name' => 'field_sba_twitter_editable',
    'label' => 'Editable by action taker?',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 1,
    ),
  );

  $field_instances['field_sba_action_id'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => NULL,
    'default_value_function' => 'entityreference_prepopulate_field_default_value',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'sba_message',
    'field_name' => 'field_sba_action_id',
    'label' => 'Action ID',
    'required' => 0,
    'settings' => array(
      'behaviors' => array(
        'prepopulate' => array(
          'action' => 'hide',
          'action_on_edit' => 1,
          'fallback' => 'form_error',
          'providers' => array(
            'node_url' => 1,
            'og_context' => FALSE,
            'url' => 0,
          ),
          'skip_perm' => 0,
          'status' => 1,
        ),
      ),
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'entityreference',
      'settings' => array(
        'match_operator' => 'CONTAINS',
        'path' => '',
        'size' => 60,
      ),
      'type' => 'entityreference_autocomplete',
      'weight' => 4,
    ),
  );

  foreach ($field_bases as $key => $values) {
    $base = field_info_field($key);
    if (empty($base)) {
      field_create_field($values);
    }
  }
  foreach ($field_instances as $key => $instance) {
    $message = field_info_instance('sba_message', $key, 'sba_social_action_twitter_message');
    if (empty($message)) {
      field_create_instance($instance);
    }
  }
}




/**
 * Delete springboard message fields
 */
function _sba_social_action_delete_twitter_fields() {
  $fields = array(
    'field_sba_twitter_message',
    'field_sba_twitter_editable',
  );
  foreach ($fields as $field_name) {
    if($field = field_info_field($field_name)){
      if (isset($field['bundles']['sba_message']))
        field_delete_field($field_name);
    }
  }
  drupal_set_message('Sba Social Action twitter fields deleted.');
  field_group_field_attach_delete_bundle('sba_message','sba_social_action_twitter_message');
  $twit = sba_message_type_load('sba_social_action_twitter_message');
  if($twit) {
    sba_message_type_delete($twit);
  }
}
