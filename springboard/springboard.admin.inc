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
//        'ubercart' => t('Ubercart'), // Ubercart default support deprc.
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
      '#states' => array(
        'visible' => array(
          ':input[name="sf_enable"]' => array('checked' => TRUE),
        ),
      ),
    );
    $form['springboard_config']['settings']['sf']['sf_donation'] = array(
      '#title' => t('Enable Salesforce Donation Integration'),
      '#type' => 'checkbox',
    );
    $form['springboard_config']['settings']['sf']['sf_npsp'] = array(
      '#title' => t('Enable Salesforce Nonprofit Starter Pack Integration'),
      '#type' => 'checkbox',
    );
    $form['springboard_config']['settings']['sf']['sf_help'] = array(
      '#type' => 'item',
      '#markup' => t('Authorize this website to communicate with Salesforce by entering the consumer key and ' .
        'secret from a remote application. Clicking authorize will redirect you to Salesforce where you will be ' .
        'asked to grant access.'),
    );
    $form['springboard_config']['settings']['sf']['sf_key'] = array(
      '#title' => t('Salesforce consumer key'),
      '#type' => 'textfield',
      '#required' => TRUE,
    );
    $form['springboard_config']['settings']['sf']['sf_secret'] = array(
      '#title' => t('Salesforce consumer secret'),
      '#type' => 'textfield',
      '#required' => TRUE,
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
  // Clear all caches.
  drupal_flush_all_caches();
  // Set up module specific configurations.
  springboard_admin_config_springboard($form, $form_state);
  if ($form_state['values']['order_handling'] == 'ubercart') {
    springboard_admin_config_ubercart($form, $form_state);
  }
  else {
    springboard_admin_config_commerce($form, $form_state);
  }
  if ($form_state['values']['sf_enable']) {
    springboard_admin_config_salesforce($form, $form_state);
  }
  // Configure user profiles
  springboard_admin_config_profile($form, $form_state);
  // Configure a default fundraiser type
  springboard_admin_config_donation_form_type($form, $form_state);
  // Clear all caches.
  drupal_flush_all_caches();
  // Create an example form.
  $email_nid = springboard_admin_config_email_wrapper_add($form, $form_state);
  springboard_admin_config_donation_form_add($form, $form_state, $email_nid);
  // Clear all caches one last time.
  drupal_flush_all_caches();
  menu_rebuild();
}

/**
 * Turn on a standard set of modules.
 */
