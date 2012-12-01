<?php

class gateway_manager_gateway_ui extends ctools_export_ui {

  function list_table_header() {
    return array(
      array('data' => t('Nickname')),
      array('data' => t('Hostname/IP')),
      array('data' => t('Status')),
      array('data' => t('Storage')),
      array('data' => t('Operations')),
    );
  }

  function list_sort_options() {
    return array(
      'disabled' => t('Nickname'),
      'smtp_host' => t('Hostname/IP'),
      'storage' => t('Storage'),
    );
  }

  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$item->name] = $item->admin_title;
        break;
      case 'smtp_host':
        $this->sorts[$item->name] = $item->smtp_host;
        break;
      case 'storage':
        $this->sorts[$item->name] = $item->type . $item->admin_title;
        break;
    }

    // Build status.
    $waiting_servers = variable_get('multi_smtp_all_waiting_servers', array());
    if (!empty($waiting_servers[$item->name])) {
      // Server is waiting.
      $status = t('Server is waiting until @time', array(
        '@time' => format_date($waiting_servers[$item->name], 'long'),
      ));
    }
    elseif (!empty($item->disabled)) {
      $status = t('Offline (disabled)');
    }
    else {
      $status = t('Online');
    }

    $this->rows[$item->name] = array(
      'data' => array(
        array('data' => check_plain($item->admin_title)),
        array('data' => check_plain($item->smtp_host)),
        array('data' => $status),
        array('data' => check_plain($item->type)),
        array('data' => theme('links', $operations)),
      ),
      'title' => check_plain($item->admin_description),
      'class' => !empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled',
    );
  }

  function list_search_fields() {
    return array(
      'name',
      'admin_title',
      'admin_description',
      'smtp_host',
    );
  }

}


/**
 * Settings form for a server.
 */
function multi_smtp_server_ui_edit_settings_form(&$form, &$form_state) {
  $item = $form_state['item'];

  $form['info']['admin_title']['#title'] = t('Server nickname');

  $form['server_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Server details'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['server_details']['smtp_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Hostname or IP address'),
    '#description' => t('The SMTP server hostname or IP address.'),
    '#default_value' => $item->smtp_host,
    '#required' => TRUE,
    '#size' => 40,
    '#maxlength' => 255,
  );
  $form['server_details']['smtp_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port'),
    '#description'   => t('The default SMTP port is 25, if that is being blocked try 80. Gmail uses 465. See !url for more information on configuring for use with Gmail.', array('!url' => l(t('this page'), 'http://gmail.google.com/support/bin/answer.py?answer=13287'))),
    '#default_value' => ($item->smtp_port > 0) ? $item->smtp_port : 25,
    '#required' => TRUE,
    '#size' => 40,
    '#maxlength' => 255,
  );

  // Only display the Encryption option if openssl is installed.
  // This part copied from SMTP module
  if (function_exists('openssl_open')) {
    $encryption_options = array(
      'standard' => t('No'),
      'ssl' => t('Use SSL'),
      'tls' => t('Use TLS'),
    );
    $encryption_description = t('This allows connection to an SMTP server that requires SSL encryption such as Gmail.');
  }
  // If openssl is not installed, use normal protocol.
  else {
    variable_set('smtp_protocol', 'standard');
    $encryption_options = array('standard' => t('No'));
    $encryption_description = t('Your PHP installation does not have SSL enabled. See the !url page on php.net for more information. Gmail requires SSL.', array('!url' => l(t('OpenSSL Functions'), 'http://php.net/openssl')));
  }
  $form['server_details']['smtp_protocol'] = array(
    '#type' => 'select',
    '#title' => t('Use encrypted protocol'),
    '#default_value' => ($item->smtp_protocol) ? $item->smtp_protocol : 'standard',
    '#options' => $encryption_options,
    '#description' => $encryption_description,
  );

  // Disabling and marking a server as waiting is not available for new servers.
  if (!empty($item->name) && empty($item->export_ui_item_is_cached)) {
    if (!multi_smtp_is_server_waiting($item->name)) {
      $form['server_details']['disabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('Server is disabled'),
        '#description' => t('Check this if you want to remove this server from the list of available servers.'),
        '#default_value' => $item->disabled,
      );
    }
    $form['server_details']['mark_wait'] = array(
      '#type' => 'checkbox',
      '#title' => t('Server is waiting'),
      '#description' => t('Check this if you need to temporarily mark this server as waiting.'),
      '#default_value' => multi_smtp_is_server_waiting($item->name),
    );
  }

  $form['credentials'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credentials'),
    '#description' => t('Leave blank if your SMTP server does not require authentication.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['credentials']['smtp_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Username for the server.'),
    '#default_value' => $item->smtp_username,
    '#size' => 40,
    '#maxlength' => 255,
  );
  $form['credentials']['smtp_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('Password for the server.'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 255,
  );

  $form['email_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail options'),
    '#description' => t('The From address will be used for most mail out of the system. Note that some modules (e.g., Simplenews) may override these values.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['email_options']['smtp_from'] = array(
    '#type' => 'textfield',
    '#title' => t('From address'),
    '#description' => t('From address to use for e-mails sent through this server.'),
    '#default_value' => $item->smtp_from,
    '#size' => 40,
    '#maxlength' => 255,
  );
  $form['email_options']['smtp_fromname'] = array(
    '#type' => 'textfield',
    '#title' => t('From name'),
    '#description' => t('Name on from address to use for e-mails sent through this server.'),
    '#default_value' => $item->smtp_fromname,
    '#size' => 40,
    '#maxlength' => 255,
  );
}


