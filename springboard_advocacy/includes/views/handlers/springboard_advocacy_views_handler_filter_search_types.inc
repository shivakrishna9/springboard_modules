<?php

class springboard_advocacy_views_handler_filter_search_types extends views_handler_filter_in_operator {

  function query() {
   if (is_array($this->value)) {

    foreach($this->value as $key => $value) {
      if (empty($value)) {
        unset($this->value[$key]);
      }
    }
    $values = array_values($this->value);
    $values = implode('|', $values);
   }
   else {
      $values = $this->value;
   }
   $field = str_replace('search_', '', $this->real_field);
   $this->query->search_terms[$field][] = $values;
  } 
}
