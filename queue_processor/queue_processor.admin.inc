<?php
/**
 * @file
 * Administration page callbacks for the queue processor module.
 */

/**
 * Allows a user to add all objects of a given type to the queue
 */
function queue_processor_mass_add() {

  $types = array();
  if (module_exists('sf_user')) {
    $types['user'] = 'Users';
  }

  /* TODO: Future enhancement
  if (module_exists('sf_webform')) {
    $types['webform'] = 'Webform submissions';
  }
  */

  $form['warning'] = array(
    '#value' => t('Please be sure all required <strong>fieldmaps</strong> are configured correctly before using this utility'),
  );

  $form['objects_to_add'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Types to bulk insert'),
    '#description' => t('Choose the types of objects you want to add to the queue. Only those objects that have not yet been exported will be added.'),
    '#options' => $types,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add objects to queue',
  );

  return $form;
}

/**
 * Submission functions for mass add
 */
function queue_processor_mass_add_submit($form, &$form_state) {

  foreach($form_state['values']['objects_to_add'] as $type) {
    if ($type) {
      switch ($type) {
        case 'user' :
          db_query("INSERT INTO {sf_queue} (oid, sfid, type, fieldmap, action, created, locked)
                    SELECT uid, NULL, 'user', 0, 'upsert', unix_timestamp(), 0
                    FROM {users}
                    WHERE uid NOT IN (SELECT oid FROM {salesforce_management_object_map} WHERE drupal_type = 'user') AND uid != 0
                    AND uid NOT IN (SELECT oid FROM {sf_queue} WHERE TYPE = 'user')");
          drupal_set_message('All users that haven\'t been exported to Salesforce have been added to the queue');
          break;

        case 'webform' :
          print 'mass add webforms<br>';
          break;
      }
    }
  }
}


/**
 * Form builder. Configure queue processor.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function queue_processor_admin_settings() {
  $form['queue_max_to_process'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of items to process each cron run'),
    '#default_value' => variable_get('queue_max_to_process', 1000),
    '#description' => t('The maximum number of items that will be processed each time cron runs. This number should be tuned for maximum performance.'),
    '#size' => 6,
  );

  $form['queue_retry_max'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum retry attempts'),
    '#default_value' => variable_get('queue_retry_max', 3),
    '#description' => t('The maximum number of times to retry a failed object before removing it from the queue.'),
    '#size' => 2,
  );

  $form['queue_heap_destination'] = array(
    '#type' => 'select',
    '#title' => t('Unrecoverable error destination'),
    '#description' => t('Where items get moved to in the event of an unrecoverable error.'),
    '#options' => array('queue' => 'Queue', 'retry queue' => 'Retry queue'),
    '#default_value' => variable_get('queue_heap_destination', 'queue'),
  );

  $form['queue_retry_delay'] = array(
    '#type' => 'select',
    '#title' => t('Retry queue delay'),
    '#description' => t('The number of hours to wait before an item is processed again.'),
    '#options' => array(2 => '2 hours', 6 => '6 hours', 12 => '12 hours', 24 => '24 hours'),
    '#default_value' => variable_get('queue_retry_delay', 24),
  );

  $form['queue_summary_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Send email summary to'),
    '#description' => t('Send a detailed summary after each cron run to this email address.'),
    '#default_value' => variable_get('queue_summary_email', NULL),
  );

  $processing_order = variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node'));

  $form['type_processing_order']['#tree'] = TRUE;
  $tree = &$form['type_processing_order'];

  foreach($processing_order as $key => $type) {
    $tree[$type]['title'] = array(
      '#value' => ucfirst($type),
    );
    $tree[$type]['weight'] = array(
      '#type' => 'weight',
      '#delta' => count($processing_order),
      '#default_value' => $key,
    );
  }

  $form['batch_sizes'] = array(
    '#type' => 'fieldset',
    '#title' => 'Batch sizes',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  foreach($processing_order as $key => $type) {
    $form['batch_sizes'][$type . '_batch_size'] = array(
      '#type' => 'textfield',
      '#title' => 'Batch size for ' . $type,
      '#default_value' => variable_get($type . '_batch_size', 200),
      '#size' => 10,
    );
  }

  $form['queue_processor_cron'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure cron settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['queue_processor_cron']['queue_processor_standalone_cron_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable standalone cron.'),
    '#description' => t('If this option is enabled all fundraiser related cron tasks will be removed from the standard cron run. These tasks will need to be cronned separately via sitename/queue_processor_cron'),
    '#default_value' => variable_get('queue_processor_standalone_cron_enabled', 0),
  );

  $form['queue_processor_validation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Validation settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['queue_processor_validation']['queue_processor_valid_booleans'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapped values to be considered true'),
    '#description' => t('Enter a comma delimited list of values that will be converted to a true boolean value when exported to Salesforce.'),
    '#default_value' => variable_get('queue_processor_valid_booleans', 'true,yes,y,1'),
  );

  $form['queue_processor_batch_limits'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log limits'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['queue_processor_batch_limits']['queue_processor_sf_batch_row_limit'] = array(
    '#type' => 'select',
    '#title' => t('Discard batch entries above the following row limit'),
    '#default_value' => variable_get('queue_processor_sf_batch_row_limit', 10000),
    '#options' => drupal_map_assoc(array(5000, 10000, 25000, 50000, 75000, 100000)),
    '#description' => t('The maximum number of rows to keep in the batch log.')
  );

  $form['queue_processor_batch_limits']['queue_processor_sf_batch_error_row_limit'] = array(
    '#type' => 'select',
    '#title' => t('Discard batch error entries above the following row limit'),
    '#default_value' => variable_get('queue_processor_sf_batch_error_row_limit', 10000),
    '#options' => drupal_map_assoc(array(5000, 10000, 25000, 50000, 75000, 100000)),
    '#description' => t('The maximum number of rows to keep in the batch error log.')
  );

  // Actions
  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => 'Save configuration',
    '#submit' => array('queue_processor_admin_settings_submit'),
  );

  $form['rest'] = array(
    '#type'   => 'submit',
    '#value'  => 'Reset to defaults',
    '#submit' => array('queue_processor_admin_settings_reset'),
  );

  return $form;
}

/**
 * Validates queue processor settings.
 */
function queue_processor_admin_settings_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['queue_max_to_process'])) {
    form_set_error('queue_max_to_process', t('Maximum number to process must be numeric.'));
  }

  if (!is_numeric($form_state['values']['queue_retry_max'])) {
    form_set_error('queue_retry_max', t('Queue retry max must be numeric.'));
  }

  // check batch size for each type
  $processing_order = variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node'));
  foreach($processing_order as $key => $type) {
    $size = $form_state['values'][$type . '_batch_size'];
    // make sure the value is numeric
    if (!is_numeric($size)) {
      form_set_error('batch_sizes][' . $type . '_batch_size', t(ucfirst($type) . ' batch size must be numeric.'));
    }
    // make sure size is 1 - 200 (Salesforce doesn't allow batch size > 200)
    if ($size < 1 || $size > 200) {
      form_set_error('batch_sizes][' . $type . '_batch_size', t(ucfirst($type) . ' batch size must be between 1 and 200.'));
    }

  }
}

