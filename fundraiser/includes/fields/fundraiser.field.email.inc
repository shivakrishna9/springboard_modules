<?php
/**
 * @file
 * Various functions for the Email field.
 */

/**
 * Validation callback for field email. Called during form validation.
 */
function _fundraiser_email_field_validate($form, $form_state, $submission_fields, $value) {
  if (!valid_email_address(trim($value))) {
    return array('mail' => t('You must enter a valid email address.'));
  }
}
