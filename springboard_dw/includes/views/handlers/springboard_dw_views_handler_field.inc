<?php

/**
 * @file springboard_dw_views_handler_field.inc
 *
 */

class springboard_dw_views_handler_field extends views_handler_field
{
  /**
   * Override the parent method
   * We aren't using a DB, so we won't have to lookup aliases
   */
  function query()
  {
    $this->field_alias = $this->real_field;
  }

  /**
   * Make target search results click sortable
   * @param $order
   */
  function click_sort($order)
  {
    if (isset($this->field_alias)) {
      $params = $this->options['group_type'] != 'group' ? array('function' => $this->options['group_type']) : array();
      $this->query->add_orderby(NULL, NULL, $order, $this->field_alias, $params);
    }
  }

  /**
   * Override parent get_value.
   * @param $values
   * @param null $field
   * @return string
   */
  function get_value($values, $field = NULL) {
    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;
    return $values->{$alias}; 
  }

  /**
   * Override parent render.
   * @param $values
   * @return mixed|string
   */
  function render($values) {
    $value = $this->get_value($values);
    $value = $this->sanitize_value($value);

    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;

    // Replace transaction ID value with on-site link, if available:
    if ($alias == 'transaction_id') {
      if (!is_numeric($value)) {
        $value = 'N/A';
      }
      else {
        $value = l($value, '<front>');
      }
    }
    // Replace the form name with on-site link, if available:
    elseif ($alias == 'form_name') {
      $value = l($value, '<front>');
    }
    elseif ($alias == 'recurs_monthly') {
      $value = ($value == 'recurs' ? 'Yes' : 'No');
      $value = l($value, '<front>');
    }
    return $value;
  }
}
