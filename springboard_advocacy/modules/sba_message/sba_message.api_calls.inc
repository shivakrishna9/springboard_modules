<?php

/**
 * @param $target_fields
 * @return array
 */
function _springboard_target_api_get_gender_or_social_ids($target_fields) {

  $api_call = springboard_advocacy_api_call();

  $ids = array();
  foreach ($target_fields as $fields) {
    $fields->limit = 1;
    $response = $api_call->invokeClientMethod('searchTargets', (array) $fields);

    if (!empty($response->data->ids)) {
      $ids = array_unique(array_merge($ids, $response->data->ids));
    }
  }
  return $ids;
}

/**
 * Take the chambers form input and format it for the API callback.
 * @param $unstructured_chambers
 * @return array
 */
function _springboard_target_api_build_chambers($unstructured_chambers) {
  $chamber_rows_container = array();

  // An "Unstructured chambers" example:
  //    0 (Object)
  //        role => FR
  //        party => D
  //    1 (Object)
  //        role => PRES01|PRES03|GOVNR
  //    2 (Object)
  //        state => NY
  //        role => SR|SS


  //Unstructured chambers above will become:
  //0 =>
  //    chamber =>FR
  //    party  => D
  //1 =>
  //    chamber => PRES01
  //    party =>
  //2 =>
  //    chamber => PRES03
  //    party =>
  //3 =>
  //    chamber => GOVNR
  //    party =>
  //4 =>
  //    chamber => SR
  //    party =>
  //    state => NY
  //5 =>
  //    chamber => SS
  //    party =>
  //    state => NY

  foreach ($unstructured_chambers as $chamber_id => $chamber_fields) {

    //create a new container to rewrap our chambers
    $chamber_rows_container[$chamber_id] = array();

    foreach ($chamber_fields as $field_name => $chamber_field_values) {
      $chamber_field_values = explode('|', $chamber_field_values);
      if ($field_name == 'role') {
        $field_name = 'chamber';
      }

      $i = 0;
      $num = count($chamber_rows_container[$chamber_id]);

      if ($num) {
        foreach ($chamber_rows_container[$chamber_id] as $chamber_field) {
          foreach ($chamber_field_values as $value) {
            _springboard_target_api_build_chambers_shmoosher($chamber_rows_container, $chamber_id, $i, $field_name, $value, $chamber_field);
            $i++;
          }
        }
      } else {
        foreach ($chamber_field_values as $value) {
          _springboard_target_api_build_chambers_shmoosher($chamber_rows_container, $chamber_id, $i, $field_name, $value);
          $i++;
        }
      }
    }
  }

  $chamber_rows = array();
  foreach ($chamber_rows_container as $container)  {
    $chamber_rows = array_merge($chamber_rows, $container);
  }
  return $chamber_rows;
}

/**
 * Helper function to build chamber rows
 * @param $chamber_rows_container
 * @param $chamber_id
 * @param $i
 * @param $field_name
 * @param $value
 * @param array $chamber_field
 */
function _springboard_target_api_build_chambers_shmoosher(&$chamber_rows_container, $chamber_id, $i, $field_name, $value, $chamber_field = array()) {

  // the container is passed by reference
  // make sure that values that are already set, get maintained.
  $chamber = isset($chamber_field['chamber']) ? $chamber_field['chamber'] : '';
  $party = isset($chamber_field['party']) ? $chamber_field['party'] : '';
  $state = isset($chamber_field['state']) ? $chamber_field['state'] : '';
  //$gender = isset($chamber_field['gender']) ? $chamber_field['gender'] : '';

  $chamber_rows_container[$chamber_id][$i]['chamber'] = $chamber;
  $chamber_rows_container[$chamber_id][$i]['party'] = $party;
  $chamber_rows_container[$chamber_id][$i]['state'] = $state;
  //$chamber_rows_container[$chamber_id][$i]['gender'] = $gender;

  $chamber_rows_container[$chamber_id][$i][$field_name] = $value;
}

