<?php

/**
 * @file
 * Admin functionality for the gateway manager module.
 */

/**
 * Menu callback. Displays a list of all configured payment gateways.
 */
function gateway_manager_list() {
  //$filter = gateway_manager_filter_query();
  //$pager_num = 0;
  //$count = 20;
  $gateway_types = module_invoke_all('gateway_manager_gateway_info');
  $gateways = gateway_manager_load_gateways();
  
  //$query = db_select('SELECT g.id, g.name, g.type, u.name as user, from_unixtime(g.created) as created, ' .
  //  'from_unixtime(g.updated) as updated, g.status ' .
  //  'FROM {gateway_manager_gateways} g ' .
  //  'LEFT OUTER JOIN users u on u.uid = g.uid ' .
  //  $filter['where'])->extend('PagerDefault');
  //$results = $query->limit(20)->execute();
  //    $count, $pager_num, NULL, $filter['args']);
  $data = array();
  foreach ($gateways as $gateway) {
    $this_gateway = array();
    $this_gateway['name'] = l($gateway['name'], 'admin/config/system/fundraiser/gateways/' . $gateway['id'] . '/edit');
    $this_gateway['type'] = $gateway_types[ $gateway['type'] ]['title'];
    $this_gateway['form_count'] = _gateway_manager_form_count($gateway['id']);
    $this_gateway['user'] = $gateway['user'];
    $this_gateway['created'] = $gateway['created'];
    $this_gateway['updated'] = $gateway['updated'];
    $this_gateway['status'] = $gateway['status'] == 1 ? t('Active') : t('Inactive');
    $data[] = $this_gateway;
  }
  $output = '';
  //$output = drupal_get_form('gateway_manager_list_filter_form');
  $output .= theme('table', array('header' => array('Name', 'Provider', '# of Forms', 'User', 'Created', 'Updated', 'Status'), 'rows' => $data));
  //$output .= theme('pager', array('tags' => NULL, 'element' => $pager_num));
  return $output;
}

// TODO replace this with a view??

/**
 * Creates a form used to filter the master payment gateway list by provider.

function gateway_manager_list_filter_form($form) {
  $gateways = module_invoke_all('gateway_manager_gateway_info');
  $options = array(t('-- Select --'));
  foreach ($gateways as $gateway) {
    $options[$gateway['id']] = $gateway['title'];
  }
  $form['filter'] = array(
    '#type' => 'select',
    '#title' => t('Filter by gateway provider'),
    '#options' => $options,
    '#default_value' => $_SESSION['gateway_manager_filter'], // Coder: This is ok.
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  $form['clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear'),
  );
  return $form;
}
 */
 
/**
 * Saves the selected gateway filter when the form is submitted.

function gateway_manager_list_filter_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == 'Filter' && !empty($form_state['values']['filter'])) {
    $_SESSION['gateway_manager_filter'] = $form_state['values']['filter']; // Coder: This is ok.
  }
  else {
    unset($_SESSION['gateway_manager_filter']); // Coder: This is ok.
  }
}
*/

/**
 * Returns a filter and related args to be applied to the payment gateway list page.

function gateway_manager_filter_query() {
  $where = $args = array();
  if (isset($_SESSION['gateway_manager_filter'])) { // Coder: This is ok.
    $where[] = "g.type = '%s'";
    $args[] = $_SESSION['gateway_manager_filter']; // Coder: This is ok.
  }
  $where = count($where) ? 'WHERE ' . implode(' AND ', $where) : '';
  return array('where' => $where, 'args' => $args);
}
 */

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
  // Then we check if the form settings or storage needs to override that for display.
  $type = isset($form_state['values']) ? $form_state['values']['type'] : $type;
  $name = isset($form_state['values']) ? $form_state['values']['name'] : $name;
  $terms = isset($form_state['values']) ? $form_state['values']['terms'] : $terms;
  $status = isset($form_state['values']) ? $form_state['values']['status'] : $status;
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
  foreach ($gateway_types as $gateway_type) {
    $gateways[ $gateway_type['id'] ] = $gateway_type['title'];
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
  //REVISE THIS TO USE STATES. CAUSE WE CAN. -TODO.
  //http://randyfay.com/states
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
  // If type has been selected, get the settings form for that type. Otherwise, provide continue button.
  if (!empty($type)) {
    $title = isset($gateway_types[$type]['title']) ? $gateway_types[$type]['title'] : '';
    $settings_function = isset($gateway_types[$type]['settings']) ? $gateway_types[$type]['settings'] : '';
    if (function_exists($settings_function)) {
      $gateway_settings = $settings_function($config);
      if (is_array($gateway_settings)) {
        $form['settings'] = array(
          '#type' => 'fieldset',
          '#title' => t('@title settings', array('@title' => $title)),
          '#collapsible' => TRUE,
        );
        $form['settings'][] = $gateway_settings;
      }
    }
    return gateway_mananger_settings_form($form);
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Continue',
    );
    return $form;
  }
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
