<?php

class springboard_advocacy_views_handler_filter_string extends views_handler_filter_string {

 function operators() {
    $operators = array(
      'contains' => array(
        'title' => t('Contains'),
        'short' => t('contains'),
        'method' => 'op_contains',
        'values' => 1,
      ),
    );

    return $operators;
  }

  function operator_options($which = 'title') {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }
    return $options;
  }

  function query() {
    $field = $this->real_field;
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }

  function op_contains($field) {
    $this->query->search_terms[$field] = $this->value;
  }
}
