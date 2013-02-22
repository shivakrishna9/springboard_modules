<?php

/**
 * @file
 * Definition of salesforce_log_handler_field_sobject.
 */

/**
 * Filter by sObject.
 *
 * @ingroup views_filter_handlers
 */
class salesforce_log_handler_field_sobject extends views_handler_field {

  /**
   * Render sObject.
   */
  public function render($values) {
    $value = $values->{$this->field_alias};

    return '<pre>' . check_plain(print_r(json_decode($value), TRUE)) . '</pre>';
  }
}
