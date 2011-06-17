<?php

/**
 * Springboard admin menu.
 * 
 * @param $base_url
 * Menu path of parent menu item (example: 'admin/springboard')
 */
function springboard_admin() {

}

/**
 * Springboard post-installation configuration wizard.
 * Multipart form wizard courtesy of Chaos Tools.
 */
function springboard_configuration_wizard() {
  if (module_exists('salesforce_management_api')) {
    $sf = salesforce_management_api_connect();
  }
  if (is_object($sf)) {
    $step = arg(2);
  }
  else {
    $step = 'salesforce';
  }
 
 module_load_include('module', 'ctools');
 ctools_include('wizard');
 ctools_include('object-cache');
 
 $next_text = t('next');

 
 $form_info = array(
   'id' => 'springboard_salesforce_config',
   'path' => 'admin/springboard/%step',
   'show trail' => TRUE,
   'show back' => FALSE,
   'show cancel' => FALSE,
   'show return' => FALSE,
   'next text' => $next_text,
   'next callback' => 'springboard_configuratioon_add_subtask_next',
   'finish callback' => 'springboard_configuration_add_subtask_finish',
   'cancel callback' => 'springboard_configuration_add_subtask_cancel',
   'order' => array(
      'salesforce' => t('Step 1: Salesforce Credentials'),
      'campaign' => t('Step 2: Configure Default Campaign'),
    ),
    'forms' => array(
     'salesforce' => array('form id' => 'springboard_salesforce_configuration'),
     'campaign' => array('form id' => 'springboard_campaign_configuration'),
    ),
  );
  $form_state = array('cache name' => NULL);
  $springboard_configuration = springboard_configuration_get_page_cache(NULL);
  if (!$springboard_configuration) {
    $step = current(array_keys($form_info['order']));
    $springboard_configuration = new stdClass();
    ctools_object_cache_set('springboard_configuration', $form_state['cache name'], $springboard_configuration);
  }
  $form_state['springboard_configuration_obj'] = $springboard_configuration;
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  return $output;
}

/**
 * Salesforce configuration form.
 */
function springboard_salesforce_configuration(&$form, &$form_state) {

  if (variable_get('springboard_salesforce_enabled', 0)) {
    $form['sf'] = array(
      '#type' => 'fieldset',
      '#title' => t('Salesforce configuration'),
      '#description' => t('Springboard needs your Salesforce credentials to continue.'),
    );
    $form['sf']['salesforce_username'] = array(
      '#type' => 'textfield',
      '#title' => t('Username'),
      '#description' => t('Your Salesforce login name. This should be an email address.'),
      '#default_value' => variable_get('salesforce_management_api_username', ''),
      '#required' => TRUE,
    );
    $form['sf']['salesforce_password'] = array(
      '#type' => 'password',
      '#title' => t('Password'),
      '#description' => t('Your Salesforce login password.'),
      '#default_value' => variable_get('salesforce_management_api_password', ''),
      '#required' => TRUE,
    );
    $form['sf']['salesforce_token'] = array(
      '#type' => 'textfield',
      '#title' => t('Security Token'),
      '#description' => t('You may set your security token by logging into Salesforce and navigating to Setup > My Personal Information > Reset My Security Token.'),
      '#default_value' => variable_get('salesforce_management_api_token', ''),
      '#required' => TRUE,
    );
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['sf']['salesforce_wsdl'] = array(
      '#type' => 'file',
      '#title' => t('WSDL File'),
      '#description' => t('The WSDL file tells your site what kinds of information are available in Salesforce. Insert instructions on obtaining a copy of your org WSDL here.'),

    );
  }
}

function springboard_salesforce_configuration_validate(&$form, $form_state) {

  // validate Salesforce credentials
  if ($form_state['values']['salesforce_username']) {
    if (!valid_email_address($form_state['values']['salesforce_username'])) {
      form_set_error('salesforce_username', t('The Salesforce username should be a valid email address.'));
    }
  }
  // Deal with WSDL file

  $source = 'salesforce_wsdl';
  if (isset($_FILES['files']) && $_FILES['files']['name'][$source] && is_uploaded_file($_FILES['files']['tmp_name'][$source])) {
    // Check for file upload errors and return FALSE if a
    // lower level system error occurred.
    switch ($_FILES['files']['error'][$source]) {
      // @see http://php.net/manual/en/features.file-upload.errors.php
      case UPLOAD_ERR_OK:
        break;

      case UPLOAD_ERR_INI_SIZE:
      case UPLOAD_ERR_FORM_SIZE:
        drupal_set_message(t('The file %file could not be saved, because it exceeds %maxsize, the maximum allowed size for uploads.', array('%file' => $source, '%maxsize' => format_size(file_upload_max_size()))), 'error');
        return 0;

      case UPLOAD_ERR_PARTIAL:
      case UPLOAD_ERR_NO_FILE:
        drupal_set_message(t('The file %file could not be saved, because the upload did not complete.', array('%file' => $source)), 'error');
        return 0;

        // Unknown error
      default:
        drupal_set_message(t('The file %file could not be saved. An unknown error has occurred.', array('%file' => $source)), 'error');
        return 0;
    }
    
    // Check the file extension
    $pathParts = pathinfo($_FILES['files']['name'][$source]);
    if(!in_array(strtolower($pathParts['extension']), array('xml', 'wsdl'))) {
      form_set_error($source, t('File upload error: invalid file extension. Please upload a file with an XML or WSDL  extension.'));
      return 0;
    }
  }
  // Check for connection
}

