<?php

/**
 * @file
 * Provides rules integration for Salesforce watchdog events.
 */

/**
 * Implements hook_rules_event_info().
 */
function salesforce_queue_rules_event_info() {
  $events = array();
  $module_info = salesforce_log_info();
  $events['salesforce_queue_missing_source_record'] = array(
    'group' => t('Salesforce Queue'),
    'module' => 'salesforce_queue',
    'label' => t('Queue item is missing its source record.'),
    'variables' => array(
      'item_id' => array(
        'type' => 'integer',
        'label' => t('Queue item ID'),
        'description' => t('The internal unique ID of the item in the Salesforce queue.'),
      ),
      'drupal_id' => array(
        'type' => 'integer',
        'label' => t('Drupal ID'),
        'description' => t('The unique Drupal record ID of the missing record.'),
      ),
      'module' => array(
        'type' => 'text',
        'label' => t('Queue item module'),
        'description' => t('The Drupal module responsible for the missing record.'),
      ),
      'delta' => array(
        'type' => 'text',
        'label' => t('Queue item delta'),
        'description' => t('The delta (or bundle) of the Drupal module responsible for the missing record.'),
      ),
      'object_type' => array(
        'type' => 'text',
        'label' => t('Salesforce type'),
        'description' => t('The Salesforce object type of the queue item with the missing record.'),
      ),
      'operation' => array(
        'type' => 'text',
        'label' => t('Salesforce operation'),
        'description' => t('The intended Salesforce operation of the broken queue item.'),
      ),
      'dedupe_field' => array(
        'type' => 'text',
        'label' => t('Dedupe field'),
        'description' => t('The Salesforce dedupe field of the broken queue item.'),
      ),
      'sobject' => array(
        'type' => 'text',
        'label' => t('Salesforce sObject'),
        'description' => t('The serialized sObject of the broken queue item.'),
      ),
      'sync_rules' => array(
        'type' => 'text',
        'label' => t('Sync rules'),
        'description' => t('The serialized sync rules of the broken queue item.')
      ),
      'queue' => array(
        'type' => 'text',
        'label' => t('Queue'),
        'description' => t('The specific Salesforce queue of the broken queue item.')
      ),
      'retries' => array(
        'type' => 'integer',
        'label' => t('Number of retries'),
        'description' => t('The number of retries prior to the queue item losing its source record.')
      ),
      'created' => array(
        'type' => 'integer',
        'label' => t('Created time'),
        'description' => t('The timestamp integer for when the broken queue item was created.'),
      ),
    ),
  );
  return $events;
}
