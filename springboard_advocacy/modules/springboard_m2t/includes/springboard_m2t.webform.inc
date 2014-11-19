<?php

/**
 * @file
 * webform functions for Springboard M2T.
 */


/**
 * Implements hook_webform_user_profile_fields_alter
 *
 */
function springboard_m2t_webform_user_profile_fields_alter(&$fields, $node) {
  if($node->type == 'springboard_m2t') {
    foreach ($fields as $index => $profile_field) {
      //make all fields except address2 mandatory
      if ($profile_field['name'] != 'sbp_address_line_2'){
        $fields[$index]['mandatory'] = 1;
      }
      //setup removeal of non-US provinces
      if ($profile_field['name'] == 'sbp_state'){
        unset($fields[$index]);
      }
      //remove zip
      if ($profile_field['name'] == 'sbp_zip'){
        unset($fields[$index]);
      }
      //move email field lower
      if ($profile_field['name'] == 'mail'){
        $fields[$index]['weight'] = 14;
      }
    } 
  }
}

/**
 * Implements hook_webfrom_submission insert
 */
function springboard_m2t_webform_submission_insert($node, $submission) {
  if(isset($_SESSION['m2t_geo']) && $node->type == 'springboard_m2t') {
    $data = array(
      'nid' => $submission->nid,
      'sid' => $submission->sid,
      'uid' => $submission->uid,
      'zip' => $_SESSION['m2t_geo']['zip'],
      'plus4' => $_SESSION['m2t_geo']['plus'],
      'lat' => $_SESSION['m2t_geo']['latitude'],
      'lon' => $_SESSION['m2t_geo']['longitude'],
    );
    drupal_write_record('springboard_m2t_location', $data);
    unset($_SESSION['m2t_geo']);
  };
}