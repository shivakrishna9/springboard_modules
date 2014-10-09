<?php
/**
* @file
* Provides rules integration for fundraiser events.
*/
/**
* Implements hook_rules_event_info().
*/
function springboard_health_rules_event_info() {
  $events = array(
    'springboard_health_row_count_exceeded'  => array(
    'label' => t('Row count exceeded on monitored table'),
    'group' => t('Springboard'),
    'variables' => array(
      'sbh_table' => array('type' => 'text', 'label' => t('The table.')),
      ),
    ),
  );
  return $events;
}