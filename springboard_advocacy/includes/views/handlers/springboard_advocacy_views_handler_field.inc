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

