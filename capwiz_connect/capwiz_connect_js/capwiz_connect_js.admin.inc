<?php

/**
* @file
* Administrative functions for the Capwiz Connect JS module
*/

/**
 * Implements of hook_form_alter()
 * 
 * Here to ensure this inc file gets loaded for form_id_alter below
 */
function capwiz_connect_js_form_alter(&$form, &$form_state, $form_id){
}

/**
 * Implements hook_form_id_alter().
 * 
 * Add our settings to the main capwiz connect settings form
 */
function capwiz_connect_js_form_capwiz_connect_settings_alter(&$form, &$form_state, $form_id) {
  // Load up all the accounts and create an array of options for the select
  $accounts = capwiz_connect_all_accounts();
  $options = array();
  if (!empty($accounts)) {
    foreach ($accounts as $id => $account) {
      $options[$account->id] = check_plain($account->display_name);
    }
  }
  
  $form['capwiz_connect_js'] = array(
    '#type' => 'fieldset',
    '#title' => t('Capwiz Connect JS'),
    '#description' => t('The following settings are for the Capwiz Connect JS Module.'),
  );

  $form['capwiz_connect_js']['capwiz_connect_js_account'] = array(
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => $options,
    '#title' => t('Account'),
    '#description' => t('Select the account that will be used with this Module.'),
    '#default_value' => variable_get('capwiz_connect_js_account', 1),
  );

  $form['capwiz_connect_js']['capwiz_connect_js_account_internal_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account\'s Internal Name'),
    '#description' => t('The account name used by capwiz, used to load up the correct alerts on th.'),
    '#default_value' => variable_get('capwiz_connect_js_account_internal_name', ''),
  ); 
}

/**
 * Implements hook_form_form_id_alter().
 * 
 * Add the mappings for the data from our tables 
 */
function capwiz_connect_js_form_capwiz_connect_account_user_mapping_alter(&$form, &$form_state, $form_id) {
  // If this is not the action mapping form or we don't have an object do nothing
  if ($form['#map_type'] != 'user' && (!$form_state['values']['object'] || empty($account->mapping_user['sf_object']))) {
    return;
  }

  $account = $form['#account'];

  // Load an array of Drupal user fields for targets
  $drupal_user_fields = array(
    '' => '',
  );

  // Add profile fields to the user object if the module is enabled.
  if (module_exists('profile')) {
    // Load all the profile fields from the database.
    $result = db_query("SELECT fid, name, title, category FROM {profile_fields} ORDER BY category, weight");

    // Loop through the fields and add them to the Drupal user object.
    while ($field = db_fetch_array($result)) {
      $drupal_user_fields['Profile fields'][$field['fid']] = t('@category: @title', array('@category' => $field['category'], '@title' => $field['title']));
    }
  }

  $qs_keys = _market_source_build_qs_keys();

  foreach (array_keys($qs_keys) as $key) {
      
    // Capwiz field
    $form['map']['user_field'][$key] = array(
      '#type' => 'item',
      '#description' => $key,
    );
    // Drupal user field
    $form['map']['drupal_user_field'][$key] = array(
      '#type' => 'select',
      '#options' => $drupal_user_fields,
      '#default_value' => (isset($account->mapping_user['map'][$key])) ? $account->mapping_user['map'][$key] : '',
    );
    // Overwrite business rules
    $form['map']['business'][$key] = array(
      '#type' => 'select',
      '#default_value' => (isset($account->mapping_user['business'][$key])) ? $account->mapping_user['business'][$key] : '',
      '#options' => array(
        '' => '',
        'blank' => t('Overwrite when blank'), 
        'append' => t('Append'),
        'always' => t('Always overwrite'),
        'never' => t('Never overwrite'),
      ),
    );
  }
}

/**
 * Implements hook_form_form_id_alter().
 * 
 * Add the mappings for the data from our tables 
 */
function capwiz_connect_js_form_capwiz_connect_account_action_mapping_alter(&$form, &$form_state) {
  // If this is not the action mapping form or we don't have an object do nothing
  if ($form['#map_type'] != 'action' && (!$form_state['values']['object'] || empty($account->mapping_action['sf_object']))) {
    return;
  }

  $account = $form['#account'];

  $object = !empty($account->mapping_action['sf_object']) ? $account->mapping_action['sf_object'] : $form_state['values']['object'];

  // Get the fields available on this type of Salesforce object
  $object_details = _capwiz_connect_get_sf_object_details($object);

  if (!empty($object_details)) {
    $fields = $object_details['fields'];
    $record_types = $object_details['record_types'];

    // Create a drop down list friendly version of the Salesforce fields
    foreach ($fields as $key => $value) {
      $options[$key] = $value['label'];
    }

    $qs_keys = _market_source_build_qs_keys();

    foreach (array_keys($qs_keys) as $key) {
      $map_for_field = $account->mapping_action['map'][$key];

      // Capwiz field
      $form['map']['user_field'][$key] = array(
        '#type' => 'item',
        '#description' => t($key),
      );
      // Salesforce field
      $form['map']['salesforce'][$key] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $account->mapping_action['map'][$key],
      );
    }
  }
}

