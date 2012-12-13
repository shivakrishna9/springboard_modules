<?php

/**
 * @file
 * Configuration page for this module
 */

/**
 * Page callback to render the admin page
 */
function salesforce_webform_admin_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'salesforce-webform-admin-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>'. t('Configure how the Salesforce integration will work with webform submissions.') .'</p>',
      '#prefix' => '<div class="salesforce-webform-admin-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_webform_admin_form' => drupal_get_form('salesforce_webform_admin_form'),
  );
  return $build;
}

function salesforce_webform_admin_form($form, &$form_state) {
  $form = array();

  // Queue system options
  $queue_options = array(
    'instant' => t('Send Instantaneously'),
    'drupal_queue' => t('Drupal Queue'),
  );
  $salesforce_queue_exists = module_exists('salesforce_queue');
  if ($salesforce_queue_exists) {
    $queue_options['salesforce_queue'] = t('Salesforce Queue');
  }

  // Default queue
  $queue_system = variable_get('salesforce_webform_queue_system', '');
  if (empty($queue_system)) {
    $queue_system = $salesforce_queue_exists ? 'salesforce_queue' : 'instant';
  }

  // Queue system form
  $form['salesforce_webform_queue_system'] = array(
    '#id' => 'salesforce-webform-queue-system',
    '#title' => t('Queue System'),
    '#description' => t(''),
    '#type' => 'radios',
    '#default_value' => $queue_system,
    '#options' => $queue_options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

function salesforce_webform_admin_form_submit($form, &$form_state) {
  $former_queue_system = variable_get('salesforce_webform_queue_system', '');
  $queue_system = $form_state['input']['salesforce_webform_queue_system'];
  variable_set('salesforce_webform_queue_system', $queue_system);

  // @TODO: Decommission old queue
  if (!empty($former_queue_system)) {
    // Switch existing queue data to the new queue and delete old queue
  }

  drupal_set_message(t('Your settings have been saved.'));
}
