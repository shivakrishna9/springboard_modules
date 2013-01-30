<?php
/**
 * @file
 * Provides Springboard administrative forms.
 */

/**
 * Admin settings form.
 */
function springboard_admin_settings() {
  $form = array();
  if (variable_get('springboard_needs_config', FALSE)) {
    $form['springboard_config'] = array(
      '#type' => 'fieldset',
      '#title' => t('Configure Springboard'),
      '#description' => t('New to Springboard? Configure the default Springboard installation to get started quickly. ' .
        'Give the system some time to work, it may take a bit of time to configure everything correctly.'),
      '#weight' => -1,
    );
    $form['springboard_config']['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Springboard settings'),
    );
    $form['springboard_config']['settings']['order_handling'] = array(
      '#type' => 'select',
      '#options' => array(
        'ubercart' => t('Ubercart'),
        'commerce' => t('Commerce'),
      ),
      '#title' => t('Select order processing system'),
    );
    $form['springboard_config']['settings']['sf_enable'] = array(
      '#title' => t('Enable Salesforce Integration?'),
      '#type' => 'checkbox',
      '#description' => t('If your organization has a Salesforce org and plans on synching information between Drupal ' .
        'and Salesforce you should enable integration.'),
    );
    $form['springboard_config']['settings']['sf'] = array(
      '#title' => t('Salesforce Integration Settings'),
      '#type' => 'fieldset',
    );
    $form['springboard_config']['settings']['sf']['sf_user'] = array(
      '#title' => t('Enable Salesforce User Integration'),
      '#type' => 'checkbox',
    );
    $form['springboard_config']['settings']['sf']['sf_donation'] = array(
      '#title' => t('Enable Salesforce Donation Integration'),
      '#type' => 'checkbox',
    );
    $form['springboard_config']['settings']['sf']['sf_webform'] = array(
      '#title' => t('Enable Salesforce Webform Integration'),
      '#type' => 'checkbox',
    );
    $form['springboard_config']['configure'] = array(
      '#type' => 'submit',
      '#value' => t('Configure default Springboard'),
      '#description' => t('Click here to use the default settings for a Springboard install.'),
      '#submit' => array('springboard_admin_config'),
    );
    $form['springboard_config']['ignore'] = array(
      '#type' => 'submit',
      '#value' => t('Custom Springboard'),
      '#description' => t('Click here if you want to configure Springboard yourself.'),
      '#submit' => array('springboard_admin_ignore'),
    );
  }
  // TODO: add support for pane weights.
  $panes = module_invoke_all('springboard_dashboard_panes');
  $default_enabled_panes = array();
  drupal_alter('springboard_dashboard_panes', $panes);
  foreach ($panes as $name => $info) {
    $panes[$name] = $info['label'];
    $default_enabled_panes[$name] = $name;
  }
  $form['panes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Panes'),
  );
  $form['panes']['springboard_enabled_panes'] = array(
    '#description' => t('Enable/disable visible panes on the Springboard Dashboard'),
    '#type' => 'checkboxes',
    '#options' => $panes,
    '#default_value' => variable_get('springboard_enabled_panes', $default_enabled_panes),
  );
  $form['dashboard'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Threshold Settings'),
    '#description' => t('Configure display settings for your dashboard.'),
  );
  $form['dashboard']['springboard_conversion_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Conversion Percentage Threshold'),
    '#description' => t('Set the minimum acceptable conversion rate percentage. Any forms with lower ' .
      'conversion rates will be flagged on the dashboard.'),
    '#default_value' => variable_get('springboard_conversion_threshold', 0),
  );
  $form['dashboard']['springboard_local_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Local Failure Threshold'),
    '#description' => t('Set the maximum acceptable local failure percentage. Any forms with higher ' .
      'local failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_local_failure_threshold', 0),
  );
  $form['dashboard']['springboard_gateway_failure_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Gateway Failure Percentage Threshold'),
    '#description' => t('Set the maximum acceptable gateway failure percentage. Any forms with higher ' .
      'gateway failure rates will be flagged.'),
    '#default_value' => variable_get('springboard_gateway_failure_threshold', 0),
  );
  $form['dashboard']['springboard_load_time_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Load Time Threshold'),
    '#description' => t('Set the maximum acceptable time (in ms) to render a donation form. Any forms ' .
      'taking longer than this to load will be flagged.'),
    '#default_value' => variable_get('springboard_load_time_threshold', 500),
  );
  $form['#validate'][] = 'springboard_admin_settings_validate';
  return system_settings_form($form);
}

/**
 * Validation handler for the springboard admin settings form.
 */