/**
 * Validate callback for the server settings form.
 */
function multi_smtp_server_ui_edit_settings_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];

  // Validate the hostname.
  if ($values['smtp_host']) {
    // Patterns copied from this Stack Overflow page:
    // http://stackoverflow.com/questions/106179/regular-expression-to-match-hostname-or-ip-address
    $patterns = array(
      // IP address pattern:
      "/^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/",
      // Hostname pattern:
      "/^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$/",
    );
    // If neither IP address nor hostname are valid, return error.
    if (!(preg_match($patterns[0], $values['smtp_host']) || preg_match($patterns[1], $values['smtp_host']))) {
      form_set_error('smtp_host', t('You must enter a valid SMTP server hostname or IP.'));
    }
  }

  if ($values['smtp_port'] && (!is_numeric($values['smtp_port']) || (int) $values['smtp_port'] != $values['smtp_port'])) {
    form_set_error('smtp_port', t('You must enter an integer for the SMTP port number.'));
  }

  if ($values['smtp_from'] && !valid_email_address($values['smtp_from'])) {
    form_set_error('smtp_from', t("The sender's email address you supplied is not valid."));
  }
}


/**
 * Submit callback for the server settings form.
 */
function multi_smtp_server_ui_edit_settings_form_submit(&$form, &$form_state) {
  $item = $form_state['item'];

  if (empty($item->name) || !empty($item->export_ui_item_is_cached)) {
    // Exit here because none of this applies to new objects.
    return;
  }

  // Don't wipe out the password if it was left blank.
  if (!empty($form_state['values']['smtp_username']) && empty($form_state['values']['smtp_password'])) {
    $form_state['values']['smtp_password'] = $item->smtp_password;
  }

  if (isset($form['server_details']['disabled'])) {
    if (!empty($form_state['values']['disabled'])) {
      ctools_export_set_object_status($item, TRUE);
    }
    else {
      ctools_export_set_object_status($item, FALSE);
    }
  }

  if (!empty($form_state['values']['mark_wait'])) {
    multi_smtp_mark_server_to_wait($item->name);
  }
  else {
    multi_smtp_unmark_server_to_wait($item->name);
  }
}
