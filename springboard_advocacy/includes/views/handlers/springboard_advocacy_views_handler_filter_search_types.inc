<?php

class springboard_advocacy_views_handler_filter_search_types extends views_handler_filter_in_operator {

  // bug workaround: https://www.drupal.org/node/2096447
  // can be removed after springboard upgrades to views 7.x-3.8
  // which should be before this module is released
  // not sure what unintended side effects this may have.
  function init(&$view, &$options) {
    if (!isset($options['group_info']['default_group_multiple'])) {
      if (isset($this->options['group_info']['default_group_multiple'])) {
        $options['group_info']['default_group_multiple'] = $this->options['group_info']['default_group_multiple'];
      }
      else {
        $options['group_info']['default_group_multiple'] = array('default' => array());
      }
    }
    parent::init($view, $options);
  }


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
