<?php

/**
 * @file
 * Definition of views_handler_field_target_actions.
 */

/**
 * A handler to provide target_action links.
 *
 * @ingroup views_field_handlers
 */
class springboard_advocacy_views_handler_field_target_actions extends views_handler_field {

  function query() {
   //override
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['which_links'] = array('default' => array('view', 'edit', 'delete'), 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['which_links'] = array(
      '#type' => 'checkboxes',
      '#options' => drupal_map_assoc(array(t('view'), t('edit'), t('delete'))),
      '#title' => t('Which links should show'),
      '#default_value' => $this->options['which_links'],
      '#description' => t('Select whether view, edit and/or delete links are visible. Edit and Delete links will only be visible on custom targets, and to properly permissioned users.')
    );
    parent::options_form($form, $form_state);
  }

  function render($values) {
    return $this->render_links($values);
  }

  function render_links($values) {
    global $user; 
    $legislator = TRUE;

    if (isset($values->class_name) && $values->class_name = "Legislator") {
      $path = 'springboard/advocacy/targets/legislator';
    }
    else {
      $path = 'springboard/advocacy/targets/custom-targets/';
      $legislator = FALSE;
    }

    $links  = array();

    if (user_access('view targets') && !empty($this->options['which_links']['view'])) {
      $links[] = l('view', $path . '/' . $values->id . '/view');
    }
    if (!$legislator && user_access('edit targets') && !empty($this->options['which_links']['edit'])) {
      $links[] = l('edit', $path . '/' . $values->id . '/edit');
    }
    if (!$legislator && user_access('delete targets') && !empty($this->options['which_links']['delete'])) {
      $links[] = l('edit', $path . '/' . $values->id . '/delete');
    }

    return implode(' | ', $links);
  }
}


