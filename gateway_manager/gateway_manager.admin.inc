<?php

/**
 * @file
 * Admin functionality for the gateway manager module. 
 */
 
/**
 * Displays a list of all configured payment gateways.
 */
function gateway_manager_list() {
  $pager_num = 0;
  $limit = 20;
  
  // get all the gateways so we can swap in a nice title
  $gateways = _managed_gateway_list();
  $query = db_select('gateway_manager_gateways', 'g')
    ->fields('g', array('id', 'name', 'type', 'status'))
    ->extend('PagerDefault')
  ;
  $query->limit($limit);
  $query->leftJoin('users', 'u', 'u.uid = g.uid');
  $query->addField('u', 'name', 'user');
  $query->addExpression('FROM_UNIXTIME(g.created)', 'created');
  $query->addExpression('FROM_UNIXTIME(g.updated)', 'updated');
  $query->gateway_manager_filter_query();
  $result = $query->execute();
      
  while ($row = $result->fetchAssoc()) {
    $gateway = array();
    $gateway['name'] = l($row['name'], 'admin/settings/fundraiser/gateways/' . $row['id'] . '/edit');
    $gateway['type'] = _gateway_manager_get_gateway_title($gateways, $row['type']);;
    $gateway['form_count'] = _gateway_manager_form_count($row['id']);
    $gateway['user'] = $row['user'];
    $gateway['created'] = $row['created'];
    $gateway['updated'] = $row['updated'];
    $gateway['status'] = $row['status'] == 1 ? t('Active') : t('Inactive'); 
    $data[] = $gateway;
  }
  $output = drupal_get_form('gateway_manager_list_filter_form');
  $output .= theme('table', array('Name', 'Provider', '# of Forms', 'User', 'Created', 'Updated', 'Status'), $data);
  $output .= theme('pager', NULL, $count , $pager_num);
  return $output;
}

/**
 * Returns the friendly title of a specified gateway type.
 */
function _gateway_manager_get_gateway_title($gateways, $type) {
  foreach ($gateways as $gateway) {
    if ($gateway['id'] == $type) {
      return $gateway['title'];
    }
  }
  return t('Unknown');
}

/**
 * Creates a form used to filter the master payment gateway list by provider.
 */
function gateway_manager_list_filter_form() {
  $gateways = _managed_gateway_list();
  $options = array(t('-- Select --'));
  foreach ($gateways as $gateway) {
    $options[$gateway['id']] = $gateway['title'];
  }
  $form['filter'] = array(
    '#type' => 'select',
    '#title' => t('Filter by gateway provider'),
    '#options' => $options,
    '#default_value' => $_SESSION['gateway_manager_filter'],
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

/**
 * Saves the selected gateway filter when the form is submitted.
 */
function gateway_manager_list_filter_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == 'Filter' && !empty($form_state['values']['filter'])) {
    $_SESSION['gateway_manager_filter'] = $form_state['values']['filter'];
  }
  else {
    unset($_SESSION['gateway_manager_filter']);
  }
}

/**
 * Adds filter(s) to the payment gateway list page query as needed.
 */
function gateway_manager_filter_query($query) {
  if (isset($_SESSION['gateway_manager_filter'])) {
    $query->condition('g.type', $_SESSION['gateway_manager_filter'], '=');
  }
  return $query;
}

/**
 * Displays the gateway settings form when editing an existing gateway.
 */
function gateway_manager_gateway_edit($gateway) {
  return drupal_get_form('gateway_manager_edit_form', $gateway);
}

/**
 * Displays the gateway settings form when adding a new gateway.
 */
function gateway_manager_gateway_add() {
  return drupal_get_form('gateway_manager_edit_form', NULL);
}

/**
 * Form for editing payment gateways.
 */
