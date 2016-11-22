<?php

/**
 * @file
 * Provide a handler for Springboard content type groupings.
 */

class springboard_views_handler_node_springboard_type extends views_handler_filter_in_operator  {

  function operators() {
    $operators = parent::operators();
    unset($operators['empty']);
    unset($operators['not empty']);
    return $operators;
  }

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Springboard Content types');
      $groups = springboard_get_springboard_content_groups();
      $options = array();
      foreach ($groups as $group => $info) {
        $options[$group] = t($info['#name']);
      }
      asort($options);
      $this->value_options = $options;
    }
  }

  function springboard_types() {
    $groups = $this->value;
    $types = springboard_get_springboard_content_types($groups);

    return $types;
  }

  function query() {
    $springboard_types = $this->springboard_types();
    $this->real_field = 'type';
    //$value = $this->value['value'];

    $this->query->add_where($this->options['group'], "$this->table.$this->real_field", $springboard_types, 'IN');
  }
}