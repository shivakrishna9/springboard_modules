<?php
// $Id$

/**
 * @file
 * Webform A/B Testing admin functions
 */


function webform_ab_admin_settings(&$form_state) {
  $form = array();
  
  $form['webform_ab_ignore_hits_from_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles to ignore hits from'),
    '#description' => t('Hits and conversions will be ignored from users of the chosen roles.'),
    '#options' => user_roles(),
    '#default_value' => variable_get('webform_ab_ignore_hits_from_roles', ''),
  );
  $form['webform_ab_ignore_hits_from_ips'] = array(
    '#type' => 'textarea',
    '#title' => t('IPs to ignore hits from'),
    '#description' => t('Ignore webform hits and conversions from users in the listed IPs. List one IP per row.'),
    '#cols' => 60,
    '#rows' => 5,
    '#default_value' => variable_get('webform_ab_ignore_hits_from_ips', ''),
  );
  
  
  return system_settings_form($form);
}

/**
 * Menu callback for the Reports and Monitoring tab on Test A/B
 * 
 * @param $node Node object
 */
function webform_ab_test_reports($node) {
  $return = '';
  
  // Get total hit counts for all webforms
  $hit_counts_result = db_query("SELECT webform_nid, count(hit_id) hits FROM {webform_ab_hits} WHERE test_nid = %d GROUP BY test_nid, webform_nid", $node->nid);
  $hit_counts = array();
  while ($row = db_fetch_object($hit_counts_result)) {
    $hit_counts[$row->webform_nid] = $row->hits;
  }
  
  // Get total conversions for all webforms
  $conversion_counts_result = db_query("SELECT webform_nid, count(sid) conversions FROM {webform_ab_conversion} WHERE test_nid = %d GROUP BY test_nid, webform_nid", $node->nid);
  $conversion_counts = array();
  while ($row = db_fetch_object($conversion_counts_result)) {
    $conversion_counts[$row->webform_nid] = $row->conversions;
  }
  
  // If one of the webforms won, determine pre-win and post-win conversions
  if (($winning_webform_nid = _webform_ab_has_winner_already($node)) > 0) {
    // Get pre- and post-win hit counts
    $result = db_query("SELECT SUM(IF(hits.time < f.win_time, 1, 0)) AS post_win, SUM(IF(hits.time >= f.win_time, 1, 0)) AS pre_win 
      FROM {webform_ab_forms} f 
      LEFT JOIN {webform_ab_hits} hits ON hits.test_nid = f.test_nid AND hits.webform_nid = f.webform_nid 
      WHERE f.test_nid = %d AND f.webform_nid = %d 
      GROUP BY f.test_nid, f.webform_nid", array($node->nid, $winning_webform_nid));
    $post_win_hit_data = db_fetch_array($result);
    
    
    // Get pre- and post-win conversions counts
    $result = db_query("SELECT SUM(IF(sub.submitted < f.win_time, 1, 0)) AS post_win, SUM(IF(sub.submitted >= f.win_time, 1, 0)) AS pre_win 
      FROM webform_ab_forms f 
      LEFT JOIN webform_ab_conversion con ON con.test_nid = f.test_nid AND con.webform_nid = f.webform_nid
      LEFT JOIN webform_submissions sub ON con.sid = sub.sid
      WHERE f.test_nid = %d AND f.webform_nid = %d
      GROUP BY f.test_nid, f.webform_nid", array($node->nid, $winning_webform_nid));
    $post_win_conversion_data = db_fetch_array($result);
  }
  
  // Status labels
  $webform_ab_status_labels = array(
    WEBFORM_AB_WEBFORM_STATUS_ACTIVE => t('Active'),
    WEBFORM_AB_WEBFORM_STATUS_WINNER => t('Winner'),
    WEBFORM_AB_WEBFORM_STATUS_RETIRED => t('Retired'),
  );  
    
  // Show the list of currently-included forms
  $headers = array(
    t('Status'),
    t('Name'), 
    t('Hits'), 
    t('Conversions'), 
    t('Conversion %'),
    t('Pre-win Hits'), 
    t('Pre-win Conversions'), 
    t('Pre-win Conversion %'),
    t('Post-win Hits'), 
    t('Post-win Conversions'), 
    t('Post-win Conversion %'),
  );
  $rows = array();
  
  // Loop over the webforms and add a row to the report table
  foreach ($node->included_webforms as $webform_nid => $webform_details) {
    $line = array(
      $webform_ab_status_labels[$webform_details['status']],
      $webform_details['title'], 
      $hit_counts[$webform_nid],
      $conversion_counts[$webform_nid],
      ($hit_counts[$webform_nid] > 0) ? round(100 * $conversion_counts[$webform_nid] / $hit_counts[$webform_nid]) . '%' : '0%'
    );
    
    // If this webform is the winner, show pre- and post-win conversion percentages
    if ($winning_webform_nid == $webform_nid) {
      $line[] = $post_win_hit_data['pre_win'];
      $line[] = $post_win_conversion_data['pre_win'];
      $line[] = round(100 * $post_win_conversion_data['pre_win'] / $post_win_hit_data['pre_win']) . '%';
      $line[] = $post_win_hit_data['post_win'];
      $line[] = $post_win_conversion_data['post_win'];
      $line[] = round(100 * $post_win_conversion_data['post_win'] / $post_win_hit_data['post_win']) . '%';
    }
    else {
      // Not the winning webform. Pad out into the columns for win data
      while (count($line) < count($headers)) {
        $line[] = '';
      }
    }
    
    $rows[] = $line;
  }
  
  // If there were no rows, show an empty message
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No webforms have been added to this A/B test. Visit the !forms_tab tab to add webforms.', array('!forms_tab' => l('Included Webforms', 'node/' . $node->nid . '/webforms'))), 'colspan' => count($headers)));
  }
  $return .= theme('table', $headers, $rows);
  
  return $return;  
}


/**
 * Menu callback for the Included Forms tab on A/B Test nodes
 * 
 * @param $node Node object
 */
function webform_ab_test_webforms($node) {
  $return = '';
  
  // Status labels
  $webform_ab_status_labels = array(
    WEBFORM_AB_WEBFORM_STATUS_ACTIVE => t('Active'),
    WEBFORM_AB_WEBFORM_STATUS_WINNER => t('Winner'),
    WEBFORM_AB_WEBFORM_STATUS_RETIRED => t('Retired'),
  );
  
  // Show the list of currently-included forms
  $headers = array(t('Name'), t('Status'), t('Operations'));
  $rows = array();
  foreach ($node->included_webforms as $webform_nid => $webform_details) {
    $rows[] = array(
      $webform_details['title'], 
      $webform_ab_status_labels[$webform_details['status']],
      l('edit', 'node/' . $webform_nid . '/edit') . ' | ' .
        l('remove', 'node/' . $node->nid . '/webforms/remove/' . $webform_nid)
    );
  }
  
  // If there are no webforms included, show an empty message
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No webforms have been added to this A/B test. Select a webform in the dropdown below and click the Add button.'), 'colspan' => count($headers)));
  }
  $return .= theme('table', $headers, $rows);
  
  // Show a form to add more
  $form = drupal_get_form('webform_ab_webform_list_add', $node);
  $return .= $form;
  
  return $return;
}

