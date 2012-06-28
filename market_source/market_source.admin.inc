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
function market_source_admin_settings_form() {
  drupal_add_css(drupal_get_path('module', 'market_source') . '/market_source.admin.css', 'module', 'all');
  $rows = array();
  $default = variable_get('market_source_default_campaign', 'default_ms');
  $custom_fields = variable_get('market_source_custom_fields', array());
  foreach ($custom_fields as $field) {
    $row = array();
    $row[] = array('data' => $field['name']);
    $row[] = array('data' => $field['key']);
    $row[] = array('data' => $field['default']);
    $row[] = array('data' => l(t('remove'), 'admin/config/search/market-source/delete/' . $field['delta']));
    $rows[] = $row;
  }
  $max_field_delta = count($custom_fields) ? count($custom_fields) : 0;
  $form['market_source_default_campaign'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Market Source'),
    '#description' => t('Select a default campaign for your donation forms.'),
    '#default_value' => $default,
  );

  if (module_exists('salesforce_management_api')) {
    $form['market_source_validate_sf_campaigns'] = array(
      '#title' => t('Validate Salesforce Campaigns'),
      '#type' => 'checkbox',
      '#description' => t('When enabled, campaigns will be validated against a list of Salesforce campaigns before being saved.'),
      '#default_value' => variable_get('market_source_validate_sf_campaigns', 0),
    );
  }

  $form['market_source_global_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webform Custom Global Fields'),
    '#description' => '<p>' . t('Any global custom fields created will be added as webform components to your webforms when they are created. Custom fields are not retroactively applied to existing webforms.') . '</p>',
  );
  
  $header = array(
    t('Field Name'),
    t('Form Key'),
    t('Default Value'),
    t('Operations'),
  );
  
  $form['market_source_global_fields']['fields'] = array(
    '#prefix' => '<div id="fields">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  
  
  $form['market_source_global_fields']['new_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Name'),
    '#description' => t('Required: alphanumeric characters and spaces only.'),
    '#required' => TRUE,
  );
  
 $form['market_source_global_fields']['new_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Key'),
    '#description' => t('Required: alphanumeric characters and underscores only.'),
    '#required' => TRUE,
  );
  
  $form['market_source_global_fields']['new_default'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Value'),
    '#description' => t('Optional: text, numbers or webform tokens permitted (ex: %title)'),
  );

  $form['market_source_global_fields']['delta'] = array(
    '#type' => 'hidden',
    '#value' => $max_field_delta,
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
 * Validates marketsource admin form.
 */
function market_source_admin_settings_form_validate($form, &$form_state) {
  // we don't want any bogus field keys or names.

    if ($form_state['values']['new_name'] && !preg_match('/^[a-zA-Z0-9_+-\s]+$/', $form_state['values']['new_name'])) {
      form_set_error('new_name', t('This field may only contain alphanumeric characters, whitespace and underscores.'));
    }
    if ($form_state['values']['new_key'] && !preg_match('/^[a-zA-Z0-9_+-]+$/', $form_state['values']['new_key'])) { 
      form_set_error('new_key', t('This field may only contain alphanumeric characters and underscores.'));
    }
    if ($form_state['values']['new_default'] && !preg_match('/^[a-zA-Z0-9_+-\s]+$/', $form_state['values']['new_default'])) {
      form_set_error('new_default', t('This field may only contain alphanumeric characters, whitespace and underscores.'));
    }

}

/**
 * Handle admin form submission.
 */
function market_source_admin_settings_form_submit($form, &$form_state) {
  $custom_fields = variable_get('market_source_custom_fields', array());

  variable_set('market_source_default_campaign', $form_state['values']['market_source_default_campaign']);
  if (isset($form_state['values']['market_source_validate_sf_campaigns'])) {
    variable_set('market_source_validate_sf_campaigns', $form_state['values']['market_source_validate_sf_campaigns']);
  }

  $new_field = array(
    'name' => $form_state['values']['new_name'],
    'key' => $form_state['values']['new_key'],
    'default' => $form_state['values']['new_default'],
    'delta' => $form_state['values']['delta'],
  );
  
  $custom_fields[] = $new_field;
 

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


function market_source_delete_custom_field($form, $form_state, $delta = NULL) {
  $form = array();
  $form['alert'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('Warning: you are about to permanently remove a custom Market Source field. Are you sure?') . '</p>',
  );
  $form['field_delta'] = array(
    '#type' => 'hidden',
    '#value' => $delta,
  );
  $form['cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#executes_submit_callback' => TRUE,
    '#submit' => array('market_source_remove_cancel'),
  );
  $form['remove'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
  );
  return $form;
}

function market_source_delete_custom_field_submit($form, $form_state) {
  $fields = array();
  $custom_fields = variable_get('market_source_custom_fields', array());
  foreach($custom_fields as $field) {
    if ($field['delta'] != $form_state['values']['field_delta']) {
      $fields[] = $field;
    }
  }
  variable_set('market_source_custom_fields', $fields);
  drupal_goto('admin/config/search/market-source');
}

function market_source_remove_cancel() {
  drupal_goto('admin/config/search/market-source');
}
