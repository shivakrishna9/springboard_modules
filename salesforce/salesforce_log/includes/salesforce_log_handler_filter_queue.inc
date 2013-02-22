<?php

/**
 * @file
 * Definition of salesforce_log_handler_filter_queue.
 */

/**
 * Filter by queue.
 *
 * @ingroup views_filter_handlers
 */
class salesforce_log_handler_filter_queue extends views_handler_filter_in_operator {

  /**
   * List of possible queues.
   */
  public function get_value_options() {
    $this->value_options = salesforce_queue_get_queue_names();
  }
}
