<?php

/**
 * @file
 * Definition of SalesforceLogHandlerFilterModule.
 */

/**
 * Filter by module.
 *
 * @ingroup views_filter_handlers
 */
class fundraiser_handler_filter_fundraiser_gateways extends views_handler_filter_string {

  /**
   * List of possible modules.
   */
  public function get_value_options() {
   $this->value_options = array(0 => 'gateways');
  }
  
  public function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);
    dpm($form);
  }
}
