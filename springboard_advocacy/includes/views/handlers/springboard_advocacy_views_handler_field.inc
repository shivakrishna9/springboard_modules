<?php

/**
 * @file springboard_advocacy_views_handler_field.inc
 */

class springboard_advocacy_views_handler_field extends views_handler_field {
  function query() {
    $this->field_alias = $this->real_field;
  }
}

