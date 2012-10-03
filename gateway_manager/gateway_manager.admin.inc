<?php

/**
 * @file
 * Admin functionality for the gateway manager module.
 */

/**
 * Menu callback. Displays a list of all configured payment gateways.
 */
function gateway_manager_list() {
  $gateway_types = module_invoke_all('gateway_manager_gateway_info');
  $gateways = gateway_manager_load_gateways();
  $data = array();
  foreach ($gateways as $gateway) {
    $this_gateway = array();
    $this_gateway['name'] = l($gateway['name'], 'admin/config/system/fundraiser/gateways/' . $gateway['id'] . '/edit');
    $this_gateway['type'] = $gateway_types[ $gateway['type'] ]['title'];
    $this_gateway['form_count'] = _gateway_manager_form_count($gateway['id']);
    $user = user_load($gateway['uid']);
    $this_gateway['user'] = $user->name;
    $this_gateway['created'] = $gateway['created'];
    $this_gateway['updated'] = $gateway['updated'];
    $this_gateway['status'] = $gateway['status'] == 1 ? t('Active') : t('Inactive');
    $data[] = $this_gateway;
  }
  $output = '';
  $output .= theme('table', array('header' => array('Name', 'Provider', '# of Forms', 'User', 'Created', 'Updated', 'Status'), 'rows' => $data));
  return $output;
}

// TODO replace this with a view!

/**
 * Menu callback. Displays the gateway settings form when editing an existing gateway.
 */
function gateway_manager_gateway_edit($gateway) {
  return drupal_get_form('gateway_manager_edit_form', $gateway);
}

/**
 * Menu callback. Displays the gateway settings form when adding a new gateway.
 */
function gateway_manager_gateway_add() {
  return drupal_get_form('gateway_manager_edit_form', NULL);
}

/**
 * Form for editing payment gateways.
 */
function gateway_manager_edit_form($form, &$form_state = NULL, $gateway = NULL) {
  // First we check vs the gateway, if it's set use that as a default.
  $type = isset($gateway->type) ? $gateway->type : '';
  $name = isset($gateway->name) ? $gateway->name : '';
  $terms = isset($gateway->terms) ? $gateway->terms : '';
  $status = isset($gateway->status) ? $gateway->status : '';
  $config = array();
  if (!empty($gateway)) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $gateway->id,
    );
    $config = unserialize($gateway->config);
  }
  // Now that defaults are set, create the form.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#default_value' => $name,
    '#description' => t('The recognizable name of the payment gateway.'),
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#default_value' => $status,
    '#return_value' => 1,
    '#description' => t('If this gateway is an active option for fundraiser donation forms.'),
  );
  $disabled = FALSE;
  if (!empty($gateway)) {
    $disabled = TRUE;
  }
  $gateway_types = module_invoke_all('gateway_manager_gateway_info');
  $gateways = array('' => '--select--');
  foreach ($gateway_types as $id => $gateway_type) {
    $gateways[ $id ] = $gateway_type['title'];
  }
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Gateway type'),
    '#required' => TRUE,
    '#options' => $gateways,
    '#default_value' => $type,
    '#disabled' => $disabled,
    '#description' => t('Select the type of gateway to create.'),
  );

  // Loop for each type, pull the relevant form in.
  // Set visibility for the type form by state of selected type.
  foreach ($gateway_types as $id => $gateway_type) {
    // If we haven't been locked in, show all.
    // If we have, show only the relevant one.
    if (!$disabled || $type == $id) {
      $settings_function = isset($gateway_type['settings']) ? $gateway_type['settings'] : '';
      if (function_exists($settings_function)) {
        $title = isset($gateway_type['title']) ? $gateway_type['title'] : '';
        $form['gateway_settings_' . $id] = array(
          '#type' => 'fieldset',
          '#title' => t('@title settings', array('@title' => $title)),
          '#states' => array(
            'visible' => array(
              ':input[name=type]' => array('value' => $id ),
            ),
          ),
        );
        $gateway_settings = $settings_function($config);
        if (is_array($gateway_settings)) {
          $form['gateway_settings_' . $id]['settings'] = $gateway_settings;
        }
      }
    }
  }
  $form['taxononmy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $vid = _gateway_manager_get_vid();
  $available_terms = array();
  $found_terms = taxonomy_term_load_multiple(array(), array('vid' => $vid));
  foreach ($found_terms as $tid => $found_term) {
    $available_terms[$tid] = $found_term->title;
  }
  $form['taxononmy']['terms'] = array(
    '#type' => 'select',
    '#title' => t('Terms'),
    '#options' => $available_terms,
    '#default_value' => $terms,
    '#multiple' => 1,
    '#description' => t('Categorize this payment gateway by selecting applicable terms.'),
  );
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  if (isset($form['id']['#value']) && gateway_manager_is_deleteable($form['id']['#value'])) {
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  elseif (isset($form['id']['#value'])) {
    $form['buttons']['delete_notice'] = array(
      '#value' => t('* This payment gateway cannot be deleted because donation forms are assigned to it' .
        ' or there are pending recurring donations.'),
      '#prefix' => '<div class="description">',
      '#suffix' => '</div>',
    );
  }
  return $form;
}

/**
 * Submission handler for saving gateway configurations.
 */
function gateway_manager_edit_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == 'Delete') {
    drupal_goto('admin/config/system/fundraiser/gateways/' . $form_state['values']['id'] . '/delete');
  }
  $record = $form_state['values'];
  $exclusions = array('name', 'type', 'op', 'submit', 'terms', 'delete', 'form_build_id', 'form_token', 'form_id');
  $config = array();
  // Loop over form values and only save those.
  foreach ($form_state['values'] as $k => $v) {
    if (!in_array($k, $exclusions)) {
      $config[$k] = $v;
    }
  }
  $record['config'] = serialize($config);
  gateway_manager_settings_update($record);
}


/**
 * Menu callback. Payment gateway delete confirmation form.
 */
function gateway_managed_gateway_delete_form(&$form_state, $gateway) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $gateway->id,
  );
  return confirm_form($form, t('Are you sure you want to delete the payment gateway %name?',
    array('%name' => $gateway->name)), 'admin/config/system/fundraiser/gateways',
    t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Submit the confirmation form.
 */
function gateway_managed_gateway_delete_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  if ($form_state['values']['confirm']) {
    gateway_manager_delete_gateway($id);
  }
}

/**
 * Returns the vocabulary id of the taxonomy in charge of handling payment gateway terms.
 * Creates the vocabulary if it doesn't already exist.
 */
function _gateway_manager_get_vid() {
  $vid = variable_get('gateway_manager_vocabulary', '');
  if (empty($vid) || is_null(taxonomy_vocabulary_load($vid))) {
    $vid = db_query('SELECT vid FROM {taxonomy_vocabulary} WHERE module = :module',
      array(':module' => 'gateway_manager'))->fetchField();
    if (!$vid) {
      $vocabulary = (object) array(
        'name' => t('Payment Gateway Terms'),
        'machine_name' => 'payment_gateway_terms',
        'description' => t('Managed gateways'),
        'hierarchy' => '0',
        'module' => 'gateway_manager',
      );
      taxonomy_vocabulary_save($vocabulary);
      $vid = $vocabulary->vid;
    }
    variable_set('gateway_manager_vocabulary', $vid);
  }
  return $vid;
}
