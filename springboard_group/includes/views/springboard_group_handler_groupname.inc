<?php

// This file must be at mymodule/views directory.

/**
 * @file
 * Definition of mymodule_handler_handlername.
 */

/**
 * Description of what my handler does.
 */
class springboard_group_handler_groupname extends views_handler_field {
  /**
   * Add some required fields needed on render().
   */
  function construct() {
    parent::construct();
    $this->additional_fields['field_something'] = array(
      'table' => 'node',
      'field' => 'title',
    );
  }

  /**
   * Loads additional fields.
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function click_sort($order) {
    dpm($this);
    if (isset($this->field_alias)) {

    }
  }

  /**
   * Default options form.
   */
  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  /**
   * Creates the form item for the options added.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }

  /**
   * Renders the field handler.
   */
  function render($values) {
    $groups = og_get_entity_groups('node', node_load($values->nid));
    $titles = [];
    foreach ($groups['node'] as $group_id) {
      $group = node_load($group_id);
      $titles[] = $group->title;
    }

    return implode(' | ', $titles);
  }
}