function springboard_admin_settings_validate(&$form, $form_state) {
  if ($form_state['values']['springboard_conversion_threshold'] && !is_numeric($form_state['values']['springboard_conversion_threshold'])) {
    form_set_error('springboard_conversion_threshold', t('The conversion threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_local_failure_threshold'] && !is_numeric($form_state['values']['springboard_local_failure_threshold'])) {
    form_set_error('springboard_local_failure_threshold', t('The local failure threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_gateway_failure_threshold'] && !is_numeric($form_state['values']['springboard_gateway_failure_threshold'])) {
    form_set_error('springboard_gateway_failure_threshold', t('The gateway failure threshold must be numeric!'));
  }
  if ($form_state['values']['springboard_load_time_threshold'] && !is_numeric($form_state['values']['springboard_load_time_threshold'])) {
    form_set_error('springboard_load_time_threshold', t('The load time threshold must be numeric!'));
  }
}

/**
 * Submit handler for the springboard configuration button.
 */
function springboard_admin_config(&$form, $form_state) {
  variable_del('springboard_needs_config');
  drupal_set_message(t('Please see the !help page for additional help configuring Springboard.',
    array('!help' => l(t('Help'), 'admin/springboard/help'))));
  // This is where what used to be profile configuration in D6 went.
  // We dropped the profile support since it was, honestly, more hinderence than harm.
  // We would rather the Springboard suite be useful for all default profiles.
  springboard_admin_config_modules($form, $form_state);
  // Clear all caches to pick up themed elements.
  drupal_flush_all_caches();
  // Set up module specific things.
  springboard_admin_config_springboard($form, $form_state);
  springboard_admin_config_profile($form, $form_state);
  if ($form_state['values']['order_handling'] == 'ubercart') {
    springboard_admin_config_ubercart($form, $form_state);
  }
  else {
    springboard_admin_config_commerce($form, $form_state);
  }
  springboard_admin_config_donation_form_type($form, $form_state);
  springboard_admin_config_donation_form_add($form, $form_state);
  // Clear all caches to pick up themed elements.
  drupal_flush_all_caches();
  menu_rebuild();
}

/**
 * Turn on a standard set of modules.
 */
function springboard_admin_config_modules(&$form, $form_state) {
  // Springboard custom.
  // Enable does recursion so we don't need to dig deeper.
  $modules = array(
    'fundraiser',
    'fundraiser_webform',
    'fundraiser_sustainers',
    'fundraiser_triggers',
    'email_wrappers',
    'market_source',
    'webform_user',
    'springboard_views',
  );
  if ($form_state['values']['order_handling'] == 'ubercart') {
    $modules[] = 'fundraiser_ubercart';
    $modules[] = 'uc_credit';
    $modules[] = 'test_gateway';
  }
  else {
    $modules[] = 'fundraiser_commerce';
    $modules[] = 'commerce_payment_example';
  }
  module_enable($modules);
  drupal_set_message(t('Modules %modules have been enabled.', array('%modules' =>implode(', ', $modules))));
  // Optional Salesforce modules.
  if ($form_state['values']['sf_enable']) {
    // Variable used to indicate the Salesforce configuration form should be displayed on the Dashboard.
    variable_set('springboard_salesforce_enabled', 1);
    //TODO: add conditional logic to enable Salesforce helper modules based on form selections.
    // Enable Salesforce module suite.
    // TODO confirm all these names post integration work
    $optional = array(
      'salesforce_api',
      //'queue_api',
      //'sb_npsp',
      //'queue_processor',
      //'sf_node',
    );
    foreach ($optional as $name) {
      $exists = db_result(db_query("SELECT name FROM {system} WHERE type='module' and name='%s'", $name));
      module_enable(array($name));
      springboard_admin_config_salesforce_contacts($form, $form_state);
    }
    drupal_set_message(t('Modules %modules have been enabled.', array('%modules' =>implode(', ', $optional))));
    if ($form_state['values']['sf_user']) {
      //module_enable(array('sf_user'));
      drupal_set_message(t('Module %modules hase been enabled.', array('%modules' => 'sf_user')));
    }
    if ($form_state['values']['sf_donation']) {
      //module_enable(array('sf_donation'));
      drupal_set_message(t('Module %modules hase been enabled.', array('%modules' => 'sf_donation')));
    }
    if ($form_state['values']['sf_webform']) {
      //module_enable(array('sf_webform'));
      drupal_set_message(t('Module %modules hase been enabled.', array('%modules' => 'sf_webform')));
    }
    // Configure default donation backend
    variable_set('sf_donation_backend', 'npsp');
  }
}

/**
 * Configure Drupal <-> Salesforce contact mapping.
 */
// TODO check this when doing integration work later.
function springboard_admin_config_salesforce_contacts(&$form, $form_state) {
  $business = 'a:15:{s:2:"Id";s:5:"never";s:9:"AccountId";s:5:"never";s:8:"LastName";s:6:"always";s:9:"FirstName";s:6:"always";s:13:"MailingStreet";s:6:"always";s:11:"MailingCity";s:6:"always";s:12:"MailingState";s:6:"always";s:17:"MailingPostalCode";s:6:"always";s:14:"MailingCountry";s:6:"always";s:5:"Email";s:6:"always";s:17:"Drupal_User_ID__c";s:6:"always";s:19:"Initial_Referrer__c";s:6:"always";s:28:"Initial_Registration_Date__c";s:6:"always";s:16:"Market_Source__c";s:6:"always";s:11:"Referrer__c";s:6:"always";}';
  $fields = 'a:15:{s:2:"Id";s:29:"profile_salesforce_contact_id";s:9:"AccountId";s:29:"profile_salesforce_account_id";s:8:"LastName";s:17:"profile_last_name";s:9:"FirstName";s:18:"profile_first_name";s:13:"MailingStreet";s:15:"profile_address";s:11:"MailingCity";s:12:"profile_city";s:12:"MailingState";s:13:"profile_state";s:17:"MailingPostalCode";s:11:"profile_zip";s:14:"MailingCountry";s:15:"profile_country";s:5:"Email";s:4:"mail";s:17:"Drupal_User_ID__c";s:3:"uid";s:19:"Initial_Referrer__c";s:24:"profile_initial_referrer";s:28:"Initial_Registration_Date__c";s:7:"created";s:16:"Market_Source__c";s:10:"profile_ms";s:11:"Referrer__c";s:16:"profile_referrer";}';
  $business_sd = 'a:15:{s:2:"Id";s:6:"always";s:9:"AccountId";s:6:"always";s:8:"LastName";s:6:"always";s:9:"FirstName";s:6:"always";s:13:"MailingStreet";s:6:"always";s:11:"MailingCity";s:6:"always";s:12:"MailingState";s:6:"always";s:17:"MailingPostalCode";s:6:"always";s:14:"MailingCountry";s:6:"always";s:5:"Email";s:5:"never";s:17:"Drupal_User_ID__c";s:5:"never";s:19:"Initial_Referrer__c";s:5:"never";s:28:"Initial_Registration_Date__c";s:5:"never";s:16:"Market_Source__c";s:5:"never";s:11:"Referrer__c";s:5:"never";}';
  $validation = 'a:15:{s:2:"Id";a:3:{s:10:"field_type";s:2:"id";s:8:"nillable";b:0;s:6:"length";i:18;}s:9:"AccountId";a:3:{s:10:"field_type";s:9:"reference";s:8:"nillable";b:1;s:6:"length";i:18;}s:8:"LastName";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:0;s:6:"length";i:80;}s:9:"FirstName";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:40;}s:13:"MailingStreet";a:3:{s:10:"field_type";s:8:"textarea";s:8:"nillable";b:1;s:6:"length";i:255;}s:11:"MailingCity";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:40;}s:12:"MailingState";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:20;}s:17:"MailingPostalCode";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:20;}s:14:"MailingCountry";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:40;}s:5:"Email";a:3:{s:10:"field_type";s:5:"email";s:8:"nillable";b:1;s:6:"length";i:80;}s:17:"Drupal_User_ID__c";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:50;}s:19:"Initial_Referrer__c";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:255;}s:28:"Initial_Registration_Date__c";a:3:{s:10:"field_type";s:4:"date";s:8:"nillable";b:1;s:6:"length";i:0;}s:16:"Market_Source__c";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:250;}s:11:"Referrer__c";a:3:{s:10:"field_type";s:6:"string";s:8:"nillable";b:1;s:6:"length";i:255;}}';
  $sql = "INSERT INTO {salesforce_management_field_map}
    VALUES (1, 'General user fieldmap', 'user', 'Contact', '', 0, '%s', '%s', '%s', 'Email', 2, '0', '%s')";
  db_query($sql, $fields, $business, $business_sd, $validation);
}

/**
 * Configure Springboard iteself.
 */
function springboard_admin_config_springboard(&$form, $form_state) {
  $default_enabled_panes = array();
  drupal_alter('springboard_dashboard_panes', $panes);
  if (is_array($panes)) {
    foreach ($panes as $name => $info) {
      $panes[$name] = $info['label'];
      $default_enabled_panes[$name] = $name;
    }
    variable_set('springboard_enabled_panes', $default_enabled_panes);
  }
}

/**
 * Configure a default set of User profile fields.
 */
function springboard_admin_config_profile(&$form, $form_state) {
  // See http://drupal.org/node/874026 for discussion re: Profile in D7.
  // By preference we use field API.
  // Add profile fields
  $profile_fields = array();
  $profile_fields[] = array(
    'label' => t('First name'),
    'field_name' => 'profile_first_field_name',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Last name'),
    'field_name' => 'profile_last_field_name',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Address'),
    'field_name' => 'profile_address',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Address Line 2'),
    'field_name' => 'profile_address_line_2',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  ); 
  $profile_fields[] = array(
    'label' => t('City'),
    'field_name' => 'profile_city',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  if ($form_state['values']['order_handling'] == 'ubercart') {
    // Default options loaded from {uc_zones} On the assumption we are using Ubercart here.
    // Load State/Province options from {uc_zones}, UC default is zone codes for US & Canada.
    $zones = array();
    $results = db_query('SELECT zone_code FROM {uc_zones} ORDER BY zone_country_id, zone_code');
    foreach ($results as $result) {
       $zones[] = $result->zone_code;
    }
    $zones = implode("\n", $zones);
    $profile_fields[] = array(
      'label' => t('State/Province'),
      'field_name' => 'profile_state',
      'type' => 'list_text',
      'widget' => array(
        'type' => 'options_select',
      ),
    );
  }
  else {
    // For Commerce or others for now.
    $profile_fields[] = array(
      'label' => t('State/Province'),
      'field_name' => 'profile_state',
      'type' => 'text',
      'widget' => array(
        'type' => 'text_textfield',
      ),
    );
  }
  $profile_fields[] = array(
    'label' => t('Postal Code'),
    'field_name' => 'profile_zip',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Country'),
    'field_name' => 'profile_country',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Campaign ID'),
    'field_name' => 'profile_cid',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  $profile_fields[] = array(
    'label' => t('Market Source'),
    'field_name' => 'profile_ms',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  $profile_fields[] = array(
    'label' => t('Referrer'),
    'field_name' => 'profile_referrer',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  $profile_fields[] = array(
    'label' => t('Initial Referrer'),
    'field_name' => 'profile_initial_referrer',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  $profile_fields[] = array(
    'label' => t('Salesforce Account Id'),
    'field_name' => 'profile_salesforce_account_id',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  $profile_fields[] = array(
    'label' => t('Salesforce Contact Id'),
    'field_name' => 'profile_salesforce_contact_id',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'hidden',
      ),
    ),
  );
  // Create the field on the user for each profile.
  foreach ($profile_fields as $profile_field) {
    $field = array(
      'field_name' => $profile_field['field_name'],
      'cardinality' => 1,
      'type' => $profile_field['type'],
    );
    field_create_field($field);
    $instance = array(
      'entity_type' => 'user',
      'bundle' => 'user',
    );
    $instance = array_merge($profile_field, $instance);
    field_create_instance($instance);
  }
  // Set the default fundraiser to webform user profile handling settings.
  variable_set('fundraiser_webform_to_user_map', array(
    'mail' => 'mail',
    'profile_first_field_name' => 'first_name',
    'profile_last_field_name' => 'last_name',
    'profile_address' => 'address',
    'profile_address_line_2' => 'address_line_2',
    'profile_city' => 'city',
    'profile_state' => 'state',
    'profile_zip' => 'zip',
    'profile_country' => 'country')
  );
  // And the same for market source
  variable_set('market_source_to_user_map', array(
    'profile_cid' => 'cid',
    'profile_ms' => 'ms',
    'profile_referrer' => 'referrer',
    'profile_initial_referrer' => 'initial_referrer')
  );
}

/**
 * Configure Fundraiser Ubercart gateways. On the assumption we are using Ubercart here.
 */
function springboard_admin_config_ubercart(&$form, $form_state) {
  // Enable the test payment gateway by default.
  variable_set('uc_pg_test_gateway_enabled', 1);
}

/**
 * Configure Fundraiser Commerce.
 */
function springboard_admin_config_commerce(&$form, $form_state) {
  // Add things as we need to.
}

/**
 * Create the donation_form type.
 */
function springboard_admin_config_donation_form_type(&$form, $form_state) {
  $type = node_type_set_defaults();
  $type->name = t('Donation Form');
  $type->type = 'donation_form';
  $type->description = t('Springboard provided donation form type.');
  $type->title_label = t('Title');
  $type->has_title = ($type->title_label != '');
  $type->base = 'node_content';
  $type->custom = TRUE;
  $type->modified = TRUE;
  $type->module = 'springboard';
  // Save or reset persistent variable values.
  $variables = array(
    'fundraiser' => 1,
    'webform_user' => 1,
    'webform_user_default_fields' => array(
      'webform_user_all_profile_fields' => 'webform_user_all_profile_fields',
    ),
  );
  foreach ($variables as $key => $value) {
    $variable_new = $key . '_' . $type->type;
    if (is_array($value)) {
      $value = array_keys(array_filter($value));
    }
    variable_set($variable_new, $value);
  }
  $status = node_type_save($type);
  node_types_rebuild();
  node_add_body_field($type);
  // Add as a webform.
  $webform_node_types = variable_get('webform_node_types', array('webform'));
  $webform_node_types_primary = variable_get('webform_node_types_primary', array('webform'));
  if (!in_array($type->type, $webform_node_types)) {
    $webform_node_types = array_merge($webform_node_types, array($type->type));
  }
  if (!in_array($type->type, $webform_node_types_primary)) {
    $webform_node_types_primary = array_merge($webform_node_types_primary, array($type->type));
  }
  variable_set('webform_node_types', array_unique($webform_node_types));
  variable_set('webform_node_types_primary', array_unique($webform_node_types_primary));
  drupal_set_message(t('The content type do<em>nation_form</em> has been added.'));
  watchdog('springboard_config', 'Added content type donation_form.');
}

/**
 * Configure the default example Donation form.
 */
function springboard_admin_config_donation_form_add(&$form, $form_state) {
  $node->is_new = 1;
  $node->type = 'donation_form';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->title = $node->internal_name = 'Test Donation Form';
  $node->body['LANGUAGE_NONE'][0]['value'] = 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, imperdiet sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend sit amet, gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce magna sem, gravida in, feugiat ac, molestie eget, wisi. Fusce consectetuer luctus ipsum. Vestibulum nunc. Suspendisse dignissim adipiscing libero. Integer leo. Sed pharetra ligula a dui. Quisque ipsum nibh, ullamcorper eget, pulvinar sed, posuere vitae, nulla. Sed varius nibh ut lacus. Curabitur fringilla. Nunc est ipsum, pretium quis, dapibus sed, varius non, lectus. Proin a quam. Praesent lacinia, eros quis aliquam porttitor, urna lacus volutpat urna, ut fermentum neque mi egestas dolor.';
  $node->body['LANGUAGE_NONE'][0]['summary'] = 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, imperdiet sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend sit amet, gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce magna sem, gravida in, feugiat ac, molestie eget, wisi.';
  $node->body['LANGUAGE_NONE'][0]['format'] = 'filtered_html';
  $node->webform = array(
    'confirmation' => 'Thanks!',
    'confirmation_format' => 'filtered_html',
    'redirect_url' => '<confirmation>',
    'teaser' => 0,
    'block' => 0,
    'allow_draft' => 0,
    'auto_save' => 0,
    'submit_notice' => 1,
    'submit_text' => '',
    'submit_limit' => -1,
    'submit_interval' => -1,
    'total_submit_limit' => -1,
    'total_submit_interval' => -1,
    'status' => 1,
    'promote' => 1,
    'record_exists' => '',
    'roles' => array(
      '0' => 1,
      '1' => 2,
    ),
    'emails' => array(),
    'components' => array(),
  );
  $node->gateway = 'test_gateway';
  $node->amount_wrapper = array(
    'donation_amounts' => array(
      '0' => array(
        'amount' => 10,
        'label' => '$10',
        'remove' => 0,
      ),
      '1' => array(
        'amount' => 20,
        'label' => '$20',
        'remove' => 0,
      ),
      '2' => array(
        'amount' => 50,
        'label' => '$50',
        'remove' => 0,
      ),
      '3' => array(
        'amount' => 100,
        'label' => '$100',
        'remove' => 0,
      ),
    ),
    'show_other_amount' => 0,
    'minimum_donation_amount' => 10,
  );
  $node->is_webform_user = 1;
  $node->send_new_user_email = 0;
  $node->receipt_email_from = ''; 
  $node->receipt_email_address = ''; 
  $node->receipt_email_subject = ''; 
  $node->receipt_email_message = ''; 
  node_save($node);
}

/**
 * Submit handler for the springboard configuration button.
 */
function springboard_admin_ignore(&$form, $form_state) {
  variable_del('springboard_needs_config');
  drupal_set_message(t('Please see the !help page for additional help configuring Springboard.',
    array('!help' => l(t('Help'), 'admin/springboard/help'))));
}