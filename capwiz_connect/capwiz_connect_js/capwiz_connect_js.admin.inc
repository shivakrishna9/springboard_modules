<?php

/**
* @file
* Administrative functions for the Capwiz Connect JS module
*/

function capwiz_connect_js_settings_form() {
  // Create an array of accounts for the select, not in use right now, the account is hardcoded to the national one
  $result = db_query('SELECT * FROM {capwiz_connect_accounts} ORDER BY display_name');
  while ($row = db_fetch_object($result)) {
    $accounts[$row->id] = check_plain($row->display_name);
  }

  $form['capwiz_connect_js_account'] = array(
    '#type' => 'select',
    '#required' => TRUE,
    '#options' => $accounts,
    '#title' => t('Account'),
    '#description' => t('Select the account that will be used with this Module.'),
    '#default_value' => variable_get('capwiz_connect_js_account', 1),
  );

  $form['capwiz_connect_js_account_internal_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account\'s Internal Name'),
    '#description' => t('The account name used by capwiz, used to load up the correct alerts.'),
    '#default_value' => variable_get('capwiz_connect_js_account_internal_name', ''),
  );

  return system_settings_form($form);
}

/**
 *
 */
function capwiz_connect_js_alert_list_form() {
  $form = array();
  
  $table_headers = array(
    theme('table_select_header_cell'),
    'Title',
    'Alert ID',
    'Redirect',
    'Actions'
  );

  $query = 'SELECT * FROM {capwiz_connect_js_alerts} ORDER BY capwiz_alert_id DESC';

  $result = pager_query($query, 10);
  
  while ($row = db_fetch_object($result)) {
    // Load the capwiz connection and the alerts
    $connection = capwiz_connect(variable_get('capwiz_connect_js_account', 1));
    if ($connection) {
      $capwiz_form = $connection->queryAlert($row->capwiz_alert_id);
    }

    $account_name = db_result(db_query('SELECT display_name FROM {capwiz_connect_accounts} WHERE id = %d', $row->account_id));
    $element = array('#name' => 'alert-action[]', '#id' => 'action-' . $row->capwiz_alert_id, '#return_value' => $row->capwiz_alert_id, '#parents' => array());
    $alerts[] = array(
      theme('checkbox', $element),
      !empty($capwiz_form->alert->title) ? check_plain($capwiz_form->alert->title) : t('Alert not found'),
      check_plain($row->capwiz_alert_id),
      check_plain($row->redirect),
      l(t('edit'), 'admin/build/capwiz_connect_js/add_alert/' . $row->capwiz_alert_id),
    );
  }

  // Table of existing alerts
  $form['existing_alerts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Alerts'),
    '#collapsible' => FALSE,
    '#description' => t('Existing alerts are listed below.'),
  );

  if (count($alerts)) {
    $form['existing_alerts']['update_options'] = array(
      '#title' => t('Update Options'),
      '#type' => 'select',
      '#options' => array(
        '' => '-Select-',
        'delete' => t('Delete'),
        'update_redirect' => 'Update Redirect',
        'remove_redirect' => 'Remove Redirect',
      ),
      '#description' => t('Select the the update option to apply.')
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
      
    $form['existing_alerts']['alert_table']['#value'] = theme('table', $table_headers, $alerts);

    $form['existing_alerts']['alert_table_pager']['#value'] = theme('pager');
  }
  else {
    $form['existing_alerts']['no_alerts']['#value'] = t('There are no alerts configured.');
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/*
 * Validation for the alert list form
 */
function capwiz_connect_js_alert_list_form_validate($form, &$form_state) {
  if (!count($form['#post']['alert-action'])) {
    form_set_error('', 'No alerts were selected.');
  }
  // If the update redirect value is selected ensure there is a redirect value
  if ($form_state['values']['update_options'] == 'update_redirect' && empty($form_state['values']['update_redirect_url'])) {
    form_set_error('', 'You selected to update the redirects but no URL was entered.');
  }
  // If the remove redirect value is selected ensure there is no redirect value
  if ($form_state['values']['update_options'] == 'remove_redirect' && !empty($form_state['values']['update_redirect_url'])) {
    form_set_error('', 'You selected to remove the redirects but you entered a URL.');
  }
}

/*
 * Handling for the alert list form
 */
function capwiz_connect_js_alert_list_form_submit($form, &$form_state) {
  // Delete actions
  if ($form_state['values']['update_options'] == 'delete') {
    db_query('DELETE FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id IN(' . implode(',', $form['#post']['alert-action']) . ')');
  }
  // Update alert redirects
  elseif ($form_state['values']['update_options'] == 'update_redirect') {
    foreach ($form['#post']['alert-action'] as $alert_id) {
      db_query("UPDATE {capwiz_connect_js_alerts} SET redirect = '%s' WHERE capwiz_alert_id = %d", $form_state['values']['update_redirect_url'], $alert_id);
    }
  }
  // Remove alert redirects
  elseif ($form_state['values']['update_options'] == 'remove_redirect') {
    foreach ($form['#post']['alert-action'] as $alert_id) {
      db_query("UPDATE {capwiz_connect_js_alerts} SET redirect = '' WHERE capwiz_alert_id = %d", $alert_id);
    }
  }
}

function capwiz_connect_js_add_alert_form() {
  //If an alert ID has been passed through the URL load that up
  $alert = NULL;
  if (is_numeric(arg(4))) {
    $alert = db_fetch_object(db_query('SELECT * FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = %d', arg(4)));
  }

  $form = array();

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
        if (!empty($capwiz_form->alert->id) && !db_result(db_query('SELECT capwiz_alert_id FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = %d', $querystring['alertid']))) {
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
  
    // Table of existing alerts
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
      '#type' => 'markup',
      '#value' => !empty($capwiz_form->alert->id) ? check_plain($capwiz_form->alert->title) : t('This alert could not be loaded from capwiz, it cannot be updated.'),
    );
  }
  $form['redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect'),
    '#description' => t('A URL to redirect users to after submitting the form.'),
    '#default_value' => ($alert) ? $alert->redirect : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#redirect'] = array('admin/build/capwiz_connect_js');
  return $form;
}


function capwiz_connect_js_add_alert_form_submit($form, &$form_state) {
  // If an alert ID has been passed through the URL then use that
  if (is_numeric(arg(4))) {
    $alert_id = arg(4);
  }
  // Else use either of the form values
  else {
    $alert_id = $form_state['values']['capwiz_alert_id_manual'] ? $form_state['values']['capwiz_alert_id_manual'] : $form_state['values']['capwiz_alert_id_select'];
  }

  if ($alert_id) {
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

    $exists = db_result(db_query('SELECT capwiz_alert_id FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = %d', $alert_id));

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
  }
}