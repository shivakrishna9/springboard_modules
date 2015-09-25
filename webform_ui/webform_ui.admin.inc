<?php
/**
 * @file
 * Administration page callbacks for the webform_dupe_cop module.
 */

/**
 * Admin settings form.
 */
function webform_ui_settings_form($form, &$form_state) {

  $form['webform_ui_hide_hidden'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide fields of the type "hidden" on the webform components page.'),
    '#default_value' => variable_get('webform_ui_hide_hidden', 'FALSE'),
    '#description' => t('Hide rarely edited hidden fields by default. A "show" link will allow you to toggle field visiblity.'),
  );

  return system_settings_form($form);
}


