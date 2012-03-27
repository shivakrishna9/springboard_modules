<?php
/**
 * @file
 * Admin functions for the Salesforce Node integration module.
 */

// Displays the Salesforce synchronization form.
function sf_node_salesforce_management_form(&$form_state, $nid) {
  $node = node_load($nid);
  
  // Fail out if the node didn't exist!
  if (!$node->nid) {
    drupal_not_found();
  }

  // Set the node page title.
  drupal_set_title(check_plain($node->title));

  $form = array();

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $nid,
  );

  // Display an export button if the node hasn't been exported before.
  if (!$node->salesforce['sfid']) {
    $form['export'] = array(
      '#type' => 'fieldset',
      '#title' => t('Export node to Salesforce'),
      '#description' => t('This node may be exported to Salesforce using any fieldmap listed below.'),
    );

    // Get an array of fieldmaps that export nodes of this type to Salesforce.
    $options = salesforce_management_api_fieldmap_options('node_'. $node->type);

    // If no corresponding fieldmaps were found...
    if (count($options) == 0) {
      // Display a message appropriate to the user's permissions.
      if (user_access('administer salesforce management')) {
        $form['export']['#description'] = t('To export this node you must first <a href="!url">add a fieldmap</a> and then set <a href="!dedupeurl">dedupe key</a> that exports @type nodes.', array('!url' => url(SALESFORCE_PATH_FIELDMAPS .'/add'), '@type' => $node->type,'!dedupeurl' => url(SALESFORCE_PATH_FIELDMAPS .'')));
      }
      else {
        $form['export']['#description'] = t('Please contact a site administrator to add a fieldmap that exports @type nodes.', array('@type' => $node->type));
      }
    }
    else {
      // Otherwise add the export form!
      $form['export']['fieldmap'] = array(
        '#type' => 'select',
        '#title' => t('Export fieldmap'),
        '#options' => $options,
      );

      $form['export']['export_node'] = array(
        '#type' => 'submit',
        '#value' => t('Export node'),
      );
    }
  }
  else {
    // Otherwise add synchronization information.
    $form['sfid'] = array(
      '#type' => 'value',
      '#value' => $node->salesforce['sfid'],
    );
    $form['fieldmap'] = array(
      '#type' => 'value',
      '#value' => $node->salesforce['fieldmap'],
    );
 	
    
	// Retrieve the object from Salesforce.
    $sf = salesforce_management_api_connect();
    $data = $sf->retrieve(array($node->salesforce['sfid']), $node->salesforce['fieldmap']);
	
	 // Load the fieldmap data.
    $map = salesforce_management_api_fieldmap_load($node->salesforce['fieldmap']);

    // Load the object definitions.
    $drupal_object = salesforce_management_api_fieldmap_objects_load('drupal', $map['drupal']);
    $object = salesforce_management_api_fieldmap_objects_load('salesforce', $map['salesforce']);

    $header = array(t('Field name'), t('Drupal @type value', array('@type' => salesforce_management_api_fieldmap_object_label('drupal', $map['drupal']))), t('Salesforce @type value', array('@type' => salesforce_management_api_fieldmap_object_label('salesforce', $map['salesforce']))));
    $rows = array();

    foreach ($map['fields'] as $key => $value) {
      if (isset($drupal_object['fields'][$value]['export'])) {
        $drupal_value = $drupal_object['fields'][$value]['export']($node, $value);
      }
      elseif (isset($node->$value)) {
        $drupal_value = $node->$value;
      }
      else {
        $drupal_value = '';
      }

      $rows[] = array(
        $object['fields'][$key]['label'],
        $drupal_value,
        $data->$key,
      );
    }

    $form['mapped'] = array(
      '#type' => 'fieldset',
      '#title' => t('Mapped field values'),
      '#description' => t('These fields have been mapped through <a href="!url">fieldmap @index</a>.', array('!url' => url(SALESFORCE_PATH_FIELDMAPS .'/'. $node->salesforce['fieldmap'] .'/edit'), '@index' => $node->salesforce['fieldmap'])),
    );
    $form['mapped']['fieldmap_values'] = array(
      '#value' => theme('table', $header, $rows),
    );

    // Create a table for the unmapped fields.
    $header = array(t('Field name'), t('Salesforce @type value', array('@type' => salesforce_management_api_fieldmap_object_label('salesforce', $map['salesforce']))));
    $rows = array();

    foreach ((array) $data as $key => $value) {
      if (!isset($map['fields'][$key]) && isset($object['fields'][$key])) {
        $rows[] = array(
          $object['fields'][$key]['label'],
          $value,
        );
      }
    }

    if (count($rows) > 0) {
      $form['unmapped'] = array(
        '#type' => 'fieldset',
        '#title' => t('Unmapped fields'),
        '#description' => t('These fields are available on Salesforce but are not currently mapped through the fieldmap used for this user.'),
      );
      $form['unmapped']['unmmaped_fields'] = array(
        '#value' => theme('table', $header, $rows),
      );
    }

    $rows = array();

    foreach (salesforce_management_api_fieldmap_system_fields() as $key => $value) {
      $rows[] = array(
        $value['label'],
        $data->$key,
      );
    }

    $form['system'] = array(
      '#type' => 'fieldset',
      '#title' => t('System fields'),
      '#description' => t('These fields provide additional system information about the Salesforce object but cannot be exported to Salesforce.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['system']['system_fields'] = array(
      '#value' => theme('table', $header, $rows),
    );

    $form['raw'] = array(
      '#type' => 'fieldset',
      '#title' => t('Raw data'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['raw']['data'] = array(
      '#value' => '<pre>'. print_r($data, TRUE) .'</pre>',
    );
  }

  return $form;
}