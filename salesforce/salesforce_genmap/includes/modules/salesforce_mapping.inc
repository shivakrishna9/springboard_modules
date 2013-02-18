<?php

/**
 * @file
 * Extends salesforce_mapping functionality.
 */

/**
 * @name salesforce_mapping_alter_form Modifications to the mapping form
 * @{
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function salesforce_mapping_form_salesforce_mapping_form_alter(&$form, &$form_state) {
  // Change the mapping table.  The whole table needs to be rebuilt from scratch
  // or else reminents from the prior form will stick around.
  if (isset($form['salesforce_field_mappings_wrapper']['salesforce_field_mappings'])) {
    $form['salesforce_field_mappings_wrapper']['salesforce_field_mappings'] = array(
      '#theme' => 'salesforce_fieldmap_form_table',
      '#tree' => TRUE,
      '#header' => array(
        'drupal_field' => t('Drupal field'),
        'salesforce_field' => t('Salesforce field'),

        // Column 'key' renamed to 'Dedupe'.
        'key' => t('Dedupe'),

        // Add columns for field-by-field sync rules.
        'drupal_sf' => t('Overwrite rule when exporting to Salesforce'),
        'sf_drupal' => t('Overwrite rule when importing into Drupal'),

        // Column 'direction' removed.
        // Column 'delete_field_maping' removed.
      ),
      '#attributes' => array(
        'id' => array('edit-salesforce-field-mappings'),
      ),
    );

    $salesforce_object_type = _sfm_alter_get_default_value('salesforce_object_type', $form_state);
    $property_info = salesforce_mapping_get_fieldmap_types('property');
    $entity_fields = call_user_func($property_info['options_callback'], _sfm_alter_get_default_value('drupal_entity_type', $form_state));
    unset($entity_fields['']);
    $sf_fields = _sfm_get_salesforce_field_options($salesforce_object_type, $form_state);
    $dedupe_isset = FALSE;
    foreach ($entity_fields as $drupal_field_name => $drupal_field_label) {
      $delta = $drupal_field_name;

      // Same as original.
      $row_id = 'edit-salesforce-field-mappings-' . $delta;
      $form['salesforce_field_mappings_wrapper']['salesforce_field_mappings'][$delta] = array(
        '#type' => 'container',
        '#attributes' => array(
          'id' => array($row_id),
        ),
      );
      $row = &$form['salesforce_field_mappings_wrapper']['salesforce_field_mappings'][$delta];

      // Same as original.
      $row['drupal_field'] = array(
        '#type' => 'container',
        '#attributes' => array(
          'id' => array('edit-drupal-field-' . $delta),
        ),
      );

      // We don't need the Drupal field type choices, so set it to properties
      // and hide the select field.
      $row['drupal_field']['fieldmap_type'] = array(
        '#type' => 'hidden',
        '#value' => 'property',
      );

      // We want all of the drupal fields to be listed on the form automatically
      // instead of adding each one manually.  Print Drupal field's user
      // friendly name as a label and set the fieldmap_value to it's machine
      // name.
      $row['drupal_field']['fieldmap_label'] = array(
        '#markup' => '<label>' . $drupal_field_label . '</label>',
      );
      $row['drupal_field']['fieldmap_value'] = array(
        '#type' => 'hidden',
        '#value' => $drupal_field_name,
      );

      // This is the same as the original but simplified since it is referencing
      // solely the property Drupal fieldmap type.
      $salesforce_field = _sfm_alter_get_default_value('salesforce_field', $form_state, $delta);
      $row['salesforce_field'] = array(
        '#id' => 'edit-salesforce-field-' . $delta,
        '#type' => 'select',
        '#description' => t('Select a Salesforce field to map.'),
        '#options' => $sf_fields,
        '#default_value' => $salesforce_field,
        '#ajax' => array(
          'wrapper' => $row_id,
          'callback' => 'salesforce_mapping_form_field_callback',
        ),
      );

      // Same as original.
      if (_sfm_salesforce_field_can_idlookup($salesforce_field, $salesforce_object_type, $form_state)) {
        $key_default_value = _sfm_alter_get_default_value('key', $form_state, $delta);
        if ($key_default_value) {
          $dedupe_isset = TRUE;
        }
        $row['key'] = array(
          '#id' => 'edit-key-' . $delta,
          '#type' => 'radio',
          '#name' => 'key',
          '#return_value' => $delta,
          '#tree' => FALSE,
          '#default_value' => $key_default_value,
        );
      }
      else {
        $row['key'] = array(
          '#type' => 'hidden',
          '#value' => FALSE,
        );
      }

      // Add field-by-field sync rule columns.
      $row['drupal_sf'] = $row['sf_drupal'] = array(
        '#type' => 'radios',
        '#options' => array(
          SALESFORCE_SYNC_RULE_NEVER => t('Never'),
          SALESFORCE_SYNC_RULE_BLANK => t('When blank'),
          SALESFORCE_SYNC_RULE_APPEND => t('Merge values'),
          SALESFORCE_SYNC_RULE_ALWAYS => t('Always'),
        ),
      );
      $sf_drupal_value = _sfm_alter_get_default_value('sf_drupal', $form_state, $delta);
      $drupal_sf_value = _sfm_alter_get_default_value('drupal_sf', $form_state, $delta);
      $row['sf_drupal']['#id'] = 'edit-sf-drupal-' . $delta;
      $row['drupal_sf']['#id'] = 'edit-drupal-sf-' . $delta;
      $row['sf_drupal']['#default_value'] = empty($sf_drupal_value) ? SALESFORCE_SYNC_RULE_NEVER : $sf_drupal_value;
      $row['drupal_sf']['#default_value'] = empty($drupal_sf_value) ? SALESFORCE_SYNC_RULE_NEVER : $drupal_sf_value;

      // Hide direction column.
      $row['direction'] = array(
        '#type' => 'hidden',
        '#value' => 'drupal_sf',
      );
    }

    // Add a "none" option for the dedupe field and some help text.
    $dedupe_help = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => array('salesforce-mapping-dedupe-helptext'),
      ),
      // Using the first column to produce a buffer.
      'drupal_field' => array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('empty-cell'),
          'colspan' => 2,
        ),
      ),
      'key' => array(
        '#id' => 'edit-key-none',
        '#title' => t('None'),
        '#type' => 'radio',
        '#name' => 'key',
        '#return_value' => 'none',
        '#tree' => FALSE,
        '#default_value' => $dedupe_isset ? FALSE : TRUE,
        '#attributes' => array(
          'valign' => 'top',
        ),
      ),
      // Using subsequent columns to contain the help text.
      'sf_drupal' => array(
        '#markup' => '<p>' . t('<strong>Dedupe:</strong> Selecting a Salesforce
          field as a "dedupe field" minimizes duplicate records and allows
          updating existing records <em>when new records are being created</em>.
          Selecting a dedupe field chooses which field will be used as the
          unique identifier for matching potential existing records.  A dedupe
          option only appears next to a Salesforce field that can be used as a
          unique identifier. Select "None" to not use this feature.') . '</p>',
        '#attributes' => array(
          'colspan' => 3,
        ),
      ),
    );
    $form['salesforce_field_mappings_wrapper']['salesforce_field_mappings']['none'] = $dedupe_help;

    // Since we are listing all Drupal fields, we do not need add field button.
    unset($form['salesforce_field_mappings_wrapper']['salesforce_add_field']);
  }

  // The need to manipulate the form causes a need to manipulate the storage
  // of data. Appending a "prevalidate" callback to shift around all of the data
  // so the original validation callback will function properly.
  array_unshift($form['#validate'], 'salesforce_mapping_form_prevalidate');

  // Same as with the need for a "prevalidate" callback, calling another submit
  // callback to add back in the additional data we need that the original
  // submit callback is filtering out.
  $form['#submit'][] = 'salesforce_mapping_form_alter_submit';
}

/**
 * Prevalidation callback for the altered mapping form.
 */
