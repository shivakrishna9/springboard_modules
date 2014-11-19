<?php

/**
 * @file
 * Smarty Streets functions for Springboard M2T.
 */

/**
 * Ajax callback for smarty streets api
 */
function _springboard_mt2_smarty_lookup() {
  
  //uncomment to test lookups via menu callback
  // $geo['zip'] = '12866';
  // $geo['plus4'] = '3300';
  // springboard_m2t_get_legislators($geo['zip'], $geo['plus4']);
  // exit();

  $responses = array();

  $address = isset($_POST['address']) ? $_POST['address'] : NULL;
  if (empty($address)) {
    return drupal_json_output(array('error' => 'address not supplied'));
  }

  $query =  array(
    //'street' => '49 Hall Rd, Canaan, NH 03741',
    'street' => urlencode($address['submitted[sbp_address']),
    'city' => urlencode($address['submitted[sbp_city']),
    'state' => urlencode($address['submitted[sbp_state']),
    'zipcode' => urlencode($address['submitted[sbp_zip']),
    'auth-id' => variable_get('springboard_m2t_smarty_authid',''),
    'auth-token' => variable_get('springboard_m2t_smarty_authtoken',''),
  );

  $url = url('https://api.smartystreets.com/street-address', array('query' => $query));

  $response = drupal_http_request($url);

  if (!isset($response->status_message) || $response->status_message != 'OK') {
    return drupal_json_output(array('error' => 'Server returned invalid response'));
  }

  if (empty($response->data)) {

    $data = json_decode($response->data);

    if(!empty($data)) {
      $geo = array();
      $geo['zip'] = isset($data[0]->components->zipcode) ? $data[0]->components->zipcode : '';
      $geo['plus4'] = isset($data[0]->components->plus4_code) ? $data[0]->components->plus4_code : '';
      //$district = $data[0]->metadata->congressional_district;
      $geo['latitude'] = isset($data[0]->metadata->latitude) ? $data[0]->metadata->latitude : '';
      $geo['longitude'] = isset($data[0]->metadata->longitude) ? $data[0]->metadata->longitude : '';
      $_SESSION['m2t_geo'] = $geo;
      if(!empty($geo['zip']) && !empty($geo['plus4'])) {
        springboard_m2t_get_legislators($geo['zip'], $geo['plus4']);
      }
    }
  }
}