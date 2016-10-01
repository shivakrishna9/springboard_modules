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
    if (!is_object($values) || !isset($values->card_id)) {
      return '';
    }

    $used_by = &drupal_static(__CLASS__ . ':' . __FUNCTION__);

    $card_id = $values->card_id;
    if (empty($used_by[$card_id])) {
      $used_by[$card_id] = module_invoke_all('fundraiser_commerce_cardonfile_used_by', $card_id);
    }

    $output = '';
    if (count($used_by[$card_id])) {
      foreach ($used_by[$card_id] as $ub) {
        if (!is_array($ub)) {
          $ub = array($ub);
        }
        foreach ($ub as $u) {
          if (!empty($this->options['element_type'])) {
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
          else {
            $output .= $u . ' ';
          }
        }
      }
    }

    return $output;
  }
}