function salesforce_mapping_form_prevalidate($form, &$form_state) {
  $field_maps = &$form_state['values']['salesforce_field_mappings'];
  if (!is_array($field_maps)) {
    return;
  }
  $key = _sfm_alter_get_default_value('key', $form_state);
  foreach ($field_maps as $delta => $field_map) {
    // Two rows need to be created for every one row submitted.  The one that is
    // shown to the user is the "Drupal to Salesforce" configuration, and the
    // "ghost" configuration we're simultating here is the "Salesforce to
    // Drupal" configuration.
    if (is_string($delta)) {
      // If the direction is set to never sync, do not set the row so it doesn't
      // go through and subsequently fail validation.
      if ($field_map['drupal_sf'] != SALESFORCE_SYNC_RULE_NEVER) {
        $field_map['key'] = ($key == $field_map['drupal_field']['fieldmap_value']);
        $field_map['direction'] = 'drupal_sf';
        $field_maps[] = $field_map;
      }
      if ($field_map['sf_drupal'] != SALESFORCE_SYNC_RULE_NEVER) {
        $field_map['key'] = $field_map['drupal_sf'] == SALESFORCE_SYNC_RULE_NEVER ? ($key == $field_map['drupal_field']['fieldmap_value']) : FALSE;
        $field_map['direction'] = 'sf_drupal';
        $field_maps[] = $field_map;
      }
      unset($field_maps[$delta]);
    }
  }
}

