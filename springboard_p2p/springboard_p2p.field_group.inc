<?php
/**
 * @file
 * springboard_p2p.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function springboard_p2p_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_campaign_defaults|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_campaign_defaults';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Personal campaign defaults',
    'weight' => '6',
    'children' => array(
      0 => 'field_p2p_campaigns_approval',
      1 => 'field_p2p_org_intro',
      2 => 'group_p2p_personal_intro',
      3 => 'group_p2p_images',
      4 => 'group_p2p_video_embed',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Personal campaign defaults',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-p2p-campaign-defaults field-group-fieldset',
        'description' => 'The following settings are used to provide default content for personal campaign pages to make it easier for people to start fundraising for this campaign. ',
      ),
      'formatter' => 'collapsible',
    ),
  );
  $export['group_p2p_campaign_defaults|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_campaign_defaults|node|p2p_category|form';
  $field_group->group_name = 'group_p2p_campaign_defaults';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_category';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Advanced',
    'weight' => '5',
    'children' => array(
      0 => 'field_p2p_org_intro',
      1 => 'group_p2p_personal_intro',
      2 => 'group_p2p_images',
      3 => 'group_p2p_video_embed',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Advanced',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-p2p-campaign-defaults field-group-fieldset',
        'description' => 'The following settings are used to provide default content for campaign pages to make it easier for people to start fundraising for this campaign category.',
      ),
      'formatter' => 'collapsible',
    ),
  );
  $export['group_p2p_campaign_defaults|node|p2p_category|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_comment_settings|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_comment_settings';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Comment settings',
    'weight' => '5',
    'children' => array(
      0 => 'field_p2p_allow_donor_comments',
      1 => 'field_p2p_show_donor_comments',
      2 => 'field_p2p_show_donor_scroll',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-comment-settings field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_comment_settings|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_donation_form|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_donation_form';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Form configuration',
    'weight' => '4',
    'children' => array(
      0 => 'field_p2p_campaign_goals',
      1 => 'field_p2p_form_header',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Form configuration',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-p2p-donation-form field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );
  $export['group_p2p_donation_form|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_donation_form|node|p2p_category|form';
  $field_group->group_name = 'group_p2p_donation_form';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_category';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Donation form',
    'weight' => '4',
    'children' => array(
      0 => 'field_p2p_form_header',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-donation-form field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_donation_form|node|p2p_category|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_images|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_images';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Default campaign images',
    'weight' => '9',
    'children' => array(
      0 => 'field_p2p_images',
      1 => 'field_p2p_images_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-images field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_images|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_images|node|p2p_category|form';
  $field_group->group_name = 'group_p2p_images';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_category';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Default campaign images',
    'weight' => '5',
    'children' => array(
      0 => 'field_p2p_images',
      1 => 'field_p2p_images_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-images field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_images|node|p2p_category|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_personal_intro|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_personal_intro';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Personal campaign introduction',
    'weight' => '6',
    'children' => array(
      0 => 'field_p2p_personal_intro',
      1 => 'field_p2p_personal_intro_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-personal-intro field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_personal_intro|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_personal_intro|node|p2p_category|form';
  $field_group->group_name = 'group_p2p_personal_intro';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_category';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Personal campaign introduction',
    'weight' => '4',
    'children' => array(
      0 => 'field_p2p_personal_intro',
      1 => 'field_p2p_personal_intro_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-personal-intro field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_personal_intro|node|p2p_category|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_video_embed|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_video_embed';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Video embed',
    'weight' => '10',
    'children' => array(
      0 => 'field_p2p_video_embed',
      1 => 'field_p2p_video_embed_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-video-embed field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_video_embed|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_video_embed|node|p2p_category|form';
  $field_group->group_name = 'group_p2p_video_embed';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_category';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_campaign_defaults';
  $field_group->data = array(
    'label' => 'Video embed',
    'weight' => '6',
    'children' => array(
      0 => 'field_p2p_video_embed',
      1 => 'field_p2p_video_embed_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-p2p-video-embed field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_video_embed|node|p2p_category|form'] = $field_group;

  return $export;
}
