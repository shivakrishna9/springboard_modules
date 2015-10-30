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

  $field_bases['field_sba_show_additional'] = array(
    'active' => 1,
    'cardinality' => 3,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_show_additional',
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
        'Website' => 'Website',
        'Facebook' => 'Facebook',
        'Youtube' => 'Youtube',
      ),
      'allowed_values_function' => '',
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );

  // Exported field_base: 'field_sba_target_options'
  $field_bases['field_sba_target_options'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sba_target_options',
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
        1 => 'Districting On',
        0 => 'Districting Off',
      ),
      'allowed_values_function' => '',
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );

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
          'sba_message_action' => 'sba_message_action',
        ),
      ),
      'target_type' => 'node',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  $field_instances = array();

  $field_instances['field_sba_show_additional'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Check to display additional contact options for targets, if available.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 13,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'sba_message',
    'field_name' => 'field_sba_show_additional',
    'label' => 'Show Additional Contact Options',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 2,
    ),
  );


  $field_instances['field_sba_target_options'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => array(
      0 => array(
        'value' => 1,
      ),
    ),
    'deleted' => 0,
    'description' => 'If you want legislators to receive tweets from their constituents only, turn districting on; this will enable address fields on the user form.',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 12,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'sba_message',
    'field_name' => 'field_sba_target_options',
    'label' => 'Districting Options',
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

// Exported field_instance: 'sba_message-sba_message_action_message-field_sba_twitter_message'
  $field_instances['field_sba_twitter_message'] = array(
    'bundle' => 'sba_social_action_twitter_message',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'The @name of each target you select below will be prepended to this message. Do not include target @names unless they are not your primary targets',
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
    'label' => 'Tweet text',
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
      'weight' => 10,
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

  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sba_target_options|sba_message|sba_social_action_twitter_message|form';
  $field_group->group_name = 'group_sba_target_options';
  $field_group->entity_type = 'sba_message';
  $field_group->bundle = 'sba_social_action_twitter_message';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->label = 'Target Options';
  $field_group->weight = '2';
  $field_group->children = array(
    0 => 'field_sba_target_options',
    1 => 'field_sba_show_additional',
  );
  $field_group->format_type = 'fieldset';
  $field_group->format_settings = array(
    'formatter' => 'open',
    'instance_settings' => array(
      'description' => '',
      'classes' => 'group-sba-target-options field-group-fieldset',
      'required_fields' => 1,
    ),
  );
  $export['group_sba_target_options|sba_message|sba_social_action_twitter_message|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_sba_sample_tweet|sba_message|sba_social_action_twitter_message|form';
  $field_group->group_name = 'group_sba_sample_tweet';
  $field_group->entity_type = 'sba_message';
  $field_group->bundle = 'sba_social_action_twitter_message';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->label = 'Tweet Options';
  $field_group->weight = '1';
  $field_group->children = array(
    0 => 'field_sba_twitter_editable',
    1 => 'field_sba_twitter_message',
  );
  $field_group->format_type = 'fieldset';
  $field_group->format_settings = array(
    'label' => 'Tweet Options',
    'instance_settings' => array(
      'required_fields' => 1,
      'classes' => 'group-sba-tweet field-group-fieldset',
        'description' => 'The @name of each target you select will be prepended automatically to this message. If there are multiple targets, multiple copies of the message will be shown to the site visitor, one per target. The target @names affect your message character limit. Your current longest target @name is <span class="char-limit">X</span> characters. Do not include target @names in the message text unless they are not your primary targets.',
    ),
    'formatter' => 'open',
  );
  $export['group_sba_sample_tweet|sba_message|sba_social_action_twitter_message|form'] = $field_group;

  foreach($export as $field_group) {
    field_group_group_save($field_group);
  }
}

/**
 * Delete springboard message fields
 */
function _sba_social_action_delete_twitter_fields() {
  $fields = array(
    'field_sba_twitter_message',
    'field_sba_twitter_editable',
    'field_sba_show_additional',
    'field_sba_target_options'
  );
  foreach ($fields as $field_name) {
    $field = field_info_instance('node', $field_name, 'sba_social_action_twitter_message');
    field_delete_instance($field);
//    $field = field_info_field($field_name);
//    field_delete_field($field);
  }

  drupal_set_message('Sba Social Action twitter fields deleted.');
  field_group_field_attach_delete_bundle('sba_message','sba_social_action_twitter_message');
  $twit = sba_message_type_load('sba_social_action_twitter_message');
  if($twit) {
    sba_message_type_delete($twit);
  }
}