function springboard_admin_config_modules($form, $form_state) {
  // Springboard custom.
  // Enable does recursion so we don't need to dig deeper.
  $modules = array(
    'fundraiser',
    'fundraiser_webform',
    'fundraiser_sustainers',
    'fundraiser_triggers',
    'market_source',
    'webform_user',
    'webform_confirmations',
    'email_wrappers',
    'springboard_views',
    'springboard_version',
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
  // Optional Salesforce modules.
  if ($form_state['values']['sf_enable']) {
    $modules[] = 'salesforce';
    $modules[] = 'salesforce_log';
    $modules[] = 'salesforce_genmap';
    if ($form_state['values']['sf_donation']) {
      $modules[] = 'salesforce_donation';
    }
    if ($form_state['values']['sf_npsp']) {
      $modules[] = 'salesforce_npsp';
    }
  }
  module_enable($modules);
  drupal_set_message(t('Modules %modules have been enabled.', array('%modules' =>implode(', ', $modules))));
}

/**
 * Configure Springboard iteself.
 */
function springboard_admin_config_springboard($form, $form_state) {
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
 * Configure Fundraiser Ubercart gateways. On the assumption we are using Ubercart here.
 */
function springboard_admin_config_ubercart($form, $form_state) {
  // Enable the test payment gateway by default.
  variable_set('uc_pg_test_gateway_enabled', 1);
}

/**
 * Configure Fundraiser Commerce.
 */
function springboard_admin_config_commerce($form, $form_state) {
  // Add things as we need to.
}

/**
 * Configure the default Salesforce settings.
 */
function springboard_admin_config_salesforce($form, $form_state) {
  // Market source auto checks campaigns.
  variable_set('market_source_validate_sf_campaigns', TRUE);
  // Set keys and secrets
  variable_set('salesforce_consumer_key', $form_state['values']['sf_key']);
  variable_set('salesforce_consumer_secret', $form_state['values']['sf_secret']);
  // To do, set oauth up.
}

/**
 * Configure a default set of User profile fields.
 */
function springboard_admin_config_profile($form, $form_state) {
  // See http://drupal.org/node/874026 for discussion re: Profile in D7.
  // By preference we use field API.
  // Add profile fields
  $profile_fields = array();
  $profile_fields[] = array(
    'label' => t('First name'),
    'field_name' => 'springboard_first_name',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Last name'),
    'field_name' => 'springboard_last_name',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Address'),
    'field_name' => 'springboard_address',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Address Line 2'),
    'field_name' => 'springboard_address_line_2',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  ); 
  $profile_fields[] = array(
    'label' => t('City'),
    'field_name' => 'springboard_city',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  if ($form_state['values']['order_handling'] == 'ubercart') {
    // Default options loaded from {uc_zones} On the assumption we are using Ubercart here.
    // Load State/Province options from {uc_zones}, UC default is zone codes for US & Canada.
    $countries = fundraiser_ubercart_get_countries();
    $zones = '';
    foreach ($countries->zones as $zone) {
       $zones .= $zone->zone_id . '|' . $zone->zone_name . "\n";
    }
    $profile_fields[] = array(
      'label' => t('State/Province'),
      'field_name' => 'springboard_state',
      'type' => 'list_text',
      'widget' => array(
        'type' => 'options_select',
      ),
      'settings' => array('allowed_values' => $zones),
    );
  }
  elseif ($form_state['values']['order_handling'] == 'commerce') {
    // Commerce does not store countries in a database, it relies instead on locale.inc
    $countries = fundraiser_commerce_get_countries();
    $zones = '';
    foreach ($countries->zones as $zone) {
       $zones .= $zone->zone_id . '|' . $zone->zone_name . "\n";
    }
    // For Commerce or others for now.
    $profile_fields[] = array(
      'label' => t('State/Province'),
      'field_name' => 'springboard_state',
      'type' => 'list_text',
      'widget' => array(
        'type' => 'options_select',
      ),
      'settings' => array('allowed_values' => $zones),
    );
  }
  else {
    // For Commerce or others for now.
    $profile_fields[] = array(
      'label' => t('State/Province'),
      'field_name' => 'springboard_state',
      'type' => 'text',
      'widget' => array(
        'type' => 'text_textfield',
      ),
    );
  }
  $profile_fields[] = array(
    'label' => t('Postal Code'),
    'field_name' => 'springboard_zip',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Country'),
    'field_name' => 'springboard_country',
    'type' => 'text',
    'widget' => array(
      'type' => 'text_textfield',
    ),
  );
  $profile_fields[] = array(
    'label' => t('Campaign ID'),
    'field_name' => 'springboard_cid',
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
    'field_name' => 'springboard_ms',
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
    'field_name' => 'springboard_referrer',
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
    'field_name' => 'springboard_initial_referrer',
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
    'label' => t('Search Engine'),
    'field_name' => 'springboard_search_engine',
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
    'label' => t('Search String'),
    'field_name' => 'springboard_search_string',
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
    'label' => t('User Agent'),
    'field_name' => 'springboard_user_agent',
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
    'field_name' => 'springboard_salesforce_account_id',
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
    'field_name' => 'springboard_salesforce_contact_id',
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
    'springboard_first_name' => 'first_name',
    'springboard_last_name' => 'last_name',
    'springboard_address' => 'address',
    'springboard_address_line_2' => 'address_line_2',
    'springboard_city' => 'city',
    'springboard_state' => 'state',
    'springboard_zip' => 'zip',
    'springboard_country' => 'country',
    )
  );
  // And the same for market source
  variable_set('market_source_to_user_map', array(
    'springboard_cid' => 'cid',
    'springboard_ms' => 'ms',
    'springboard_referrer' => 'referrer',
    'springboard_initial_referrer' => 'initial_referrer',
    'springboard_search_engine' => 'search_engine',
    'springboard_search_string' => 'search_string',
    'springboard_user_agent' => 'user_agent',
    )
  );
}

/**
 * Create the donation_form type.
 */
function springboard_admin_config_donation_form_type($form, $form_state) {
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
    'node_submitted' => 0,
    'comment' => COMMENT_NODE_HIDDEN,
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
  $webform_node_types = array_merge($webform_node_types, array('donation_form'));
  $webform_node_types_primary = array_merge($webform_node_types_primary, array('donation_form'));
  variable_set('webform_node_types', array_unique($webform_node_types));
  variable_set('webform_node_types_primary', array_unique($webform_node_types_primary));
  // Done.
  drupal_set_message(t('The content type <em>donation_form</em> has been added.'));
  watchdog('springboard_config', 'Added content type donation_form.');
}

/**
 * Create an example Email Wrapper.
 */

function springboard_admin_config_email_wrapper_add($form, $form_state) {
  $node->is_new = 1;
  $node->type = 'email_wrapper';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->title ='Example Email Wrapper';
  $node->status = 1;
  $node->promote = 1;
  $node->email_wrapper_from_name[LANGUAGE_NONE][0] = array(
    'value' => 'Springboard',
    'format' => '',
    'safe_value' => 'Springboard',
  );
  $node->email_wrapper_from_email[LANGUAGE_NONE][0] = array(
    'value' => 'example-from@example.com',
    'format' => '',
    'safe_value' => 'example-from@example.com',
  );
  $node->email_wrapper_reply_email[LANGUAGE_NONE][0] = array(
    'value' => 'example-to@example.com',
    'format' => '',
    'safe_value' => 'example-to@example.com',
  );
  $node->email_wrapper_subject[LANGUAGE_NONE][0] = array(
    'value' => 'Thank you for your generous donation!',
    'format' => '',
    'safe_value' => 'Thank you for your generous donation!',
  );
  $node->email_wrapper_html_template[LANGUAGE_NONE][0] = array(
    'value' => 'Thank you for your generous donation!' . "\n" .
      "\n" .
      '%html_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
    'summary' => '',
    'format' => 'filtered_html',
    'safe_value' => '<p>Thank you for your generous donation!</p>' . "\n" .
      '<p>%html_message</p>' . "\n" .
      '<p>-----<br>' . "\n" .
      'Springboard Example</p>',
  );
  $node->email_wrapper_html_message[LANGUAGE_NONE][0] = array(
    'value' => 'Please visit us again soon.',
    'summary' => '',
    'format' => 'filtered_html',
    'safe_value' => '<p>Please visit us again soon.</p>',
  );
  $node->email_wrapper_text_template[LANGUAGE_NONE][0] = array(
    'value' =>  'Thank you for your generous donation!' . "\n" .
      "\n" .
      '%text_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
    'format' => '',
    'safe_value' => 'Thank you for your generous donation!' . "\n" .
      "\n" .
      '%text_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
  );
  $node->email_wrapper_text_message[LANGUAGE_NONE][0] = array(
    'value' => 'Please visit us again soon.',
    'format' => 'filtered_html',
    'safe_value' => '<p>Please visit us again soon.</p>',
  );
  node_save($node);
  // Save extras.
  drupal_set_message(t('An example donation form has been created.'));
  return $node->nid;
}

/**
 * Create an example Fundraiser form.
 */
function springboard_admin_config_donation_form_add($form, $form_state, $email_nid = NULL) {
  $node->is_new = 1;
  $node->type = 'donation_form';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->title = $node->internal_name = 'Test Donation Form';
  $node->status = 1;
  $node->promote = 1;
  $node->body['LANGUAGE_NONE'][0]['value'] = 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, ' .
    'imperdiet sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend ' .
    'sit amet, gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce ' .
    'magna sem, gravida in, feugiat ac, molestie eget, wisi. Fusce consectetuer luctus ipsum. Vestibulum nunc. ' .
    'Suspendisse dignissim adipiscing libero. Integer leo. Sed pharetra ligula a dui. Quisque ipsum nibh, ullamcorper ' .
    'eget, pulvinar sed, posuere vitae, nulla. Sed varius nibh ut lacus. Curabitur fringilla. Nunc est ipsum, ' .
    'pretium quis, dapibus sed, varius non, lectus. Proin a quam. Praesent lacinia, eros quis aliquam porttitor, ' .
    'urna lacus volutpat urna, ut fermentum neque mi egestas dolor.';
  $node->body['LANGUAGE_NONE'][0]['summary'] = 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, imperdiet ' .
    'sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend sit amet, ' .
    'gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce magna sem, gravida ' .
    'in, feugiat ac, molestie eget, wisi.';
  $node->body['LANGUAGE_NONE'][0]['format'] = 'filtered_html';
  $node->confirmation_page_title = 'Thank you for your donation!';
  $node->webform = array(
      'confirmation' => 'Thank you [donation:first_name] [donation:last_name] for your donation of [donation:amount] USD.' . "\n" .
      "\n" .
      'The email address we have for you is: [donation:mail]' . "\n" .
      "\n" .
      'The address we have on record for you is:' . "\n" .
      '[donation:address] [donation:address_line_2]' . "\n" .
      '[donation:city], [donation:state] [donation:zip]' . "\n" .
      '[donation:country]' . "\n" .
      "\n" .
      'Your card ending in [donation:card_number], expires [donation:card_expiration_month] / [donation:card_expiration_year]',
    'confirmation_format' => 'filtered_html',
    'redirect_url' => '<confirmation>',
    'teaser' => 0,
    'block' => 0,
    'allow_draft' => 0,
    'auto_save' => 0,
    'submit_notice' => 1,
    'submit_text' => t('Donate'),
    'submit_limit' => -1,
    'submit_interval' => -1,
    'total_submit_limit' => -1,
    'total_submit_interval' => -1,
    'status' => 1,
    'record_exists' => '',
    'roles' => array(
      '0' => 1,
      '1' => 2,
    ),
    'emails' => array(),
    'components' => array(),
  );
  if ($email_nid != NULL) {
    $node->webform['emails'][1] = array(
      'email' => 6,
      'template' => 'default',
      'subject' => 'Thank you for your generous donation!',
      'from_name' => 'Springboard',
      'from_address' => 'example-from@example.com',
      'html' => 0,
      'attachments' => 0,
    );
  }
  $node->gateways = 'commerce_payment_example|commerce_payment_commerce_payment_example';
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
    'show_other_amount' => 1,
    'minimum_donation_amount' => 10,
  );
  $node->is_webform_user = 1;
  $node->send_new_user_email = 0;
  node_save($node);
  $node = node_load($node->nid);
  // Save confirmation details.
  $confirmation['nid'] = $node->nid;
  $confirmation['confirmation_page_title'] = $node->confirmation_page_title;
  // Save email wrapper details.
  if ($email_nid != NULL) {
    $settings = array(
    'nid' => $node->nid,
    'eid' => $node->webform['emails'][1]['eid'],
    'tid' => $email_nid,
    'bcc_email' => implode(',', $values['email_wrappers_bcc']),
    'html_message' => 'Thank you [donation:first_name] [donation:last_name] for your donation of [donation:amount] USD.' . "\n" .
      "\n" .
      'The email address we have for you is: [donation:mail]' . "\n" .
      "\n" .
      'Your card ending in [donation:card_number], expires [donation:card_expiration_month] / [donation:card_expiration_year]',
    'html_message_format' => 'filtered_html',
    'text_message' => 'Thank you [donation:first_name] [donation:last_name] for your donation of [donation:amount] USD.' . "\n" .
      "\n" .
      'The email address we have for you is: [donation:mail]' . "\n" .
      "\n" .
      'Your card ending in [donation:card_number], expires [donation:card_expiration_month] / [donation:card_expiration_year]',
    'extra' => array(),
    );
  }
  _webform_confirmations_update($confirmation);
  drupal_set_message(t('An example donation form has been created.'));
}

/**
 * Submit handler for the springboard configuration button.
 */
function springboard_admin_ignore($form, $form_state) {
  variable_del('springboard_needs_config');
  drupal_set_message(t('Please see the !help page for additional help configuring Springboard.',
    array('!help' => l(t('Help'), 'admin/springboard/help'))));
}