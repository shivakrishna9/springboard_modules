<?php

/**
 * @file
 * Provides rules integration for sustainer events.
 */

/**
 * Implements hook_rules_event_info().
 */
function fundraiser_sustainers_rules_event_info() {
  $events = array(
    'fundraiser_sustainer_duplicate_charge_attempt_detected' => array(
      'label' => t('Duplicate sustainer charge attempted'),
      'group' => t('Fundraiser Sustainers'),
      'variables' => array(
        'order_id' => array('type' => 'text', 'label' => t('The id of the order.')),
      ),
    ),
    'fundraiser_sustainer_db_integrity_check_failed' => array(
      'label' => t('Sustainer processing failed database integrity check'),
      'group' => t('Fundraiser Sustainers'),
      'variables' => array(
        'error_message' => array('type' => 'text', 'label' => t('The specific error message.')),
      ),
    ),
  );

  return $events;
}