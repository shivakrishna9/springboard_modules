<?php

/**
* @file
* Callback functions for JSON input/output by the Capwiz Connect JS module
*/

/**
 * Callback for the decryption of the form values
 *
 * Uses the secure prepopulate module to decrypt a string containing personal address information
 *
 * @return
 *    JSON output containing the unecnrypted personal address information
 */
function capwiz_connect_js_decrypt() {
  $key = variable_get('secure_prepopulate_key', '') ? decrypt(variable_get('secure_prepopulate_key', '')) : '';
  $iv = variable_get('secure_prepopulate_iv', '') ? decrypt(variable_get('secure_prepopulate_iv', '')) : '';
  $fields = array();

  $af = $_GET['af'];
  // first, check to see if the query string has already been used
  if (!secure_prepopulate_is_expired($af)) {
    $qs = _secure_prepopulate_decrypt($af, $key, $iv);
    // expire the query string
    secure_prepopulate_expire($af);

    $segments = explode('&', $qs);

    // loop over each field in the segement and extract the key/value pairs
    if (count($segments)) {
      foreach ($segments as $segment) {
        $key_value = explode('=', $segment);
        $fields[$key_value[0]] = $key_value[1];
      }
    }
  }

  $a = array('fieldValues' => $fields);

  header('Content-Type: text/javascript; charset=utf8');
  header('Access-Control-Allow-Origin: *');
  header('Access-Control-Max-Age: 3628800');
  header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
  header("Access-Control-Allow-Headers: X-Requested-With");
  print $_GET['callback'] . '(' . json_encode($a) . ')';
  module_invoke('exit');
  exit;
}

/**
 * Callback for the confirmation page
 *
 * @return
 *    JSON output containing the redirect page
 */
function capwiz_connect_js_confirmation() {
  $data = json_decode(base64_decode($_GET['data']), TRUE);
  $return['redirect'] = 0;
  // Load the alert from the public alert id
  $alert = db_query('SELECT * FROM {capwiz_connect_js_alerts} WHERE capwiz_alert_id = :id', array(':id' => $data['fieldValues']['MC_alertid']))->fetchObject();
  if (!empty($alert)) {
    // Save the action
    capwiz_connect_js_save_action($data, $alert);
    // Create the redirect
    if (!empty($alert->redirect)) {
      $return['redirect'] = url($alert->redirect, array('absolute' => TRUE));
    }
  }
  header('Content-Type: text/javascript; charset=utf8');
  header('Access-Control-Allow-Origin: *');
  header('Access-Control-Max-Age: 3628800');
  header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE');
  header("Access-Control-Allow-Headers: X-Requested-With");
  print $_GET['callback'] . '(' . json_encode($return) . ')';
  module_invoke('exit');
  exit;
}
