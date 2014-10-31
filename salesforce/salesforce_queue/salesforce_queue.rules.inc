<?php

/**
 * @file
 * Provides rules integration for Salesforce queue events.
 */

/**
 * Implements hook_rules_event_info().
 */
function salesforce_queue_rules_event_info() {
  $events = array(
    'salesforce_queue_abnormal_failure' => array(
      'label' => t('Abnormal failure needs review'),
      'group' => t('Salesforce Queue'),
      'variables' => array(
        'abnormality' => array('type' => 'text', 'label' => t('Short description of the abnormality of the failure.')),
        'item_id' => array('type' => 'integer', 'label' => t('The ID of the queue item.')),
        'drupal_id' => array('type' => 'integer', 'label' => t('The ID of the Drupal record.')),
        'module' => array('type' => 'text', 'label' => t('The module responsible for the type of Drupal record.')),
        'delta' => array('type' => 'text', 'label' => t('The delta of the type of Drupal record.')),
        'object_type' => array('type' => 'text', 'label' => t('The Salesforce object type.')),
        'message' => array('type' => 'text', 'label' => t('The error message given to failItem().')),
        'item' => array('type' => 'text', 'label' => t('A variable dump of the queue item.')),
        'result' => array('type' => 'text', 'label' => t('The variable dump of the result object.')),
      ),
    ),
  );
  return $events;
}
