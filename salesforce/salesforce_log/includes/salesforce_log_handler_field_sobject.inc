<?php

/**
 * @file
 * Definition of SalesforceLogHandlerFieldSobject.
 */

/**
 * Filter by sObject.
 *
 * @ingroup views_filter_handlers
 */
class SalesforceLogHandlerFieldSobject extends views_handler_field {

  /**
   * Render sObject.
   */
  public function render($values) {
    $value = $values->{$this->field_alias};

    return '<pre>' . check_plain(print_r(json_decode($value), TRUE)) . '</pre>';
  }
}
