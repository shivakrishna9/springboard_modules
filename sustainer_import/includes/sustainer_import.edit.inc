<?php

/**
 * @file import record edit form and associated helper functions.
 */

/**
 * Import record edit form.
 */
function sustainer_import_edit_record() {
  $args = func_get_args();
  return array();
}

/**
 * Validation callback for edit form.
 */
function sustainer_import_edit_record_validate(&$form, $form_state) {

}

/**
 * Submit callback for edit form.
 */
function sustainer_import_edit_record_submit($form, $form_state) {
  dsm($form_state['values'], 'form state');
}