/**
 * Submit callback to handle extra data from the altered mapping form.
 */
function salesforce_mapping_form_alter_submit($form, &$form_state) {
  // The original callback, salesforce_mapping_form_submit(), saved the entity
  // and removed our additional data in the process so we'll have to reload it
  // and save it again adding in our new values.
  $machine_name = _sfm_alter_get_default_value('name', $form_state);
  $mapping = salesforce_mapping_load($machine_name);
  $key = _sfm_alter_get_default_value('key', $form_state);
  foreach ($mapping->field_mappings as $delta => $field_map) {
    $mapping->field_mappings[$delta]['key'] = ($key == $field_map['drupal_field']['fieldmap_value']);
    $mapping->field_mappings[$delta]['drupal_sf'] = _sfm_alter_get_default_value('drupal_sf', $form_state, $field_map['drupal_field']['fieldmap_value']);
    $mapping->field_mappings[$delta]['sf_drupal'] = _sfm_alter_get_default_value('sf_drupal', $form_state, $field_map['drupal_field']['fieldmap_value']);
  }
  $mapping->save();

  // Clear the cache so all newly created and modified entities will
  // automatically get the new settings.
  entity_get_controller('salesforce_mapping')->resetCache();
}

/**
 * @} salesforce_mapping_alter_form
 */


/**
 * @name salesforce_mapping_alter_queue Sending entity mapping to Queue
 * @{
 */

/**
 * Implements hook_entity_insert().
 */
function salesforce_genmap_entity_insert($entity, $entity_type) {
  salesforce_mapping_send_entity_to_queue($entity, $entity_type, 'CREATE');
}

/**
 * Implements hook_entity_update().
 */
function salesforce_genmap_entity_update($entity, $entity_type) {
  salesforce_mapping_send_entity_to_queue($entity, $entity_type, 'UPDATE');
}

/**
 * Implements hook_entity_delete().
 */
function salesforce_genmap_entity_delete($entity, $entity_type) {
  // Dequeue anything previous this entity may have sent to the queue.
  salesforce_mapping_dequeue_entity($entity, $entity_type);

  // If the entity needs to queue up its delete state, then queue its delete.
  salesforce_mapping_send_entity_to_queue($entity, $entity_type, 'DELETE');
}

/**
 * Send a given entity to a queue for a given operation.
 *
 * @param object $entity
 *   Entity to queue.
 * @param string $entity_type
 *   Type of entity being queued.
 * @param string $op
 *   Salesforce operation to be performed.
 *
 * @return bool
 *   TRUE if the item was successfully queued.  FALSE if the item could not be
 *   queued, or if the item was not supposed to be queued.
 */
