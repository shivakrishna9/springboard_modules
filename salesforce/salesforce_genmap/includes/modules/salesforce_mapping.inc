<?php

/**
 * @file
 * Extends salesforce_mapping functionality.
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
        'sf_drupal' => t('Overwrite rule when importing into Drupal'),
        'drupal_sf' => t('Overwrite rule when exporting to Salesforce'),

        // Column 'direction' is hidden to enforce the value 'sync'.
        'direction' => '',

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
      $row['sf_drupal'] = $row['drupal_sf'] = array(
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
        '#value' => 'sync',
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

  $form['#submit'][] = 'salesforce_mapping_form_alter_submit';
}

/**
 * Submit callback to handle extra data from the altered mapping form.
 */
function salesforce_mapping_form_alter_submit($form, &$form_state) {
  // The original callback, salesforce_mapping_form_submit(), has already saved
  // the entity, so we'll reload it and save it again adding in our new values.
  $machine_name = _sfm_alter_get_default_value('name', $form_state);
  $mapping = salesforce_mapping_load($machine_name);
  $key = _sfm_alter_get_default_value('key', $form_state);
  foreach ($mapping->field_mappings as $delta => $field_map) {
    $mapping->field_mappings[$delta]['key'] = ($key == $field_map['drupal_field']['fieldmap_value']);
    $mapping->field_mappings[$delta]['sf_drupal'] = _sfm_alter_get_default_value('sf_drupal', $form_state, $field_map['drupal_field']['fieldmap_value']);
    $mapping->field_mappings[$delta]['drupal_sf'] = _sfm_alter_get_default_value('drupal_sf', $form_state, $field_map['drupal_field']['fieldmap_value']);
  }
  $mapping->save();
}

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
