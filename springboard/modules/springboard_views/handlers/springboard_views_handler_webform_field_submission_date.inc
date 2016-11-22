<?php

class springboard_views_handler_webform_field_submission_date extends views_handler_field_date {

  // Update the query to correctly join the webform_submissions table and output only the date of the latest submission.
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
    $table_alias = 'webform_submissions_time';
    $this->field_alias = "last_submitted";

    // Adding the `submitted` field on {webform_submissions} using the MAX() sql function.
    $this->query->add_field(NULL, "{$table_alias}.{$this->real_field}", $this->field_alias, array('function' => 'max'));
    // Adding the join we created above.
    $this->query->add_relationship($table_alias, $join, 'node');

  }

}