function salesforce_mapping_send_entity_to_queue($entity, $entity_type, $op) {
  // Information specific to this entity.
  list($entity_id, , $bundle) = entity_extract_ids($entity_type, $entity);

  // Associate Salesforce operations with sync triggers.
  $op_sync = array(
    'CREATE' => SALESFORCE_MAPPING_SYNC_DRUPAL_CREATE,
    'UPDATE' => SALESFORCE_MAPPING_SYNC_DRUPAL_UPDATE,
    'DELETE' => SALESFORCE_MAPPING_SYNC_DRUPAL_DELETE,
  );

  // Grab the map for this particular entity.
  $mappings = salesforce_mapping_load_multiple(array(
    'drupal_entity_type' => $entity_type,
    'drupal_bundle' => $bundle,
  ));

  $map = NULL;
  $queue_item = FALSE;
  if (is_array($mappings)) {
    foreach ($mappings as $mapping) {
      // Check if this operation is configured to trigger a sync to Salesforce.
      if ($mapping->sync_triggers & $op_sync[$op]) {
        $queue_item = TRUE;
        $map = $mapping;
      }
    }
  }

  // Not all entities need to be queued, and this happens to be one of them.
  if (is_null($map) || !$queue_item) {
    return FALSE;
  }

  // Find the dedupe field.
  $dedupe_field = NULL;
  foreach ($map->field_mappings as $delta => $field_map) {
    if ($field_map['key']) {
      $dedupe_field = $field_map['salesforce_field']['name'];
    }
  }

  // When there is a dedupe field set and the action is to create a new record,
  // the operation needs to be UPSERT.
  $op = ($op == 'CREATE' && !is_null($dedupe_field)) ? 'UPSERT' : $op;

  // Create the queue item and queue it.
  $mapped_entity = salesforce_mapping_map_entity($entity, $entity_type, $map);
  $item = array(
    'drupal_id' => $entity_id,
    'module' => $entity_type,
    'delta' => $bundle,
    'object_type' => $map->salesforce_object_type,
    'operation' => $op,
    'dedupe_field' => $dedupe_field,
    'sobject' => $mapped_entity['sobject'],
    'sync_rules' => $mapped_entity['sync_rules'],
  );
  $queue = salesforce_queue_load();
  $result = $queue->createItem($item);
  if (!$result) {
    // @TODO Send an error to admin that submission didn't go to queue
    drupal_set_message(t('The submission was not sent to Salesforce.'), 'error');
  }
}

/**
 * Map an entity to a sobject and sync rules.
 *
 * @param object $entity
 *   Entity to queue.
 * @param string $entity_type
 *   Type of entity being queued.
 * @param object $map
 *   SalesforceMapping object of the entity that provides instructions of how
 *   to map to the Salesforce object.
 *
 * @return array
 *   An array keyed with 'sobject' containing an sobject and 'sync_rules' with
 *   an array of sync rules.
 */
function salesforce_mapping_map_entity($entity, $entity_type, $map) {
  $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);
  $fields = array();
  $sync_rules = array();
  foreach ($map->field_mappings as $field_map) {
    // Skip fields that aren't being pushed to Salesforce.
    if (!in_array($field_map['direction'], array(SALESFORCE_MAPPING_DIRECTION_DRUPAL_SF, SALESFORCE_MAPPING_DIRECTION_SYNC))) {
      continue;
    }
    $drupal_field_type = $field_map['drupal_field']['fieldmap_type'];
    $sf_field = $field_map['salesforce_field']['name'];

    $fieldmap_type = salesforce_mapping_get_fieldmap_types($drupal_field_type);
    $value = call_user_func($fieldmap_type['push_value_callback'], $field_map, $entity_wrapper);
    $fields[$sf_field] = $value;
    $sync_rules[$sf_field] = $field_map['drupal_sf'];
  }

  $sobject = new stdClass();
  $sobject->type = $map->salesforce_object_type;
  $sobject->record = ($map->salesforce_record_type == 'default') ? NULL : $map->salesforce_record_type;
  $sobject->fields = $fields;
  return array(
    'sobject' => $sobject,
    'sync_rules' => $sync_rules,
  );
}

