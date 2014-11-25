<?php

/**
 * @file
 * Definition of views_handler_filter_combine.
 */

/**
 * Filter handler which allows to search on multiple fields.
 *
 * @ingroup views_field_handlers
 */
class springboard_advocacy_views_handler_filter_combine extends springboard_advocacy_views_handler_filter_string {
  /**
   * @var views_plugin_query_default
   */
  public $query;

  function option_definition() {
    $options = parent::option_definition();
    $options['fields'] = array('default' => array());

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $this->view->init_style();

    // Allow to choose all fields as possible.
    if ($this->view->style_plugin->uses_fields()) {
      $options = array();
      foreach ($this->view->display_handler->get_handlers('field') as $name => $field) {
        $options[$name] = $field->ui_name(TRUE);
      }
      if ($options) {
        $form['fields'] = array(
          '#type' => 'select',
          '#title' => t('Choose fields to combine for filtering'),
          '#description' => t("Modified to advacy fields"),
          '#multiple' => TRUE,
          '#options' => $options,
          '#default_value' => $this->options['fields'],
        );
      }
      else {
        form_set_error('', t('You have to add some fields to be able to use this filter.'));
      }
    }
  }

  function query() {
    $this->view->_build('field');
    $fields = array();
    foreach ($this->options['fields'] as $id) {
      $field = $this->view->field[$id];
      $fields[] = $field->real_field;
    }
    if ($fields) {
      $separated_fields = array();
      foreach ($fields as $key => $field) {
        $separated_fields[] = $field;
      }

      $expression = implode(', ', $separated_fields);

      $info = $this->operators();
      if (!empty($info[$this->operator]['method'])) {
        $this->{$info[$this->operator]['method']}($expression);
      }
    }
  }

  function op_contains($field) {
    $this->query->search_terms[$field] = $this->value;
  }
}
