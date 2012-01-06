<?php

/**
 * Filter by Drupal type in salesforce_management_object_map table.
 */
class fundraiser_handler_filter_drupal_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Drupal type');
      $drupal_types = array();
      $result = db_query("SELECT DISTINCT drupal_type FROM {salesforce_management_object_map}");
      while ($drupal_type = db_result($result)) {
        $drupal_types[$drupal_type] = $drupal_type;
      }
      $this->value_options = $drupal_types;
    }
  }
}
