<?php

/**
 * @file
 * Rules integration with Springboard P2P.
 */

/**
 * Implements hook_rules_condition_info().
 */
function springboard_p2p_rules_condition_info() {
  return array(
    'springboard_p2p_condition_passed_in_campaign' => array(
      'label' => t('Passed in P2P campaign'),
      'parameter' => array(
        'campaign' => array(
          'label' => t('Campaign'),
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'optional' => TRUE,
          'default value' => NULL,
          'allow null' => TRUE,
        ),
        'value' => array(
          'label' => t('Value'),
          'type' => 'text',
          'options list' => 'springboard_p2p_rules_campaign_options_list',
        ),
      ),
      'group' => t('Springboard P2P'),
    ),
  );
}

/**
 * Options list for the passed in campaign Rules condition.
 *
 * @return array
 *   The options array.
 */
function springboard_p2p_rules_campaign_options_list() {
  return array(
    'not provided' => 'is not provided',
    'public' => 'is public',
    'private' => 'is private',
  );
}

/**
 * The condition evaluation for passed in campaign.
 *
 * @param object|NULL $campaign
 *   The campaign node, or NULL if one isn't passed in.
 * @param string $value
 *   The value to compare the campaign to.
 *   Can be one of 'not provided', 'private', or 'public'.
 *
 * @return bool
 *   TRUE if the condition passes.
 */
function springboard_p2p_condition_passed_in_campaign($campaign, $value) {
  if (is_null($campaign)) {
    if ($value == 'not provided') {
      return TRUE;
    }
    return FALSE;
  }

  if (springboard_p2p_campaign_is_private($campaign)) {
    if ($value == 'private') {
      return TRUE;
    }
    return FALSE;
  }

  if ($value == 'public') {
    return TRUE;
  }

  return FALSE;
}

/**
 * Implements hook_rules_event_info().
 */
function springboard_p2p_rules_event_info() {
  $defaults = array(
    'group' => t('Springboard P2P'),
    'module' => 'springboard_p2p',
    'access callback' => 'springboard_p2p_integration_access',
  );

  return array(
    'springboard_p2p_user_register' => $defaults + array(
      'label' => t('After a user registers for P2P'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User registering'),
        ),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Campaign node'),
          'optional' => TRUE,
        ),
        'create_personal_campaign_url' => array(
          'type' => 'uri',
          'label' => t('Create personal campaign URL'),
          'description' => t('The URL to the personal campaign creation page.  Defaults to the dashboard URL if no campaign is selected during registration.'),
        ),
        'dashboard_url' => array(
          'type' => 'uri',
          'label' => t('User P2P dashboard URL'),
          'description' => t("The URL to the user's list of personal campaigns."),
        ),
      ),
    ),
    'springboard_p2p_user_requests_approval' => $defaults + array(
      'label' => t('After a user requests approval for a private campaign'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User requesting approval'),
        ),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
        ),
        'create_personal_campaign_url' => array(
          'type' => 'uri',
          'label' => t('Create personal campaign URL'),
          'description' => t('The URL to the personal campaign creation page for the campaign.'),
        ),
        'dashboard_url' => array(
          'type' => 'uri',
          'label' => t('User P2P dashboard URL'),
          'description' => t("The URL to the user's list of personal campaigns."),
        ),
        'account_is_registering_for_p2p' => array(
          'type' => 'boolean',
          'label' => t('User is also registering for P2P.'),
          'description' => t('The user is registering for P2P as well as requesting approval.'),
        ),
      ),
    ),
    'springboard_p2p_user_is_approved' => $defaults + array(
      'label' => t('After a user is approved for a private campaign'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('Approved user'),
        ),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
        ),
        'create_personal_campaign_url' => array(
          'type' => 'uri',
          'label' => t('Create personal campaign URL'),
          'description' => t('The URL to the personal campaign creation page for the campaign.'),
        ),
        'dashboard_url' => array(
          'type' => 'uri',
          'label' => t('User P2P dashboard URL'),
          'description' => t("The URL to the user's list of personal campaigns."),
        ),
      ),
    ),
    'springboard_p2p_user_is_rejected' => $defaults + array(
      'label' => t('After a user is rejected for a private campaign'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('Rejected user'),
        ),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
        ),
        'create_personal_campaign_url' => array(
          'type' => 'uri',
          'label' => t('Create personal campaign URL'),
          'description' => t('The URL to the personal campaign creation page for the campaign.'),
        ),
        'dashboard_url' => array(
          'type' => 'uri',
          'label' => t('User P2P dashboard URL'),
          'description' => t("The URL to the user's list of personal campaigns."),
        ),
      ),
    ),
    'springboard_p2p_user_requests_password_reset' => $defaults + array(
      'label' => t('After a user requests a password reset via the P2P UI.'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User requesting a password reset'),
        ),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Campaign node'),
          'optional' => TRUE,
        ),
        'reset_url' => array(
          'type' => 'uri',
          'label' => t('Password reset URL'),
          'description' => t("The URL to the P2P password reset page."),
        ),
      ),
    ),
  );
}

/**
 * P2P integration access callback.
 */
function springboard_p2p_integration_access($type, $name) {
  if ($type == 'event') {
    return entity_access('view', 'user') && entity_access('view', 'node');
  }

  return FALSE;
}
