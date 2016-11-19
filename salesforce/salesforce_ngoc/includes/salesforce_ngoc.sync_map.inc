<?php
/**
 * @file
 * CRUD and helper functions for salesforce_ngoc_sync_map entity.
 */

/**
 * Creates or updates a Salesforce NGOC sync map record.
 *
 * @param mixed $record
 *   Can be an array of values or a salesforce_ngoc_sync_map entity object.
 *
 * @return mixed
 *   SAVED_NEW or SAVED_UPDATED depending on the operation performed. Otherwise,
 *   FALSE is returned if the record could not be saved.
 */
function salesforce_ngoc_sync_map_save($record) {
  if (is_array($record)) {
    // Allow sending an array and updating an existing entity.
    $id = _salesforce_ngoc_sync_get_id($record);
    if (!empty($id)) {
      $entity = salesforce_ngoc_sync_map_load($id);
    }
    // Adds array values to the loaded entity.
    if (isset($entity)) {
      foreach ($record as $key => $value) {
        $entity->$key = $value;
      }
      $record = $entity;
    }
    else {
      // Create a new entity object, but does not yet add to the database.
      $record = entity_create('salesforce_ngoc_sync_map', $record);
    }
  }
  if ($record->entityType() == 'salesforce_ngoc_sync_map') {
    // Adds the data to the database.
    $result = entity_save('salesforce_ngoc_sync_map', $record);
  }
  return FALSE;
}

/**
 * Creates and/or updates Salesforce NGOC sync map record(s).
 * @param array $records
 *   An array of salesforce_ngoc_sync_map records.
 */
function salesforce_ngoc_sync_map_save_multiple($records) {
  foreach ($records as $record) {
    salesforce_ngoc_sync_map_save((array) $record);
  }
}

/**
 * Load a single salesforce_ngoc_sync_map record.
 *
 * @param int $id
 *    The id representing the record we want to load.
 * @param bool $reset
 *    Whether to reset the internal cache for the requested entity type.
 *
 * @return mixed
 *    The Entity Object on success or FALSE.
 */
function salesforce_ngoc_sync_map_load($id, $reset = FALSE) {
  $entities = salesforce_ngoc_sync_map_load_multiple(array($id), array(), $reset);
  // entity_load() returns an array. We want the entity itself.
  if (!isset($entities[$id])) {
    return FALSE;
  }
  return $entities[$id];
}

/**
 * Load multiple salesforce_ngoc_sync_map records.
 */
function salesforce_ngoc_sync_map_load_multiple($ids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('salesforce_ngoc_sync_map', $ids, $conditions, $reset);
}

/**
 * Helper to grab the record id of a map.
 *
 * @param mixed $item
 *   Can be an array of values or an entity object.
 */
function _salesforce_ngoc_sync_get_id($item) {
  if (isset($item->id) && !empty($item->id)) {
    return $item->id;
  }
  if (is_object($item)) {
    $record = (array) clone($item);
  }
  else {
    $record = $item;
  }
  if (isset($record['sobject']->Id)) {
    return db_select('salesforce_ngoc_sync_map', 'm')
      ->fields('m', array('id'))
      ->condition('sfid', $record['sobject']->Id)
      ->execute()
      ->fetchField();
  }
  else {
    return db_select('salesforce_ngoc_sync_map', 'm')
      ->fields('m', array('id'))
      ->condition('module', $record['module'])
      ->condition('delta', $record['delta'])
      ->condition('drupal_id', $record['drupal_id'])
      ->execute()
      ->fetchField();
  }
}
