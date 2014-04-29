<?php

/**
 * @file
 * Rules integration with Springboard P2P.
 */

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
        'account' => array('type' => 'user', 'label' => t('User registering')),
      ),
    ),
    'springboard_p2p_user_requests_approval' => $defaults + array(
      'label' => t('After a user requests approval for a private campaign'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('User requesting approval')),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
        ),
      ),
    ),
    'springboard_p2p_user_is_approved' => $defaults + array(
      'label' => t('After a user is approved for a private campaign'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('Approved user')),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
        ),
      ),
    ),
    'springboard_p2p_user_is_rejected' => $defaults + array(
      'label' => t('After a user is rejected for a private campaign'),
      'variables' => array(
        'account' => array('type' => 'user', 'label' => t('Rejected user')),
        'campaign' => array(
          'type' => 'node',
          'bundle' => 'p2p_campaign',
          'label' => t('Private campaign node'),
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