/**
 *
 */
function capwiz_connect_js_alert_list_form($form, &$form_state) {
  $form = array();

  $header = array(
    'title' => t('Title'),
    'id' => array(
      'data' => t('Alert ID'),
      'field' => 'ca.capwiz_alert_id',
    ), 
    'redirect' => t('Redirect'),
  );
  $header['operations'] = array('data' => t('Operations'));

  $query = db_select('capwiz_connect_js_alerts', 'ca')->extend('PagerDefault')->extend('TableSort');
  $alerts = $query
    ->fields('ca')
    ->orderByHeader($header)
    ->limit(10)
    ->execute()
    ->fetchAllAssoc('capwiz_alert_id');

  $options = array();
  foreach ($alerts as $id => $alert) {
    // Load the capwiz connection and the alerts
    $connection = capwiz_connect(variable_get('capwiz_connect_js_account', 1));
    if ($connection) {
      $capwiz_form = $connection->queryAlert($id);
    }

    $options[$id] = array(
      'title' => !empty($capwiz_form->alert->title) ? check_plain($capwiz_form->alert->title) : t('Alert not found'),
      'id' => check_plain($id),
      'redirect' => check_plain($alert->redirect),
    );

    $operations['edit'] = array(
      'title' => t('edit'), 
      'href' => CAPWIZ_CONNECT_ADMIN_URL . '/alerts/' . $id . '/edit'
    );

    // Render an unordered list of operations links.
    $options[$id]['operations'] = array(
      'data' => array(
        '#theme' => 'links', 
        '#links' => $operations, 
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
  }

  // Table of existing alerts
  $form['existing_alerts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Alerts'),
    '#collapsible' => FALSE,
    '#description' => t('Existing alerts are listed below. !link', array('!link' => l(t('Add a new alert'), CAPWIZ_CONNECT_ADMIN_URL . '/alerts/add'))),
  );

  $form['existing_alerts']['operation'] = array(
    '#type' => 'select', 
    '#title' => t('Operation'), 
    '#options' => array(
      '' => '-Select-',
      'delete' => t('Delete'),
      'update_redirect' => 'Update Redirect',
      'remove_redirect' => 'Remove Redirect',
    ),
  );

  // Update redirect options
  $form['existing_alerts']['update_redirect'] = array(
    '#type' => 'fieldset',
    '#title' => t('Redirect URL'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['existing_alerts']['update_redirect']['update_redirect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Update Redirect'),
    '#description' => t('To update the redirect of the alert(s) select the option above and enter the URL in this field.'),
  );

  if (user_access('manage capwiz js alerts')) {
    $form['existing_alerts']['alert_table'] = array(
      '#type' => 'tableselect', 
      '#header' => $header, 
      '#options' => $options, 
      '#empty' => t('No content available.'),
    );
  }

  $form['existing_alerts']['pager'] = array('#markup' => theme('pager'));

  $form['existing_alerts']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Update'), 
    //'#validate' => array('node_admin_nodes_validate'), 
    //'#submit' => array('node_admin_nodes_submit'),
  );

  return $form;
}

/*
 * Validation for the alert list form
 */
function capwiz_connect_js_alert_list_form_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['alert_table']) || !count(array_filter($form_state['values']['alert_table']))) {
    form_set_error('', 'No alerts were selected.');
  }

  // If the update redirect value is selected ensure there is a redirect value
  if ($form_state['values']['operation'] == 'update_redirect' && empty($form_state['values']['update_redirect_url'])) {
    form_set_error('', 'You selected to update the redirects but no URL was entered.');
  }

  // If the remove redirect value is selected ensure there is no redirect value
  if ($form_state['values']['operation'] == 'remove_redirect' && !empty($form_state['values']['update_redirect_url'])) {
    form_set_error('', 'You selected to remove the redirects but you entered a URL.');
  }
}

/*
 * Handling for the alert list form
 */
function capwiz_connect_js_alert_list_form_submit($form, &$form_state) {
  $ids = array_filter($form_state['values']['alert_table']);
  // Delete actions
  if ($form_state['values']['operation'] == 'delete') {
    db_delete('capwiz_connect_js_alerts')
      ->condition('capwiz_alert_id', $ids, 'IN')
      ->execute();
  }
  // Update alert redirects
  elseif ($form_state['values']['operation'] == 'update_redirect') {
    db_update('capwiz_connect_js_alerts')
      ->fields(array('redirect' => $form_state['values']['update_redirect_url']))
      ->condition ('capwiz_alert_id', $ids, 'IN')
      ->execute();
  }
  // Remove alert redirects
  elseif ($form_state['values']['operation'] == 'remove_redirect') {
    db_update('capwiz_connect_js_alerts')
      ->fields(array('redirect' => ''))
      ->condition ('capwiz_alert_id', $ids, 'IN')
      ->execute();
  }
}

