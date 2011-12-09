<?php
/**
 * Admin functions for Capwiz Connect integration
 */


/**
 * Amdin list of Capwiz Connect accounts
 */
function capwiz_connect_account_list() {
  $table_headers = array(
    'Name',
    'Username',
    'Operations',
  );

  $accounts = array();
  
  $result = db_query('SELECT * FROM {capwiz_connect_accounts} ORDER BY display_name');
  while ($row = db_fetch_object($result)) {
    $operations = array(
      l('edit', 'admin/settings/capwiz_connect/edit/' . $row->id),
      l('user mapping', 'admin/settings/capwiz_connect/user_mapping/' . $row->id),
      l('action mapping', 'admin/settings/capwiz_connect/action_mapping/' . $row->id),
      l('delete', 'admin/settings/capwiz_connect/delete/' . $row->id),
    );
    
    $accounts[] = array(
      $row->display_name,
      $row->user_name,
      implode(' | ', $operations),
    );
  }
  
  if (empty($accounts)) {
    $accounts[] = array(
      array(
        'data' => t('No Capwiz Connect accounts have been added.'),
        'colspan' => count($table_headers),
      ),
    );
  }
  
  return theme_table($table_headers, $accounts);
}


/**
 * Form to add or edit a Capwiz Connect account
 */
function capwiz_connect_account_addoredit($form_state, $account = NULL) {
  $form = array();
  $form['#redirect'] = 'admin/settings/capwiz_connect';
  if (is_object($account)) {
    $form['account_id'] = array(
      '#type' => 'value',
      '#value' => $account->id,
    );
    $account_details = (array)$account;
  }
  
  $form['display_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Name'),
    '#description' => t('Name to show for this account. This is only used to differentiate accounts for admins.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['display_name'],
  );
  $form['user_name'] = array(
    '#type' => 'textfield',
    '#title' => t('User Name'),
    '#description' => t('API username for the Capwiz Connect API.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['user_name'],
  );
  $form['user_password'] = array(
    '#type' => 'textfield',
    '#title' => t('User Password'),
    '#description' => t('API password for the Capwiz Connect API.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $account_details['user_password'],
  );
  
  
  /**
   * How to store users. For now, the only option is to store users into the
   * capwiz_connect_users table. Later, we will add an option to create Drupal
   * users from Capwiz users.
   * For now, use the only option available.
   */
  $form['store_users_as'] = array(
    '#type' => 'value',
    '#value' => 'db'
  );
  
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );  
  
  return $form;
}


/**
 * Submit the add/edit form
 */
function capwiz_connect_account_addoredit_submit($form, &$form_state) {
  $values = $form_state['values'];
  $record = array(
    'display_name' => $values['display_name'],
    'user_name' => $values['user_name'],
    'user_password' => $values['user_password'],
    'id' => $values['account_id'],
  );
  
  // Determine if inserting or updating
  if ($values['account_id'] > 0) {
    $save_ok = drupal_write_record('capwiz_connect_accounts', $record, 'id');
    if ($save_ok) {
      drupal_set_message(t('Account updated'));
    }
  }
  else {
    $save_ok = drupal_write_record('capwiz_connect_accounts', $record);
    if ($save_ok) {
      drupal_set_message(t('New Account created'));
    }
  }
  
  return $save_ok;
}


/**
 * User field mapping for Capwiz Connect account
 */
function capwiz_connect_account_user_mapping($form_state, $account) {
  $form = array();
  
  $form['user-mapping-form'] = array(
    '#value' => 'user mapping form',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  
  
  return $form;
}


/**
 * Action field mapping for Capwiz Connect account
 */
function capwiz_connect_account_action_mapping($form_state, $account) {
  $form = array();
  
  $form['action-mapping-form'] = array(
    '#value' => 'action mapping form',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  
  
  return $form;
}



/**
 * Confirm account delete
 */
function capwiz_connect_account_delete($form_state, $account) {
  if ($account->id > 0) {
    $form = array(
      '#account_id' => $account->id,
    );
    
    return confirm_form(
      $form,
      t('Are you sure you want to delete the account %display_name?', array('%display_name' => $account->display_name)),
      'admin/settings/capwiz_connect/accounts',
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel'),
      'capwiz_connect_account_delete');
  }
  else {
    drupal_set_message(t('That account does not exist.'), 'error');
    drupal_goto('admin/settings/capwiz_connect/accounts');
  }
}


/**
 * After confirming an account delete, actually delete it
 */
function capwiz_connect_account_delete_submit($form, &$form_state) {
  $account_id = $form['#account_id'];
  db_query('DELETE FROM {capwiz_connect_accounts} WHERE id = %d', $account_id);
  drupal_set_message(t('The account has been deleted.'));
  $form_state['redirect'] = 'admin/settings/capwiz_connect/accounts';
}