<?php
/**
 * @file
 * Administration page callbacks for the webform_dupe_cop module.
 */
 
/**
 * Settings form for webform_user
 */
function webform_dupe_cop_settings_form() {
  $form['webform_dupe_cop_hours'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#title' => t('Time span for dupe protection (in hours)'),
    '#default_value' => variable_get('webform_dupe_cop_hours', 0),
    '#description' => t('Each webform submission will be protected from duplicates for this period after the time when it was first submitted. Enter 0 to protect each submission forever.'),
  );
  
  $form['webform_dupe_cop_message'] = array(
    '#type' => 'textarea',
    '#rows' => 3,
    '#title' => t('Message to display on dupe submission'),
    '#default_value' => variable_get('webform_dupe_cop_message', ''),
    '#description' => t('Optional. You can use [date] and [time] to substitute the date and time of the original submission.'),
  );
  
	// add our own validation handler
	$form['#validate'][] = 'webform_dupe_cop_settings_form_validate';
//	$form['#submit'][] = 'webform_user_settings_form_submit';
	
	return system_settings_form($form);
}

function webform_dupe_cop_settings_form_validate($form, &$form_settings) {
  if (!is_numeric($form_settings['values']['webform_dupe_cop_hours'])) {
    form_set_error('webform_dupe_cop_hours', t('Time span must be a number'));
  }
}
