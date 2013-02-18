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
class salesforce_log_handler_filter_object_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $result = db_query("SELECT distinct(object_type) FROM " . $this->view->base_table);
      foreach ($result as $record) {
        $this->value_options[$record->object_type] = $record->object_type;
      }
    }
  }
}
