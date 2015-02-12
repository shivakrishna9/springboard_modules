<?php

/**
 * @file springboard_advocacy_views_handler_field.inc
 *
 */

class springboard_advocacy_views_handler_field extends views_handler_field {

  /**
   * Override the parent method
   * We aren't using a DB, so we won't have to lookup aliases
   */ 
  function query() {

    $this->field_alias = $this->real_field;
  }
}

function click_sort($order) {
  if (isset($this->field_alias)) {

    $params = $this->options['group_type'] != 'group' ? array('function' => $this->options['group_type']) : array();
    $this->query->add_orderby(NULL, NULL, $order, $this->field_alias, $params);
  }
}