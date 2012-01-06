<?php

/**
 * Filter by Drupal type in salesforce_management_object_map table.
 */
class fundraiser_handler_filter_gateway_resp extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Gateway response');
      $gateway_responses = array();
      $result = db_query("SELECT DISTINCT gateway_resp FROM {fundraiser_recurring}");
      while ($gateway_response = db_result($result)) {
        $gateway_responses[$gateway_response] = $gateway_response;
      }
      $this->value_options = $gateway_responses;
    }
  }
}
