<?php

/**
 * Page callback for admin/settings/springboard_api.
 */
function springboard_api_admin() {
  $headers = array(
    t('App Name'),
    t('User'),
    t('Public Key'),
    t('Private Key'),
    t('Links'),
  );
  $results = db_query('SELECT s.kid, s.uid, s.name, u.name as username, s.pubkey, s.privkey
   FROM {springboard_api} s
   INNER JOIN {users} u
     ON u.uid = s.uid');
  while ($keyset = db_fetch_object($results)) {
    $row = array();
    $row[] = $keyset->name;
    $row[] = $keyset->username ? l($keyset->username, 'user/' . $keyset->uid) : t('Anonymous');
    $row[] = $keyset->pubkey;
    $row[] = $keyset->privkey;
    $row[] = _springboard_api_links($keyset->kid);
    $rows[] = $row;
  }
  
  $users = _springboard_api_get_users();
  
  $form['api_keysets'] = array(
    '#type' => 'fieldset',
    '#title' => t('API key sets'),
    '#description' => t('The following key sets are valid for this site.'),
  );
  $form['api_keysets']['existing'] = array(
    '#value' => theme_table($headers, $rows),
  );
  $form['new'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add an application'),
    '#description' => t('Create a key set associated with an application. Each application should have unique keysets.'),
  );
  $form['new']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('App Name'),
    "#maxlength" => 255,
    '#size' => 50,
    '#description' => t(''),
  );
  $form['new']['user'] = array(
    '#type' => 'select',
    '#title' => t('User'),
    '#description' => t('Select a user account to associate with this app. Service calls made by this app will use the permissionsa assigned to this account for purposes of access checking.'),
    '#options' => $users,
  );
  $form['new']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add app'),
  );
  $form['#validate'][] = 'springboard_api_admin_validate';
  $form['#submit'][] = 'springboard_api_admin_submit';
  return $form;
}

function springboard_api_admin_validate(&$form, $form_state) {

  if (!$form_state['values']['name']) {
    form_set_error('name', t('The name field is required.'));
  }
}

function springboard_api_admin_submit($form, $form_state) {
  $pubkey = md5(user_password());
  $privkey = sha1(user_password());
  db_query("INSERT INTO {springboard_api} (kid, uid, name, pubkey, privkey) VALUES (NULL, %d, '%s', '%s', '%s')", $form_state['values']['user'], $form_state['values']['name'], $pubkey, $privkey);
  // save.
}


function springboard_api_edit_keyset($form, $edit) {
  $users = _springboard_api_get_users();
  $form = array();
  $form['kid'] = array(
    '#type' => 'hidden',
    '#value' => $edit->kid,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('App Name'),
    '#size' => 50,
    '#maxlength' => 255,
    '#default_value' => $edit->name,
  );
  $form['new']['user'] = array(
    '#type' => 'select',
    '#title' => t('User'),
    '#description' => t('Select a user account to associate with this app. Service calls made by this app will use the permissionsa assigned to this account for purposes of access checking.'),
    '#options' => $users,
    '#default_value' => $edit->uid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#validate'][] = 'springboard_api_admin_validate';
  $form['#submit'][] = 'springboard_api_edit_submit';
  return $form;
}

function springboard_api_edit_submit($form, $form_state) {
  db_query("UPDATE {springboard_api} SET uid = %d, name = '%s' WHERE kid = %d", $form_state['values']['user'], $form_state['values']['name'], $form_state['values']['kid']);
  drupal_set_message('Settings saved.');
  drupal_goto('admin/settings/springboard_api');
}

function springboard_api_delete_keyset($form, $edit) {
  if (isset($form['post']['op']) && $form['post']['op'] == t('Cancel')) {
    drupal_goto("admin/settings/springboard_api");
  }
  $form = array();
  $form['warning'] = array(
    '#value' => '<p>' . t('Warning: this action cannot be undone. Any appications currently using this key set will no longer be able to access the API. Are you sure you want to delete this?') . '</p>',
  );
  $form['kid'] = array(
    '#type' => 'hidden',
    "#value" => $edit->kid,
  );
  $form['cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['#submit'][] = 'springboard_api_delete_submit';
  return $form;
}

/**
 * 
 */
function springboard_api_delete_submit($form, $form_state) {
  global $user;
  $name = db_result(db_query('SELECT name FROM {springboard_api} WHERE kid = %d', $form_state['values']['kid']));
  db_query('DELETE FROM {springboard_api} WHERE kid=%d', $form_state['values']['kid']);
  drupal_set_message(t('%name key set deleted.', array("%name" => $name)));
  watchdog('springboard_api', t('%name key set deleted by !user.', array('%name' => $name, '!user' => l($user->name, 'user/' . $user->uid))));
  drupal_goto('admin/settings/springboard_api');
}

/**
 * 
 */
function springboard_api_test_client($form = array()) {

  $form = array();
  $form['url'] =  array(
    '#type' => 'textfield',
    '#title' => t('Service URL'),
  );
  $form['resource'] = array(
    '#type' => 'select',
    '#title' => t('Service resource'),
    '#options' => array('index' => 'index', 'retrieve' => 'retrieve', 'submit' => 'submit'),
  );
  $form['method'] = array(
    '#type' => 'select',
    '#title' => t('Method'),
    '#options' => array('GET' => 'GET', 'POST' => 'POST'),
  );
  $form['pubkey'] = array(
    '#type' => 'textfield',
    '#title' => 'Public Key',
  );
  $form['privkey'] = array(
    '#type' => 'textfield',
    '#title' => 'Private Key',
  );
  $form['data'] = array(
    '#type' => 'textarea',
    '#title' => t('data'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Run test'),
  );
  $form['#submit'][] = 'springboard_api_test_client_submit';
  return $form;
}

function springboard_api_test_client_validate(&$form, $form_state) {

}

function springboard_api_test_client_submit(&$form, $form_state) {
  if (function_exists('curl_init')) {
    $url = $form_state['values']['url'];
    $data = _springboard_api_pack_test_data($form_state['values']['pubkey'], $form_state['values']['privkey'], $form_state['values']['data'], $form_state['values']['resource']);
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
    if ($form_state['values']['method'] == 'POST') {
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    }
    else {
      $url .= '&key=' . $data['pubkey'] . '&time=' . $data['timestamp'] . '&checksum=' . $data['checksum'];
    }
    curl_setopt($ch, CURLOPT_URL,$url);
    $result['contents']=curl_exec($ch);
    $result['info']=curl_getinfo($ch);
    drupal_set_message('result:' . print_r($result, TRUE));
    curl_close($ch);
  }
}

function _springboard_api_pack_test_data($pubkey, $privkey, $data, $resource) {

  $data['resource'] = $resource;
  $data['timestamp'] = (int) time();
  $data['pubkey'] = trim($pubkey);
  $data['privkey'] = trim($privkey);
  $data['checksum'] = sha1(json_encode($data));

  unset($data['privkey']);
  return $data;
}

function _springboard_api_get_users() {
  $users = array('0' => 'Anonymous');
  $results = db_query('SELECT uid, name FROM {users} where uid > 1 order by uid');
  while ($user = db_fetch_object($results)) {
    $users[$user->uid] = $user->name;
  }
  return $users;
}