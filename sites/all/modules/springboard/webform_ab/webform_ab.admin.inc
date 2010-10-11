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
  foreach ($node->included_forms as $form_nid => $form_details) {
    $rows[] = array($form_details['title'], $form_details['status'], l('edit', 'node/' . $form_nid . '/edit'));
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
    '#title' => t('Add form'),
    '#description' => t('Choose a form to add to this test'),
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




function webform_ab_add_form_to_test($test_nid, $form_nid) {
  $exists = db_result(db_query('SELECT COUNT(*) FROM {webform_ab_forms} WHERE test_nid = %d AND webform_nid = %d', array($test_nid, $form_nid))) > 0;
  
  if (!$exists) {
    db_query('INSERT INTO {webform_ab_forms} (test_nid, webform_nid) VALUES(%d, %d)', array($test_nid, $form_nid));
  }
}