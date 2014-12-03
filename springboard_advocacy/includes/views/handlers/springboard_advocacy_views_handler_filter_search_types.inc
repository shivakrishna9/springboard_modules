<?php

class springboard_advocacy_views_handler_filter_search_types extends views_handler_filter_in_operator {

  function get_value_options() {
    if (isset($this->value_options)) {
      return;
    }

    $this->value_options = array(
      'custom' => t('Custom Target'), 
      'legislator' => t('Legislator'), 
      'group' => t('Group')
      );

    return $this->value_options;
  }

  function query() {

     if (is_array($this->value)) {
      $keys = array_keys($this->value);
      $types = implode(' ', $keys);
     }
     else {
      $types = $this->value;
     }

     $this->query->search_terms['type'] = $types;
    }
  }
