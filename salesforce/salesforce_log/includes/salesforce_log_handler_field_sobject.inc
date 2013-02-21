<?php

/**
 * @file
 * Definition of views_handler_filter_node_type.
 */ 

/**
 * Filter by node type.
 *
 * @ingroup views_filter_handlers
 */
class salesforce_log_handler_field_sobject extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};

    return '<pre>' . check_plain(print_r(json_decode($value), TRUE)) . '</pre>';
  }
}