/**
 * Form to add webforms to the A/B Test
 * 
 * @param $form_state
 * @param $node   Node object
 */
function webform_ab_webform_list_add(&$form_state, $node) {
  // Get all valid forms for the form type from other modules
  $flat_forms = module_invoke_all('webform_ab_valid_webforms', $node->webform_types);
  
  // Since module_invoke_all() will wipe out numeric array indices, the
  // implementations of hook_webform_ab_valid_webforms() have to return the nid
  // and title within a sub-array, rather than using the nid as an array key.
  // Re-index the array to use the nid as the key and title as the value.
  $indexed_forms = array();
  foreach ($flat_forms as $f) {
    // Don't add a webform in the list if it's already included in the test
    if (!isset($node->included_webforms[$f['nid']])) {
      $indexed_forms[$f['nid']] = $f['title'];
    }
  }
  
  // Allow modules to alter the list of forms
  drupal_alter('webform_ab_valid_webforms', $indexed_forms, $node->webform_types);
  
  $form = array();

  $form['add_form_id'] = array(
    '#type' => 'select',
    '#title' => t('Add webform'),
    '#description' => t('Choose a webform to add to this test'),
    '#options' => array(0 => t('-- Select one --')) + $indexed_forms,
    '#default_value' => -1,
  );
  $form['test_nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );
  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  // If there are no more webforms that may be added, disable the form
  if (count($indexed_forms) == 0) {
    $form['add_form_id']['#disabled'] = TRUE;
    $form['add_form_id']['#description'] = 'There are no more webforms available to be added to this A/B test.';
    unset($form['add']);
  }

  return $form;
}


/**
 * Handle submission of the form to add a webform to an A/B Test
 */
function webform_ab_webform_list_add_submit($form, &$form_state) {
  // Only continue if they actually selected a webform
  if ($form_state['values']['add_form_id'] > 0) {
    // Make sure the chosen webform isn't already in the test
    // It wouldn't have been an option if it was already in the test, but be on the safe side
    $exists = db_result(db_query('SELECT COUNT(*) FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($form_state['values']['test_nid'], $form_state['values']['add_form_id']))) > 0;

    if (!$exists) {
      db_query('INSERT INTO {webform_ab_forms} (test_nid, webform_nid, status) VALUES(%d, %d, %d)', array($form_state['values']['test_nid'], $form_state['values']['add_form_id'], WEBFORM_AB_WEBFORM_STATUS_ACTIVE));
    }
  }
}


/**
 * Show a confirmation form when a user tries to remove a webform from a test
 */
function webform_ab_test_remove_webform($node, $webform_nid) {
  // Make sure the webform they want to remove is actually in the test
  $form_is_included = db_result(db_query('SELECT COUNT(*) FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($node->nid, $webform_nid))) > 0;
  
  if ($form_is_included) {
    // Show a confirmation form
    return drupal_get_form('webform_ab_test_remove_webform_confirm', $node->nid, $webform_nid);
  }
  else {
    // Somehow tried to remove a webform that isn't in the test
    drupal_set_message(t('That form is not in this A/B test.'), 'error');
    drupal_goto('node/' . $node->nid . '/webforms');
  }
}

/**
 * Confirmation form when removing a webform from a test
 */
function webform_ab_test_remove_webform_confirm(&$form_state, $test_nid, $webform_nid) {
  $form = array();
  $form['#test_nid'] = $test_nid;
  $form['#webform_nid'] = $webform_nid;
  $title = db_result(db_query('SELECT TITLE from {node} WHERE nid=%d', $webform_nid));
  return confirm_form(
    $form,
    t('Are you sure you want to remove webform %title from the A/B test?', array('%title' => $title)),
    'node/' . $test_nid . '/webforms',
    t('This action cannot be undone.'),
    t('Remove'),
    t('Cancel'),
    'webform_ab_test_remove_webform_confirm');
}


/**
 * Remove a webform from a test
 */
function webform_ab_test_remove_webform_confirm_submit($form, &$form_state) {
  db_query('DELETE FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($form['#test_nid'], $form['#webform_nid']));
  
  drupal_set_message(t('The webform has been removed.'));
  // Redirect back to the list of webforms
  $form_state['redirect'] = 'node/' . $form['#test_nid'] . '/webforms';
}


/**
 * Show a confirmation form when a user tries to clear data on a test
 */
function webform_ab_clear_test_data_confirm(&$form_state, $node) {
  $form = array();
  $form['#test_nid'] = $node->nid;
  return confirm_form(
    $form,
    t('Are you sure you want to clear all hit and conversion data on the test %title?', array('%title' => $node->title)),
    'node/' . $node->nid . '/reports',
    t("This action will remove all hit and conversion records saved for this test, return all included webforms to the Active status, and it cannot be undone. Note that any data submitted to the included webforms will remain intact, it just won't be associated with this A/B Test or used to declare a winning webform."),
    t('Remove'),
    t('Cancel'));
}



/**
 * Clear the data in the test. Removes hits, conversions, and returns all webforms to
 * the active status
 */
function webform_ab_clear_test_data_confirm_submit($form, &$form_state) {
  db_query('DELETE FROM {webform_ab_conversion} WHERE test_nid = %d', array($form['#test_nid']));
  db_query('DELETE FROM {webform_ab_hits} WHERE test_nid = %d', array($form['#test_nid']));
  db_query('UPDATE {webform_ab_forms} SET status = %d, win_time = %d WHERE test_nid = %d', array(WEBFORM_AB_WEBFORM_STATUS_ACTIVE, 0, $form['#test_nid']));
  
  drupal_set_message(t('The data has been cleared.'));

  $form_state['redirect'] = 'node/' . $form['#test_nid'] . '/reports';
}