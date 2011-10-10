<?php
// $Id$

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
function market_source_admin_settings() {
  drupal_add_css(drupal_get_path('module', 'market_source') . '/market_source.admin.css', 'module', 'all');
  
  $default = variable_get('market_source_default_campaign', 'default_ms');
  $custom_fields = variable_get('market_source_custom_fields', array());
  
  $form['market_source_default_campaign'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Market Source'),
    '#description' => t('Select a default campaign for your donation forms.'),
    '#default_value' => $default,
  );

  $form['market_source_validity_period'] = array(
    '#title' => 'Validity Period',
    '#description' => 'Determines how long values should be stored in the user\'s session.<br /><strong>Note:</strong> Values do not persist if the user\'s session ends.',
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => array(
      6 => t('6 Hours'),
      12 => t('12 Hours'),
      24 => t('1 Day'),
      48 => t('2 Days'),
      72 => t('3 Days'),
      96 => t('4 Days'),
      120 => t('5 Days'),
      144 => t('6 Days'),
      168 => t('1 Week'),
      336 => t('2 Weeks'),
      504 => t('3 Weeks'),
      672 => t('4 Weeks'),
      0 => t('Always Valid'),
    ),
    '#default_value' => variable_get('market_source_validity_period', 6),
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
    '#description' => t('Any global custom fields created will be added as webform components to your webforms when they are created. Custom fields are not retroactively applied to existing webforms.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['market_source_global_fields']['custom_fields_wrapper'] = array(
    '#prefix' => '<div id="marketsource-custom-fields">',
    '#suffix' => '</div>',
  );
  // Add UI for any pre-existing fields
  if (count($custom_fields)) {

    for ($i = 0; $i < 5; $i++) {
      if(!isset($custom_fields[$i])) {
        _market_source_add_field_elements($form, array('key' => '', 'name' => ''), $i);
      }
      else {
        _market_source_add_field_elements($form, $custom_fields[$i], $i);
      }
    }
  }
  else {
    for ($i = 0; $i < 5; ++$i) {
       _market_source_add_field_elements($form, array('key' => '', 'name' => '',), $i);
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  $form['#validate'][] = 'market_source_validate_admin_form';
  $form['#submit'][] = 'market_source_submit_admin_form';
  return $form;
}


/**
 * Validates marketsource admin form.
 */
function market_source_validate_admin_form(&$form, $form_state) {
  // we don't want any bogus field keys or names.
  for ($i = 0; $i < 5; ++$i) {
    if ($form_state['values']['ms_custom_field_name_' . $i] && !preg_match('/^[a-zA-Z0-9_+-\s]+$/', $form_state['values']['ms_custom_field_name_' . $i])){ 
		form_set_error('ms_custom_field_name_' . $i, t('This field may only contain alphanumeric characters, whitespace and underscores.'));
	}
    if ($form_state['values']['ms_custom_field_key_' . $i] && !preg_match('/^[a-zA-Z0-9_+-]+$/', $form_state['values']['ms_custom_field_key_' . $i])){ 
		form_set_error('ms_custom_field_key_' . $i, t('This field may only contain alphanumeric characters and underscores.'));
	}
    if ($form_state['values']['ms_custom_field_default_' . $i] && !preg_match('/^[a-zA-Z0-9_+-\s]+$/', $form_state['values']['ms_custom_field_default_' . $i])){ 
		form_set_error('ms_custom_field_default_' . $i, t('This field may only contain alphanumeric characters, whitespace and underscores.'));
	}
  }
}

/**
 * Handle admin form submission.
 */
function market_source_submit_admin_form($form, $form_state) {
  $custom_fields = array();
  
  variable_set('market_source_default_campaign', $form_state['values']['market_source_default_campaign']);
  variable_set('market_source_validity_period', $form_state['values']['market_source_validity_period']);
  if (module_exists('salesforce_management_api')) {
    variable_set('market_source_validate_sf_campaigns', $form_state['values']['market_source_validate_sf_campaigns']);
  }
  
  for ($i = 0; $i < 5; ++$i) {
    if ($form_state['values']['ms_custom_field_name_' . $i] && $form_state['values']['ms_custom_field_name_' . $i]) {
      $custom_fields[] = array(
        'key' => $form_state['values']['ms_custom_field_key_' . $i],
        'name' => $form_state['values']['ms_custom_field_name_' . $i],
        'default' => $form_state['values']['ms_custom_field_default_' . $i],
        'delta' => $i,
      );
    }
  }
  variable_set('market_source_custom_fields', $custom_fields);
}

/**
 * 
 */
function _market_source_add_field_elements(&$form, $custom_field, $delta) {

  $fieldset = 'fieldset_' . ($delta + 1);
  $collapsed = $delta ? TRUE : FALSE;
  $form['market_source_global_fields']['custom_fields_wrapper'][$fieldset] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Field #%number', array('%number' => ($delta + 1))),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );
  $form['market_source_global_fields']['custom_fields_wrapper'][$fieldset]['ms_custom_field_name_' . $delta] = array(
    '#type' => 'textfield',
    '#title' => t('Field Name'),
    '#description' => t('The human-readable field name. This value is displayed in the webform component list.'),
    '#default_value' => $custom_field['name'],
  );
  $form['market_source_global_fields']['custom_fields_wrapper'][$fieldset]['ms_custom_field_key_' . $delta] = array(
    '#type' => 'textfield',
    '#title' => t('Field Key'),
    '#description' => t('The unique machine name of this field. This value cannot match any pre-existing field keys.'),
    '#default_value' => $custom_field['key'],
  );
  $form['market_source_global_fields']['custom_fields_wrapper'][$fieldset]['ms_custom_field_default_' . $delta] = array(
    '#type' => 'textfield',
    '#title' => t('Default Value'),
    '#default_value' => $custom_field['default'],
    '#suffix' => '<div style="clear:both"></div>',
  );
  return $form;
}