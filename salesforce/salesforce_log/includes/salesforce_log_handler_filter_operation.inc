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
class salesforce_log_handler_filter_operation extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_options = array(
        'CREATE' => 'Create',
        'UPDATE' => 'Update',
        'UPSERT' => 'Upsert',
      );
    }
  }
}
