<?php
// $Id$

/**
 * @file
 * Webform A/B Testing admin functions
 */


function webform_ab_test_reports($node) {
  return 'Reporting for ' . $node->title;
}


function webform_ab_test_forms($node) {
  $return = '';
  
  // Show the list of currently-included forms
  $headers = array(t('Name'), t('Status'), t('Operations'));
  $rows = array();
  foreach ($node->included_forms as $webform_nid => $form_details) {
    $rows[] = array(
      $form_details['title'], 
      $form_details['status'], 
      l('edit', 'node/' . $webform_nid . '/edit') . ' | ' .
        l('remove', 'node/' . $node->nid . '/forms/remove/' . $webform_nid)
    );
  }
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No webforms have been added to this A/B test. Select a webform in the dropdown below and click the Add button.'), 'colspan' => count($headers)));
  }
  $return .= theme('table', $headers, $rows);
  
  // Show a form to add more
  $form = drupal_get_form('webform_ab_form_list_add', $node);
  $return .= $form;
  
  return $return;
}



function webform_ab_form_list_add(&$form_state, $node) {
  // Get all valid forms for the form type from other modules
  $flat_forms = module_invoke_all('webform_ab_valid_forms', $node->form_types);
  drupal_alter('webform_ab_valid_forms', $flat_forms);

  // Since module_invoke_all() will wipe out numeric array indices, the
  // implementations of hook_webform_ab_valid_forms() have to return the nid and
  // title within a sub-array, rather than using the nid as an array key.
  // Re-index the array to use the nid as the key and title as the value.
  $indexed_forms = array();
  foreach ($flat_forms as $f) {
    $indexed_forms[$f['nid']] = $f['title'];
  }


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
  return $form;
}


function webform_ab_form_list_add_submit($form, &$form_state) {
  if ($form_state['values']['add_form_id'] > 0) {
    webform_ab_add_form_to_test($form_state['values']['test_nid'], $form_state['values']['add_form_id']);
  }
}




function webform_ab_add_form_to_test($test_nid, $webform_nid) {
  $exists = db_result(db_query('SELECT COUNT(*) FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($test_nid, $webform_nid))) > 0;
  
  if (!$exists) {
    db_query('INSERT INTO {webform_ab_forms} (test_nid, webform_nid) VALUES(%d, %d)', array($test_nid, $webform_nid));
  }
}




function webform_ab_test_remove_form($node, $webform_nid) {
  $form_is_included = db_result(db_query('SELECT COUNT(*) FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($node->nid, $webform_nid))) > 0;
  
  if ($form_is_included) {
    return drupal_get_form('webform_ab_test_remove_form_confirm', $node->nid, $webform_nid);
  }
  else {
    drupal_set_message(t('That form is not in this A/B test.'), 'error');
    drupal_goto('node/' . $node->nid . '/forms');
  }
}


function webform_ab_test_remove_form_confirm(&$form_state, $test_nid, $webform_nid) {
  $form = array();
  $form['#test_nid'] = $test_nid;
  $form['#webform_nid'] = $webform_nid;
  $title = db_result(db_query('SELECT TITLE from {node} WHERE nid=%d', $webform_nid));
  return confirm_form(
    $form,
    t('Are you sure you want to remove webform %title from the A/B test?', array('%title' => $title)),
    'node/' . $test_nid . '/forms',
    t('This action cannot be undone.'),
    t('Remove'),
    t('Cancel'),
    'webform_ab_test_remove_form_confirm');
}

function webform_ab_test_remove_form_confirm_submit($form, &$form_state) {
  drupal_set_message(t('The webform has been removed.'));

  db_query('DELETE FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($form['#test_nid'], $form['#webform_nid']));

  $form_state['redirect'] = 'node/' . $form['#test_nid'] . '/forms';
}