/**
 * Submit action for settings page
 */
function queue_processor_admin_settings_submit($form, &$form_state) {
    $cron_enabled = variable_get('queue_processor_standalone_cron_enabled', 0);
    if ($form_state['values']['queue_processor_standalone_cron_enabled'] != $cron_enabled) {
      variable_set('queue_processor_standalone_cron_enabled', $form_state['values']['queue_processor_standalone_cron_enabled']);
      menu_rebuild();
    }
  $ordered_types = $form_state['values']['type_processing_order'];

  // sort the types based on weights
  uasort($ordered_types, "sort_weights");

  variable_set('queue_max_to_process', $form_state['values']['queue_max_to_process']);
  variable_set('queue_processing_order', array_keys($ordered_types));
  variable_set('queue_retry_max', $form_state['values']['queue_retry_max']);
  variable_set('queue_summary_email', $form_state['values']['queue_summary_email']);
  variable_set('queue_heap_destination', $form_state['values']['queue_heap_destination']);
  variable_set('queue_retry_delay', $form_state['values']['queue_retry_delay']);
  variable_set('queue_processor_valid_booleans', $form_state['values']['queue_processor_valid_booleans']);
  variable_set('queue_processor_sf_batch_row_limit', $form_state['values']['queue_processor_sf_batch_row_limit']);
  variable_set('queue_processor_sf_batch_error_row_limit', $form_state['values']['queue_processor_sf_batch_error_row_limit']);

  // save batch sizes per type
  $processing_order = variable_get('queue_processing_order', array('user', 'donation', 'webform', 'node'));
  foreach($processing_order as $key => $type) {
    $name = $type . '_batch_size';
    $value = $form_state['values'][$name];
    variable_set($name, $value);
  }

  drupal_set_message(t('Settings have been saved.'));
}

/**
 * Resets queue processing settings to their default values
 */
function queue_processor_admin_settings_reset($form, &$form_state) {
  variable_set('queue_processing_order', array('user', 'donation', 'webform', 'node'));
  variable_set('queue_retry_max', 3);
  variable_set('queue_summary_email', NULL);
  variable_set('queue_heap_destination', 'queue');
  variable_set('queue_retry_delay', 24);
  variable_set('queue_processor_valid_booleans', 'true,yes,y,1');
  variable_set('queue_processor_sf_batch_row_limit', 10000);
  variable_set('queue_processor_sf_batch_error_row_limit', 10000);

  drupal_set_message(t('Settings have been reset.'));
}

function sort_weights($a, $b) {
  if ($a['weight'] == $b['weight']) {
    return 0;
  }
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Theme admin settings form
 */
function theme_queue_processor_admin_settings($form) {
  drupal_add_tabledrag('processing-order', 'order', 'sibling', 'processing-element');

  // Header
  $header = array(
    t('Processing Order'),
    t('Weight'),
  );

  // Rows for the tabledrag
  $rows = array();

  $tree = &$form['type_processing_order'];
  foreach (element_children($tree) as $key) {
    $tree[$key]['weight']['#attributes']['class'] = 'processing-element';
    $row = array();
    $row[] = drupal_render($tree[$key]['title']);
    $row[] = drupal_render($tree[$key]['weight']);

    $rows[] = array('data' => $row, 'class' => 'draggable');
  }

  // Build the output
  $output = '<div>' . t('Drag the types to rearrange the order in which they process.') . '</div>';
  $output .= theme('table', $header, $rows, array('id' => 'processing-order'));
  $output .= drupal_render($form);

  return $output;
}
