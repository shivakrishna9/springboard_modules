<?php
/**
 * @file
 * Handler for the Used By field.
 */

/**
 * Field handler to present the card expiration date.
 */
class fundraiser_commerce_cardonfile_handler_field_used_by extends views_handler_field_entity {

  function render($values) {
    if (!isset($this->entities[$this->view->row_index])) {
      return '';
    }

    $used_by = &drupal_static(__FUNCTION__);

    $card_id = $this->view->row_index;
    if (empty($used_by[$card_id])) {
      $used_by[$card_id] = module_invoke_all('fundraiser_commerce_cardonfile_used_by', $card_id);
    }

    $output = '';
    if (count($used_by[$card_id])) {
      foreach ($used_by[$card_id] as $u) {
        $element = array(
          'element' => array(
            '#tag' => $this->options['element_type'],
            '#value' => $u,
          ),
        );
        if (!empty($this->options['element_class'])) {
          $element['element']['#attributes']['class'] = $this->option['element_class'];
        }
        $output .= theme('html_tag', $element);
      }
    }

    return $output;
  }
}
