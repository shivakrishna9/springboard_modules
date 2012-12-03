<?php

/**
 * @file
 * Provides an admin interface for the Market Source module.
 */

/**
 * Callback function for the admin settings menu item.
 *
 * @return array
 *   Returns a system_settings_form() formatted Form API array.
 */
function market_source_admin_settings_form(&$form_state) {
  $form = array();

  $form['market_source_default_campaign'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Market Source'),
    '#description' => t('Select a default campaign for your donation forms.'),
    '#default_value' => variable_get('market_source_default_campaign', 'default_ms'),
  );

  if (module_exists('salesforce_management_api')) {
    $form['market_source_validate_sf_campaigns'] = array(
      '#title' => t('Validate Salesforce Campaigns'),
      '#type' => 'checkbox',
      '#description' => t('When enabled, campaigns will be validated against a list of Salesforce campaigns before being saved.'),
      '#default_value' => variable_get('market_source_validate_sf_campaigns', 0),
    );
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#tree' => FALSE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $additional_forms = variable_get('market_source_additional_forms', array());
  if (!is_array($additional_forms)) {
    $additional_forms = array();
  }
  $form['advanced']['market_source_additional_forms'] = array(
    '#title' => t('Additional forms to process'),
    '#type' => 'textarea',
    '#description' => t('Specify any additional forms to process. By default, Market Source will process all webforms and the user registration form. Enter the internal Drupal form ID, one per line.'),
    '#default_value' => implode("\n", $additional_forms),
  );

  // Default global field settings.
  $form['market_source_default_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webform Default Global Fields'),
    '#description' => t('Customize certain aspects of the default webform fields.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['market_source_default_fields']['default_fields_wrapper'] = array(
    '#prefix' => '<div id="marketsource-default-fields">',
    '#suffix' => '</div>',
    '#theme' => 'market_source_admin_global_fields_table',
  );

  // Get current settings for these fields.
  if (!isset($form_state['values']['market_source_default_fields']['default_fields_wrapper'])) {
    $form_state['values']['market_source_default_fields']['default_fields_wrapper'] = variable_get('market_source_default_fields', NULL);
  }
  //set up fields to loop through
  $default_fields_list = array('market_source' => 'Market Source', 'campaign' => 'Campaign ID', 'referrer' => 'Referrer', 'initial_referrer' => 'Initial Referrer');
  //shortcut
  $default_fields_values = &$form_state['values']['market_source_default_fields']['default_fields_wrapper'];

  foreach($default_fields_list as $key=>$field_name) {
    $form['market_source_default_fields']['default_fields_wrapper'][$key] = array(
       'persistence' => array(
         '#type' => 'checkbox',
         '#title' => t('Persistent'),
         '#description' => t('Enables caching of field value per user session.'),
         '#default_value' => isset($default_fields_values[$key]['persistence']) ? $default_fields_values[$key]['persistence'] : TRUE,
         '#size' => 20,
       ),
       'name' => array(
         '#type' => 'item',
         '#title' => t('Human-readable field name'),
         '#value' => $field_name,
       ),
       'key' => array(
         '#type' => 'item',
         '#title' => t('Unique field key'),
         '#value' => $key,
       ),
    );
  }

  // Custom global fields.
  $form['market_source_global_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webform Custom Global Fields'),
    '#description' => t('Any global custom fields created will be added as webform components to your webforms when they are created. Custom fields are not retroactively applied to existing webforms.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['market_source_global_fields']['custom_fields_wrapper'] = array(
    '#prefix' => '<div id="marketsource-custom-fields">',
    '#suffix' => '</div>',
    '#theme' => 'market_source_admin_global_fields_table',
  );
  $custom_fields = &$form['market_source_global_fields']['custom_fields_wrapper']; // shortcut

  // If this is the first time visiting the form, this will be empty.
  if (!isset($form_state['values']['market_source_global_fields']['custom_fields_wrapper'])) {
    $form_state['values']['market_source_global_fields']['custom_fields_wrapper'] = variable_get('market_source_custom_fields', array());
  }
  $custom_field_values = &$form_state['values']['market_source_global_fields']['custom_fields_wrapper']; // shortcut

  // Add a blank row at the end.
  $custom_field_values[] = array();

  foreach ($custom_field_values as $i => $field_value) {
    $custom_fields[$i] = array();
    $custom_fields[$i]['persistence'] = array(
      '#type' => 'checkbox',
      '#title' => t('Persistent'),
      '#description' => t('Enables caching of field value per user session.'),
      '#default_value' => isset($field_value['persistence']) ? $field_value['persistence'] : TRUE,
      '#size' => 20,
    );
    $custom_fields[$i]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Human-readable field name'),
      '#description' => t('This value is displayed in the webform component list.'),
      '#default_value' => isset($field_value['name']) ? $field_value['name'] : '',
      '#size' => 20,
    );
    $custom_fields[$i]['key'] = array(
      '#type' => 'textfield',
      '#title' => t('Unique field key'),
      '#description' => t('This value cannot match any pre-existing field keys.'),
      '#default_value' => isset($field_value['key']) ? $field_value['key'] : '',
      '#size' => 20,
    );
    $custom_fields[$i]['default'] = array(
      '#type' => 'textfield',
      '#title' => t('Default value'),
      '#description' => t('Default value sent to webform.'),
      '#default_value' => isset($field_value['default']) ? $field_value['default'] : '',
      '#size' => 20,
    );
  }
  $form['market_source_global_fields']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add another'),
    '#ahah' => array(
      'path' => 'admin/settings/market-source/add-more-ahah',
      'wrapper' => 'marketsource-custom-fields',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  global $cookie_domain;
  $form['market_source_cookie_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Market Source cookie settings'),
    '#description' => t('The cookie domain name for Market Source cookies.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['market_source_cookie_settings']['market_source_cookie_domain'] = array(
    '#title' => t('Cookie domain'),
    '#type' => 'textfield',
    '#description' => t('This is the domain name Market Source cookies will exist under.'),
    '#default_value' => variable_get('market_source_cookie_domain', $cookie_domain),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * AHAH callback for the Add More button for custom fields.
 */
function market_source_admin_add_more_ahah() {
  $form_state = array(
    'storage' => NULL,
    'submitted' => FALSE,
    'rebuild' => TRUE,
  );
  $form_build_id = $_POST['form_build_id'];
  // Get the form from the cache.
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  // We will run some of the submit handlers so we need to disable redirecting.
  $form['#redirect'] = FALSE;
  // We need to process the form, prepare for that by setting a few internals
  // variables.
  $form['#post'] = $_POST;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  // Build, validate and if possible, submit the form.
  drupal_process_form($form_id, $form, $form_state);
  // This call recreates the form relying solely on the form_state that the
  // drupal_process_form set up.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  // Render the new output.
  $custom_fields = $form['market_source_global_fields']['custom_fields_wrapper'];
  $output = theme('status_messages') . drupal_render($custom_fields);
  // Output JSON with the output.
  drupal_json(array('status' => TRUE, 'data' => $output));
}

/**
 * Validates marketsource admin form.
 */
function market_source_admin_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  // we don't want any bogus field keys or names.
  foreach ($values['market_source_global_fields']['custom_fields_wrapper'] as $i => $field) {
    if (empty($field['name']) XOR empty($field['key'])) {
      $error_key = empty($field['name']) ? 'name' : 'key';
      form_set_error('market_source_global_fields][custom_fields_wrapper][' . $i . '][' . $error_key,
        t('Both the field name and the field key must be provided.'));
    }
    if (!empty($field['name']) && !preg_match('/^[a-zA-Z0-9_+-\s]+$/', $field['name'])) {
      form_set_error('market_source_global_fields][custom_fields_wrapper][' . $i . '][name',
        t('The field name may only contain alphanumeric characters, whitespace and underscores.'));
    }
    if (!empty($field['key']) && !preg_match('/^[a-zA-Z0-9_+-]+$/', $field['key'])) {
      form_set_error('market_source_global_fields][custom_fields_wrapper][' . $i . '][key',
        t('The field key may only contain alphanumeric characters and underscores.'));
    }
    if (!empty($field['default']) && !preg_match('/^[a-zA-Z0-9_+-\s%\[\]]+$/', $field['default'])) {
      form_set_error('market_source_global_fields][custom_fields_wrapper][' . $i . '][default',
        t('The default value may only contain alphanumeric characters, whitespace, underscores, square brackets and the percent sign.'));
    }
  }
}

/**
 * Handle admin form submission.
 */
function market_source_admin_settings_form_submit($form, &$form_state) {
  $values = &$form_state['values'];

  // Set the default campaign value.
  variable_set('market_source_default_campaign', $values['market_source_default_campaign']);
  // Set whether to validate campaigns.
  if (isset($values['market_source_validate_sf_campaigns'])) {
    variable_set('market_source_validate_sf_campaigns', $values['market_source_validate_sf_campaigns']);
  }
  // Save the additional forms.
  $additional_forms = explode("\n", $values['market_source_additional_forms']);
  foreach ($additional_forms as $i => &$val) {
    $val = trim($val);
  }
  variable_set('market_source_additional_forms', array_filter($additional_forms));

  //Save default field settings.
  $default_fields = array();
  if (!empty($values['market_source_default_fields']['default_fields_wrapper'])) {
    foreach ($values['market_source_default_fields']['default_fields_wrapper'] as $key => $default_values) {
      $default_fields[$key] = array(
        'persistence' => $default_values['persistence'],
      );
    }
  }
  variable_set('market_source_default_fields', $default_fields);

  // Save all the custom fields.
  $custom_fields = array();
  if (!empty($values['market_source_global_fields']['custom_fields_wrapper'])) {
    foreach ($values['market_source_global_fields']['custom_fields_wrapper'] as $i => $field) {
      if (!empty($field['key']) && !empty($field['name'])) {
        $custom_fields[] = array(
          'persistence' => $field['persistence'],
          'key' => $field['key'],
          'name' => $field['name'],
          'default' => $field['default'],
        );
      }
    }
  }
  variable_set('market_source_custom_fields', $custom_fields);

  variable_set('market_source_cookie_domain', '');
  $market_source_cookie_domain = $form_state['values']['market_source_cookie_domain'];
  // Per RFC 2109, cookie domains must contain at least one dot other than the
  // first. For hosts such as 'localhost' or IP Addresses we don't set a cookie domain.
  if (count(explode('.', $market_source_cookie_domain)) > 1 && !is_numeric(str_replace('.', '', $market_source_cookie_domain))) {
    variable_set('market_source_cookie_domain', $market_source_cookie_domain);
  }
  elseif (!empty($market_source_cookie_domain)) {
    drupal_set_message(t('Your cookie settings will not work. Either you do not have enough of a domain path to be used '.
      'or you have used an IP address as the domain. The full site domain will be used.'), 'error');
  }
  // Empty can be left alone, empty is ok. The cookie will fall back to the full domain.

  drupal_set_message(t('Market Source settings saved.'));
}

/**
 * Theme custom fields as a table.
 */
function theme_market_source_admin_global_fields_table($element) {
  $header = array();
  $rows = array();

  foreach (element_children($element) as $i) {
    $row = array();
    foreach (element_children($element[$i]) as $f) {
      $this_element = &$element[$i][$f];
      if (isset($this_element['#title'])) {
        if (!isset($header[$f])) {
          $header[$f] = $this_element['#title'];
        }
        unset($this_element['#title']);
        $row[] = drupal_render($this_element);
      }
    }
    $rows[] = array(
      'data' => $row,
    );
  }

  return theme('table', $header, $rows);
}
