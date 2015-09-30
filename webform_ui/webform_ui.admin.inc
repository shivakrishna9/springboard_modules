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

  $form['webform_ui_use_alternate_submission'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use an alternate submission results page with pager and search functionality'),
    '#default_value' => variable_get('webform_ui_use_alternate_submission', 'FALSE'),
    '#description' => '',
  );

  $form['webform_ui_hide_clear_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide the "Clear" tab.'),
    '#default_value' => variable_get('webform_ui_hide_clear_all', 'FALSE'),
    '#description' => t("Prevent access to the mass delete feture onthe submission results page."),
  );

  $form['webform_ui_hide_table'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide the "Table" tab only on the submissions results page'),
    '#default_value' => variable_get('webform_ui_hide_clear_all', 'FALSE'),
    '#description' => t("If you have many submissions, the table page will require extensive resources to load. If you don't need this feature, disable the link."),
  );

  $form['webform_ui_use_alternate_submission'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use an alternate submission results page with pager and search functionality'),
    '#default_value' => variable_get('webform_ui_use_alternate_submission', 'FALSE'),
    '#description' => '',
  );

  $form['#submit'][] = 'webform_ui_settings_form_submit';
  return system_settings_form($form);
}

function webform_ui_settings_form_submit($form, $form_state) {
  variable_set('menu_rebuild_needed', TRUE);
}

