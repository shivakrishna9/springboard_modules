<?php


class springboard_views_handler_webform_filter_submission_date extends views_handler_filter_date {
  function query() {
    // Join in an instance of the webform_submissions table
    $this->ensure_my_table();
    $first = array(
      'left_table' => 'node',
      'left_field' => 'nid',
      'table' => 'webform_submissions',
      'field' => 'nid',
      'type' => 'inner',
    );
    $join = new views_join();
    $join->definition = $first;
    $join->construct();
    $join->adjusted = TRUE;

    // Ensure our aliases are correct since this is a "Fake" field.
    $this->real_field = 'submitted';
    $this->table_alias = 'webform_submissions_time';
    $this->field_alias = "last_submitted";

    // Adding the `submitted` field on {webform_submissions} using the MAX() sql function.
    //$this->query->add_field(NULL, "{$this->table_alias}.{$this->real_field}", $this->field_alias, array('function' => 'max'));
    if (isset($this->options['value']['value']) && $this->options['value']['value'] != '') {
      $this->op_simple();
    }
    else if ($this->options['value']['max'] != '' || $this->options['value']['min'] != '') {
      $this->op_between();
    }

    // Adding the join we created above.
    $this->query->add_relationship($this->table_alias, $join, 'node');
  }
  
  function op_between() {
    // Use the substitutions to ensure a consistent timestamp.
    $query_substitutions = views_views_query_substitutions($this->view);
    $a = intval(strtotime($this->value['min'], $query_substitutions['***CURRENT_TIME***']));
    $b = intval(strtotime($this->value['max'], $query_substitutions['***CURRENT_TIME***']));

    // This is safe because we are manually scrubbing the values.
    // It is necessary to do it this way because $a and $b are formulas when using an offset.
    $operator = strtoupper($this->operator);
    $this->query->add_having_expression($this->options['group'], "MAX($this->table_alias.$this->real_field) $operator $a AND $b");

    //$this->query->add_where_expression($this->options['group'], "$field $operator $a AND $b");
  }

  function op_simple() {
    // Use the substitutions to ensure a consistent timestamp.
    $query_substitutions = views_views_query_substitutions($this->view);
    $value = intval(strtotime($this->value['value'], $query_substitutions['***CURRENT_TIME***']));

    // This is safe because we are manually scrubbing the value.
    // It is necessary to do it this way because $value is a formula when using an offset.
    $operator = strtoupper($this->operator);
    $this->query->add_having_expression($this->options['group'], "MAX($this->table_alias.$this->real_field) $operator $value");

    //$this->query->add_where_expression($this->options['group'], "$field $this->operator $value");
  }
}