/**
 * Form for adding or editing an alert 
 */
function capwiz_connect_js_alert_addoredit_form($form, &$form_state, $action, $id = NULL) {
  $form = array();
  $form['#alert'] = $alert = NULL;
  $options = array();

  if ($action == 'edit') {
    $alert = db_query('SELECT * FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = :id', array(':id' => intval($id)))->fetchObject();
    if (empty($alert)) {
      drupal_set_message('There is no alert with this ID');
      drupal_goto(CAPWIZ_CONNECT_ADMIN_URL . '/alerts');
    }
    $form['#alert'] = $alert;
  }

  // Load the capwiz connection and the alerts
  $connection = capwiz_connect(variable_get('capwiz_connect_js_account', 1));

  // If we aren't editing an alert then show the select list
  if (!$alert) {
    // If the internal capwiz name has been set then we can load up alerts from the RSS feed
    if (variable_get('capwiz_connect_js_account_internal_name', '') != '') {
     $url = 'http://www.capwiz.com/' . variable_get('capwiz_connect_js_account_internal_name', '') . '/RSS/alerts/';
      $public_rss = @simplexml_load_file($url);
    }

    $public_alerts = NULL;
    // Loop through the public alerts
    if (count($public_rss->item)) {
      foreach ($public_rss->item as $key => $item) {
        $url = parse_url($item->link);
        parse_str($url['query'], $querystring);
        if ($connection) {
          $capwiz_form = $connection->queryAlert($querystring['alertid']);
        }
        // If this alert is active in capwiz and has not already been setup then list        
        if (!empty($capwiz_form->alert->id) && !(bool) db_query('SELECT capwiz_alert_id FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = :id', array(':id' => $querystring['alertid']))->fetchField()) {
          $options[$querystring['alertid']] = (string) $item->title;  
        }
      }
    }

    if (count($options)) {
      $form['capwiz_alert_id_select'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Alert'),
        '#description' => t('Select the alert.'),
      );
    }
 
    // Provide a field to manually enter the Alert ID
    $form['manual_alert'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manual'),
      '#collapsible' => TRUE,
      '#collapsed' => count($options) ? TRUE : FALSE,
    );

    $form['manual_alert']['capwiz_alert_id_manual'] = array(
      '#type' => 'textfield',
      '#title' => t('Alert ID (Manual)'),
      '#description' => t('If the alert is not listed above this alert ID can be found in the URL of the alert popup window.'),
    );
  }
  // We are editing an existing alert, just show the title
  else {
    if ($connection) {
      $capwiz_form = $connection->queryAlert($alert->capwiz_alert_id);
    }
    $form['capwiz_alert_edit'] = array(
      '#markup' => !empty($capwiz_form->alert->id) ? check_plain($capwiz_form->alert->title) : t('This alert could not be loaded from capwiz, it cannot be updated.'),
    );
  }
  $form['redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect'),
    '#description' => t('A URL to redirect users to after submitting the form.'),
    '#default_value' => $alert && !empty($alert->redirect) ? $alert->redirect : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for the alert add/edit form 
 */
function capwiz_connect_js_alert_addoredit_form_submit($form, &$form_state) {
  // If an alert ID has been passed through the URL then use that
  if (!empty($form['#alert'])) {
    $alert_id = $form['#alert']->capwiz_alert_id;
  }
  // Else use either of the form values
  else {
    $alert_id = $form_state['values']['capwiz_alert_id_manual'] ? $form_state['values']['capwiz_alert_id_manual'] : $form_state['values']['capwiz_alert_id_select'];
  }
  
  if (!$alert_id) {
    form_set_error('', 'No alert id was set.');
    return;
  }

  $record = array(
    'capwiz_alert_id' => $alert_id,
    'account_id' => variable_get('capwiz_connect_js_account', 1),
    'redirect' => $form_state['values']['redirect'],
  );

  // Load the capwiz connection and the alerts
  $connection = capwiz_connect(variable_get('capwiz_connect_js_account', 1));
  if ($connection) {
    $capwiz_form = $connection->queryAlert($alert_id);
  }

  if (!empty($capwiz_form->alert->id)) {
    $record['capwiz_api_alert_id'] = $capwiz_form->alert->id;
  }
  else {
    form_set_error('', 'The system was not able to find the API ID.');
    return;
  }

  $exists = (bool) db_query('SELECT capwiz_alert_id FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = :id', array(':id' => $alert_id))->fetchField();

  if ($exists) {
    $record = drupal_write_record('capwiz_connect_js_alerts', $record, 'capwiz_alert_id');
    if ($record) {
      $message = t('Alert updated');
    }
  }
  else {
    $record = drupal_write_record('capwiz_connect_js_alerts', $record);
    if ($record) {
      $message = t('New Alert created');
    }
  }
  if ($record) {
    drupal_set_message($message);
  }
  $form_state['redirect'] = array(CAPWIZ_CONNECT_ADMIN_URL . '/alerts');
}
