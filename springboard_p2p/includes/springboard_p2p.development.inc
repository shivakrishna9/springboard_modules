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

  $options = array();
  $users = springboard_p2p_get_users();
  foreach ($users as $user) {
    $options[$user->uid] = $user->name;
  }

  $form['approval']['add']['uid'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#title' => 'User',
  );

  $options = array();
  $private_campaigns = springboard_p2p_get_private_campaigns();
  foreach ($private_campaigns as $campaign) {
    $options[$campaign->nid] = $campaign->title;
  }

  $form['approval']['add']['nid'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#title' => 'Private campaign',
  );

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

function springboard_p2p_get_users() {
  $users = array();

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'user')
    ->propertyOrderBy('name', 'ASC');

  $result = $query->execute();

  if (isset($result['user'])) {
    $uids = array_keys($result['user']);
    $users = entity_load('user', $uids);
  }

  return $users;
}
