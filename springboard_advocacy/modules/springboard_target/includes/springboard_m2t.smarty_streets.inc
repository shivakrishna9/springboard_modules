<?php

/**
 * @file
 * Smarty Streets functions for Springboard M2T.
 */

function _springboard_mt2_smarty_lookup($values) {
  
  $responses = array();
  $query =  array(
    'street' => $values['sbp_address'],
    'city' => $values['sbp_city'],
    'state' => $values['sbp_state'],
    'zipcode' => $values['sbp_zip'],
    'auth-id' => variable_get('springboard_m2t_smarty_authid',''),
    'auth-token' => variable_get('springboard_m2t_smarty_authtoken',''),
  );

  $url = url('https://api.smartystreets.com/street-address', array('query' => $query));

  $response = drupal_http_request($url);

  if (!empty($response->data)) {

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
         return $geo['zip'] . '-' . $geo['plus4'];
      }
    }
  }
}