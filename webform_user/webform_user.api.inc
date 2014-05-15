<?php

/**
 * @file
 * API documentation for webform_user.
 */

/**
 * @param array $updates
 *   An array of profile fields that will be updated to the database. Webform_user
 *   will first delete any existing values, and then write the new ones back. This
 *   sidesteps the user_save() function and writes directly to the profile_values table
 *
 *   Array data is in this format:
 *     $updates[] = array(
 *       'fid' => $fid,
 *       'name' => $field_name,
 *       'type' => $field_type,
 *       'data' => $data,
 *      );
 */
function hook_webform_user_save_profile_alter(&$updates) {
  // Remove extra white space on profile fields
  foreach ($updates as $id => $field) {
    if (is_string($field['data'])) {
      $updates[$id]['data'] = trim($field['data']);
    }
  }
}
