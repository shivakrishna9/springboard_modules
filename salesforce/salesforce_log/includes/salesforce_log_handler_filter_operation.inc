<?php

/**
 * @file
 * Definition of salesforce_log_handler_filter_operation.
 */

/**
 * Filter by Salesforce operation.
 *
 * @ingroup views_filter_handlers
 */
class salesforce_log_handler_filter_operation extends views_handler_filter_in_operator {

  /**
   * List of possible Salesforce operations.
   */
  public function get_value_options() {
    $this->value_options = array(
      'CREATE' => 'Create',
      'UPDATE' => 'Update',
      'UPSERT' => 'Upsert',
    );
  }
}