function gateway_manager_edit_form(&$form_state = NULL, $gateway) {

  $type = isset($form_state['values']) ? $form_state['storage']['type'] : $gateway->type;
  $name = isset($form_state['values']) ? $form_state['storage']['name'] : $gateway->name;
  $terms = isset($form_state['values']) ? $form_state['storage']['terms'] : $gateway->terms;
  $status = isset($form_state['values']) ? $form_state['storage']['status'] : $gateway->status; 
  
  $config = array();
  if (isset($gateway)) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $gateway->id,
    );
    $config = unserialize($gateway->config);
  }
  
  $disabled = FALSE;
  if (isset($gateway)) {
    $disabled = TRUE;
  }
  
  $gateways = _managed_gateway_list();
  
  $gateway_types = array('' => '--select--');
  foreach ($gateways as $gateway) {
    $gateway_types[$gateway['id']] = $gateway['title'];
  }
  
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#weight' => 1,
    '#default_value' => $name,
    '#description' => t('The recognizable name of the payment gateway.'),
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active'),
    '#weight' => 2,
    '#default_value' => $status,
    '#return_value' => 1,
    '#description' => t('If this gateway is an active option for fundraiser donation forms.'),
  );
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Gateway type'),
    '#required' => TRUE,
    '#options' => $gateway_types,
    '#weight' => 3,
    '#default_value' => $type,
    '#disabled' => $disabled,
    '#description' => t('Select the type of gateway to create.'),
  );
  
  $vid = _gateway_manager_get_vid();
  $form['taxononmy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy'),
    '#collapsible' => TRUE,
    '#weight' => 4,
  );
  
  $form['taxononmy']['terms'] = _taxonomy_term_select(t('Terms'), '', $terms, $vid, t('Categorize this payment gateway by selecting applicable terms.'), 1, null);
  $form['taxononmy']['terms']['#weight'] = 4;
  
  // if type has been selected, get the settings form for that type 
  if (isset($type)) {
    $title = _gateway_manager_get_gateway_title($gateways, $type);
    $settings_function = _gateway_manager_get_settings_function($gateways, $type);
    if (function_exists($settings_function)) {
      $gateway_settings = $settings_function($config);
      if (is_array($gateway_settings)) {
        $form['settings'] = array(
          '#type' => 'fieldset',
          '#title' => t($title . ' settings'),
          '#weight' => 5,
          '#collapsible' => TRUE,
        );
        $form['settings'][] = $gateway_settings;
      }
    }
    // return the form wrapped in a function that will save the configuration.
    return gateway_mananger_settings_form($form);
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#weight' => 6,
      '#value' => 'Continue',
    );
    return $form;
  }
}

/**
 * Submit handler for gateway_manager_edit_form. Store name and type for next step.
 */
function gateway_manager_edit_form_submit($form, &$form_state) {
  // Simply stick the choosen values into storage so that when the
  // for reloads it will pull up the correct gateway config form
  $form_state['storage']['type'] = $form_state['values']['type'];
  $form_state['storage']['name'] = $form_state['values']['name'];
  $form_state['storage']['terms'] = $form_state['values']['terms'];
  $form_state['storage']['status'] = $form_state['values']['status'];
  return;
}

/**
 * Payment gateway delete confirmation form.
 */
function gateway_managed_gateway_delete_form(&$form_state, $gateway) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $gateway->id,
  );
  return confirm_form($form, t('Are you sure you want to delete the payment gateway %name?', array('%name' => $gateway->name)), 'admin/settings/fundraiser/gateways', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function gateway_managed_gateway_delete_form_submit($form, &$form_state) {
  $gid = $form_state['values']['id'];
  if ($form_state['values']['confirm']) {
    gateway_manager_delete_gateway($gid);
  }
}

/**
 * Pulls the settings function out of an array of payment gateways.
 */
function _gateway_manager_get_settings_function($gateways, $type) {
  foreach($gateways as $gateway) {
    if ($gateway['id'] == $type) {
      return $gateway['settings'];
    }
  }
}

/**
 * Returns the vocabulary id of the taxonomy in charge of handling payment gateway terms. 
 * Creates the vocabulary if it doesn't already exist.
 */
function _gateway_manager_get_vid() {
  $vid = variable_get('gateway_manager_vocabulary', '');
  if (empty($vid) || is_null(taxonomy_vocabulary_load($vid))) {
    $vid = db_result(db_query("SELECT vid FROM {vocabulary} WHERE module = 'gateway_manager'"));
    if (!$vid) {
      $vocabulary = array(
        'name' => t('Payment Gateway Terms'),
        'multiple' => '1',
        'required' => '0',
        'hierarchy' => '0',
        'relations' => '0',
        'module' => 'gateway_manager',
      );
      taxonomy_save_vocabulary($vocabulary);
      $vid = $vocabulary['vid'];
    }
    variable_set('gateway_manager_vocabulary', $vid);
  }
  
  return $vid;
}
