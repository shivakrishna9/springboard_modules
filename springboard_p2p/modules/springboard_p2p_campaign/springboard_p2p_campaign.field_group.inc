<?php
/**
 * @file
 * springboard_p2p_campaign.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function springboard_p2p_campaign_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_campaign_defaults|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_campaign_defaults';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Personal campaign defaults',
    'weight' => '8',
    'children' => array(
      0 => 'field_p2p_campaigns_approval',
      1 => 'field_p2p_org_intro',
      2 => 'field_p2p_private_access_message',
      3 => 'field_p2p_expiration_message',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Personal campaign defaults',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_campaign_defaults|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_comment_settings|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_comment_settings';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Comment settings',
    'weight' => '7',
    'children' => array(
      0 => 'field_p2p_allow_donor_comments',
      1 => 'field_p2p_show_donor_comments',
      2 => 'field_p2p_show_donor_scroll',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Comment settings',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_comment_settings|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_donation_amount|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_donation_amount';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Personal Campaign suggested donation amount',
    'weight' => '11',
    'children' => array(
      0 => 'field_p2p_ask_amount_edit',
      1 => 'field_p2p_suggested_donation',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Personal Campaign suggested donation amount',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => 'group-p2p-donation-amount field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_donation_amount|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_donation_form|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_donation_form';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Form configuration',
    'weight' => '6',
    'children' => array(
      0 => 'body',
      1 => 'field_p2p_campaign_goals',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Form configuration',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_donation_form|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_images|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_images';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Media',
    'weight' => '13',
    'children' => array(
      0 => 'field_p2p_campaign_banner',
      1 => 'field_p2p_campaign_thumbnail',
      2 => 'group_p2p_video_embed',
      3 => 'group_slideshow',
      4 => 'group_personal_campaign_images',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Media',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_images|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_personal_intro|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_personal_intro';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_tabs';
  $field_group->data = array(
    'label' => 'Personal campaign introduction',
    'weight' => '10',
    'children' => array(
      0 => 'field_p2p_personal_intro',
      1 => 'field_p2p_personal_intro_edit',
      2 => 'field_p2p_feature_pers_campaigns',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Personal campaign introduction',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $export['group_p2p_personal_intro|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_p2p_video_embed|node|p2p_campaign|form';
  $field_group->group_name = 'group_p2p_video_embed';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_images';
  $field_group->data = array(
    'label' => 'Video embed',
    'weight' => '17',
    'children' => array(
      0 => 'field_p2p_video_embed',
      1 => 'field_p2p_video_embed_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_p2p_video_embed|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_personal_campaign_images|node|p2p_campaign|form';
  $field_group->group_name = 'group_personal_campaign_images';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_images';
  $field_group->data = array(
    'label' => 'Personal Campaign Images',
    'weight' => '14',
    'children' => array(
      0 => 'field_p2p_images',
      1 => 'field_p2p_images_edit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-personal-campaign-images field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_personal_campaign_images|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_slideshow|node|p2p_campaign|form';
  $field_group->group_name = 'group_slideshow';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_p2p_images';
  $field_group->data = array(
    'label' => 'Landing page slideshow',
    'weight' => '13',
    'children' => array(
      0 => 'field_p2p_campaign_slider',
      1 => 'field_landing_page_slider_text',
      2 => 'field_landing_page_slider_link',
      3 => 'field_external_link',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_slideshow|node|p2p_campaign|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_tabs|node|p2p_campaign|form';
  $field_group->group_name = 'group_tabs';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'p2p_campaign';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Tabs',
    'weight' => '2',
    'children' => array(
      0 => 'group_p2p_campaign_defaults',
      1 => 'group_p2p_comment_settings',
      2 => 'group_p2p_donation_amount',
      3 => 'group_p2p_donation_form',
      4 => 'group_p2p_images',
      5 => 'group_p2p_personal_intro',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-tabs field-group-htabs',
      ),
    ),
  );
  $export['group_tabs|node|p2p_campaign|form'] = $field_group;

  return $export;
}