function springboard_salesforce_configuration_submit(&$form, $form_state) {
 // If the user hit "Save Configuration" and the required field
  // salesforce_management_api_password is blank, try to get it from variables
  if ($form_state['values']['submit'] == $form_state['values']['op'] and empty($form_state['values']['salesforce_password'])) {
    $pass = variable_get('salesforce_management_api_password', FALSE);
    if ($pass) {
      $form_state['values']['salesforce_password'] = $pass;
    }
  }
  // Save settings. If connection validates and WSDL is valid unset springboard_salesforce_enabled flag.
  variable_set('salesforce_management_api_username', $form_state['values']['salesforce_username']);
  variable_set('salesforce_management_api_password', $form_state['values']['salesforce_password']);
  variable_set('salesforce_management_api_token', $form_state['values']['salesforce_token']);

  // Save WSDL
  $dir = drupal_get_path('module', 'salesforce_management_api') . '/toolkit/soapclient/';

  // Upload file path
  $file = $dir . 'enterprise.wsdl.xml';

  // Array key for the file element
  $source = 'salesforce_wsdl';

 
    // Make sure the directory is writeable
    if (file_check_directory($dir)) {  
      // Try to move the uploaded file into the right place
      if (!move_uploaded_file($_FILES['files']['tmp_name'][$source], $file)) {
        form_set_error($source, t('File upload error. Could not move uploaded file.'));
        watchdog('file', 'Upload error. Could not move uploaded file to destination %destination.', array('%destination' => $dir));
        return 0;
      }
      else {
        drupal_set_message('The WSDL file has been successfully uploaded');

        // Clear the cache
        drupal_flush_all_caches();

        // Clear WSDL files from tmp directory
        $tmpDir = file_directory_temp();
        // Make sure the tmp dir exists and isn't the root, just to be safe
        if(is_dir($tmpDir) && $tmpDir != '/') {
          $cmd = 'rm -f ' . $tmpDir . '/*.wsdl.*';
          $execOutput = exec($cmd);
        }
      }
    } 
    else {
      drupal_set_message( t('WARNING: server directory is not writeable. Please contact a site admin to correct this.'), 'error' );
      watchdog('salesforce_management_api', 'WSDL file upload failed because directory is not writeable: %directory', array('%directory'=>$dir), WATCHDOG_ERROR);
      return 0;
    }
}


function springboard_campaign_configuration(&$form, &$form_state) {

  $default_campaign = variable_get('sf_donation_default_campaign', '');
  $campaigns = salesforce_management_api_get_campaigns();
  $form['salesforce_default_campaign'] = array(
    '#type' => 'select',
    '#title' => t(''),
    '#description' => t(''),
    '#options' => $campaigns,
    '#default_value' => $default_campaign,
  );
  drupal_set_message('campaigns:' . print_r($campaigns, TRUE));
}

function springboard_campaign_configuration_submit($form, $form_state) {
  if ($form_state['values']['salesforce_default_campaign']) {
    // Finalize configuration
    variable_set('sf_donation_default_campaign', $form_state['values']['salesforce_default_campaign']);

    // Put fundraiser into development mode
    variable_set('fundraiser_development_mode', 1);
    
    // Pre-configure secure pages
    $secure = "user/*\nsecure/*";
    $host = 'http://www.' . $_SERVER['HTTP_HOST'];
    $host_ssl = 'https://www.' . $_SERVER['HTTP_HOST'];
    variable_set('securepages_enable', 1);
    variable_set('securepages_basepath', $host);
    variable_set('securepages_basepath_ssl', $host_ssl);
    variable_set('securepages_secure', $secure);
    
    // Return homepage control to Drupal
    variable_set('site_frontpage', 'node/1');
    variable_del('springboard_salesforce_enabled');
  }
}
function springboard_configuration_get_page_cache($name) {
  $cache = ctools_object_cache_get('springboard_configuration', $name);
  return $cache;
}

function springboard_configuratioon_add_subtask_next(&$form_state) {
  $springboard_configuration = $form_state['springboard_configuration_obj'];
  $cache = ctools_object_cache_set('springboard_configuration', $form_state['cache name'], &$form_state['springboard_configuration_obj']);
}

Function springboard_configuration_add_subtask_finish() {
  $springboard_configuration = $form_state['springboard_configuration_obj'];
  drupal_set_message('obj:' . print_r($springboard_configuration, TRUE));
}

function springboard_configuration_add_subtask_cancel() {

}

/*
  $main_menu = menu_get_item('admin/springboard');
  $content = system_admin_menu_block((array)$main_menu);
  $output = theme('admin_block_content', $content);
  $output = drupal_get_form('springboard_welcome');
  $output .= system_admin_menu_block_page();
  return $output;
}
*/
