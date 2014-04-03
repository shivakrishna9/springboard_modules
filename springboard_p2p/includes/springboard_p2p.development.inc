<?php
/**
 * @file
 * Development and debugging things. @todo Remove me eventually.
 */

function springboard_p2p_development_settings($form, &$form_state) {
  $form = array();

  $form['approval'] = array(
    '#type' => 'fieldset',
    '#title' => 'Private campaign approvals',
  );

  $form['approval']['add'] = array();

  foreach (array('uid', 'nid') as $key) {
    $form['approval']['add'][$key] = array(
      '#type' => 'textfield',
      '#title' => $key,
    );
  }

  $form['approval']['add']['status'] = array(
    '#type' => 'select',
    '#title' => 'Status',
    '#options' => array(
      'request' => 'Requested',
      'approve' => 'Approved',
      'reject' => 'Rejected',
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  return $form;
}

function springboard_p2p_development_settings_submit(&$form, &$form_state) {
  if (isset($form_state['values']['uid'])) {
    $approval = new SpringbaordP2pPrivateCampaignApproval($form_state['values']['uid'], $form_state['values']['nid']);
    $approval->{$form_state['values']['status']}();

    drupal_set_message('Added');
  }
}
