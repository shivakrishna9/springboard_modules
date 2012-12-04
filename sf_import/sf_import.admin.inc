<?php

/**
 * @file
 * Admin functions for sf_import.
 */

function sf_import_settings_form($form_state, $settings) {
  // get the drupal object for this fieldmap
  $results = db_query("SELECT salesforce, drupal FROM {salesforce_management_field_map} WHERE fieldmap = %d", $settings['fieldmap']);
  while ($data = db_fetch_object($results)) {
    $salesforce_object = $data->salesforce;
    $drupal_content_type = $data->drupal;
  }

  $fields = array();

  // get fields from salesforce
  $sf = salesforce_management_api_connect();
  if ($sf) {
    $object = $sf->client->describeSObject($salesforce_object);
    foreach ($object->fields as $field) {
      $fields[$field->name] = $field->label . ' (' . $field->name . ')';
    }

    asort($fields);

    $frequency = $settings['frequency'];
    $frequency_type = $settings['frequency_type'];
    $where_clause = $settings['filters'];
    $updated_objects_only = $settings['updated_objects_only'];
    $import_new_objects = $settings['import_new_objects'];
    $last_import = $settings['last_import'];
    $next_import = $settings['next_import'];
    $is_new = empty($settings['frequency']);

    $form['#tree'] = TRUE;

    $form['last_import'] = array(
      '#type' => 'hidden',
      '#value' => $last_import,
    );

    $form['next_import'] = array(
      '#type' => 'hidden',
      '#value' => $next_import,
    );

    $form['is_new'] = array(
      '#type' => 'hidden',
      '#value' => $is_new,
    );

    $form['info'] = array(
      '#prefix' => '<h4>',
      '#value' => $salesforce_object . ' to ' . $drupal_content_type,
      '#suffix' => '</h4>',
    );

    $form['frequency'] = array(
      '#type' => 'fieldset',
      '#title' => t('Frequency'),
      '#description' => t('How often should salesforce be checked for new records.'),
    );

    $form['frequency']['interval'] = array(
      '#type' => 'textfield',
      '#title' => 'Every',
      '#size' => 5,
      '#default_value' => !empty($frequency) ? $frequency : 1,
    );

    $form['frequency']['interval_type'] = array(
      '#type' => 'radios',
      '#options' => array(
        'minutes' => 'minutes',
        'hours' => 'hours',
        'days' => 'days',
      ),
      '#default_value' => !empty($frequency_type) ? $frequency_type : 'days',
    );

    $form['criteria'] = array(
      '#type' => 'fieldset',
      '#title' => t('Criteria'),
      '#description' => t('Add criteria to filter the objects you want to import.'),
    );

    $form['criteria']['where_clause'] = array(
      '#type' => 'textarea',
      '#default_value' => $where_clause,
    );

    $form['criteria']['updated_objects_only'] = array(
      '#type' => 'checkbox',
      '#title' => t('Only include objects that have been updated in Salesforce since the time of the last import'),
      '#default_value' => $updated_objects_only,
      '#description' => t('When checked, only those objects that were updated in Salesforce since the last time an import was performed will be selected.'),
    );

    $form['criteria']['import_new_objects'] = array(
      '#type' => 'checkbox',
      '#title' => t('Import objects that do not already exist in Drupal'),
      '#default_value' => $import_new_objects,
      '#description' => t('When checked, new Drupal objects will be created when a Salesforce object is downloaded that does not already exist.'),
    );

    $form['criteria']['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Available fields'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );


    foreach ($fields as $name => $label) {
      $field_list .= '<li>' . $label . '</li>';
    }

    $form['criteria']['fields']['object'] = array(
      '#type' => 'markup',
      '#prefix' => '<ul>',
      '#value' => $field_list,
      '#suffix' => '</ul>',
    );

    $form['save'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
    );

    $form['delete'] = array(
      '#type' => 'submit',
      '#submit' => array('sf_import_settings_delete'),
      '#value' => 'Delete',
    );
  }
  else {
    $form['notice'] = array(
      '#value' => '<h3>Unable to connect to salesforce. Please check your settings.</h3>',
    );
  }

  return $form;

}

/**
 * Deletes import settings.
 */
function sf_import_settings_delete($form, &$form_state) {

  db_query("DELETE FROM {sf_import_schedule} WHERE fieldmap = %d", arg(4));
  drupal_set_message(t('Import settings for fieldmap %fieldmap deleted.', array('%fieldmap' => arg(4))));
  drupal_goto(SALESFORCE_PATH_FIELDMAPS);

}

/**
 * Form submission handler. Saves import settings to databaes
 */
function sf_import_settings_form_submit($form, &$form_state) {
  $table = 'sf_import_schedule';
  $record = new stdClass();
  $record->fieldmap = arg(4);
  $record->filters = $form_state['values']['criteria']['where_clause'];
  $record->updated_objects_only = $form_state['values']['criteria']['updated_objects_only'];
  $record->import_new_objects = $form_state['values']['criteria']['import_new_objects'];
  $record->frequency = $form_state['values']['frequency']['interval'];
  $record->frequency_type = $form_state['values']['frequency']['interval_type'];

  // make sure next_import isn't later than now + periodicity
  $max_next_import = strtotime('+' . $record->frequency . ' ' . $record->frequency_type);
  if (!($form_state['values']['next_import']) || $form_state['values']['next_import'] > $max_next_import) {
    $record->next_import = $max_next_import;
  }

  // warn if periodicity is less than interval since last cron
  if (($cron_last = variable_get('cron_last', FALSE)) && (time() - $cron_last > $max_next_import - time())) {
    drupal_set_message(
      t('The last cron run was %interval ago, so it may not be running frequently enough for @frequency-@unit imports.',
        array('%interval' => format_interval(time() - $cron_last), '@frequency' => $record->frequency, '@unit' => substr($record->frequency_type, 0, -1))
      ),
      'warning'
    );
  }

  if ($form_state['values']['is_new']) {
    drupal_write_record($table, $record);
  }
  else {
    drupal_write_record($table, $record, 'fieldmap');
  }

  drupal_set_message('Import settings saved.');

}