/**
 * Remove an entity's items from the queue.
 *
 * @param object $entity
 *   Entity to queue.
 * @param string $entity_type
 *   Type of entity being queued.
 *
 * @return boolean
 *   TRUE if there are no items for the entity in the queue, FALSE if the delete
 *   process failed.
 */
function salesforce_mapping_dequeue_entity($entity, $entity_type) {
  // Information specific to this entity.
  list($entity_id, , $bundle) = entity_extract_ids($entity_type, $entity);

  // Grab the map for this particular entity.
  $mappings = salesforce_mapping_load_multiple(array(
    'drupal_entity_type' => $entity_type,
    'drupal_bundle' => $bundle,
  ));

  $map = NULL;
  if (is_array($mappings)) {
    foreach ($mappings as $mapping) {
      $map = $mapping;
    }
  }
  if (is_null($map)) {
    return TRUE;
  }

  $item = new stdClass();
  $item->drupal_id = $entity_id;
  $item->module = $entity_type;
  $item->delta = $bundle;
  $item->object_type = $map->salesforce_object_type;
  $queue = salesforce_queue_load();
  return $queue->deleteItem($item);
}

/**
 * @} salesforce_mapping_alter_queue
 */


/**
 * @name salesforce_mapping_alter_api Helper API the extension
 * @{
 */

/**
 * Wrapper for _sfm_get_default_value() to compensate for delta differences.
 *
 * @param string $field
 *   Name of the field who's value to retreive.
 * @param array $form_state
 *   Current state of the form to compare with.
 * @param mixed $delta
 *   Optionally provide the delta of the specific field map to check.
 *
 * @return mixed
 *   Value of the field that is appropriate for #default_value.
 */
function _sfm_alter_get_default_value($field, &$form_state, $delta = NULL) {
  $value = _sfm_get_default_value($field, $form_state, $delta);

  // The delta between stored mapping and the form input mapping are
  // incompatible so we'll figure out what the mapping's delta is and check the
  // default value again.
  if (is_null($value) && !is_null($delta)) {
    $mapping = NULL;
    if (isset($form_state['salesforce_mapping'])) {
      $mapping = &$form_state['salesforce_mapping'];
    }
    if (!empty($mapping)) {
      $map_delta = NULL;
      foreach ($mapping->field_mappings as $row_key => $row) {
        if (isset($row['drupal_field']['fieldmap_value']) && $row['drupal_field']['fieldmap_value'] == $delta) {
          $map_delta = $row_key;
        }
      }
      if (!is_null($map_delta)) {
        $value = _sfm_get_default_value($field, $form_state, $map_delta);
      }
    }
  }
  return $value;
}

/**
 * Helper to see if the given Salesforce field can be used as an idLookup.
 *
 * @param string $field
 *   The name of the Salesforce field to check.
 * @param string $salesforce_object_type
 *   The object type the field belongs to.
 * @param array $form_state
 *   Current state of the form to store and retreive results from to minimize
 *   the need for recalculation.
 *
 * @return bool
 *   TRUE if the field can be used as an idLookup, otherwise FALSE.
 */
function _sfm_salesforce_field_can_idlookup($field, $salesforce_object_type, $form_state) {
  if (empty($field) || empty($salesforce_object_type)) {
    return FALSE;
  }
  if (isset($form_state['sfm_storage']) && isset($form_state['sfm_storage']['idlookup']) && isset($form_state['sfm_storage']['idlookup'][$salesforce_object_type]) && isset($form_state['sfm_storage']['idlookup'][$salesforce_object_type][$field])) {
    return $form_state['sfm_storage']['idlookup'][$salesforce_object_type][$field];
  }
  $sfobject = _sfm_get_salesforce_object($salesforce_object_type, $form_state);
  foreach ($sfobject['fields'] as $field_info) {
    $form_state['sfm_storage']['idlookup'][$salesforce_object_type][$field] = $field_info['idLookup'];
    if ($field_info['name'] == $field) {
      return $field_info['idLookup'];
    }
  }
}

/**
 * @} salesforce_mapping_alter_api
 */
