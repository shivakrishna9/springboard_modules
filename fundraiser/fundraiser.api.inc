<?php

/**
 * @file
 * API documentation for fundraiser.
 */

/**
 * @param array $updates
 *   An array of profile fields that will be updated to the database. Fundraiser
 *   will first delete any existing values, and then write the new ones back. This
 *   sidesteps the user_save() function and writes directly to the profile_values table
 * @param string $category
 *   The profile category that the fields are in
 */
function hook_fundraiser_save_profile_alter(&$updates, $category) {
  // Remove extra white space on profile fields
  foreach ($updates as $id => $field) {
    if (is_string($field['data'])) {
      $updates[$id]['data'] = trim($field['data']);
    }
  }
}
