<?php

function _webform_defaults_tickets() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'field_prefix' => '',
      'field_suffix' => '',
    ),
  );
}

  /**
 * Implements _webform_edit_component().
 */
function _webform_edit_tickets($component) {
  $form = array();
  $form['edit_notice']['#markup'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_tickets($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $node_wrapped = entity_metadata_wrapper('node', $node);
  $element = array();
  $element['#prefix'] =  '<table class="ticket_table">';
  $element['#prefix'] .= '<tr class="ticket_headers"><th>Type</th><th>Description</th><th>Quantity</th><th>Amount</th></tr>';
  $element['#suffix'] = '<tr class="ticket_additional_donation"><td colspan="2"><input type="checkbox">Add an additional donation?</td><td><input type="text"></td><td>$0</td></tr>';
  $element['#suffix'] .= '<tr class="ticket_total"><td colspan="3">Total</td><td>$0</td></tr>';
  $element['#suffix'] .= '</table>';

  $ticket_rows = $node_wrapped->fr_tickets_ticket_types->value();

  if (! empty($ticket_rows)){

    $rows = array();
    // Loop through the payment methods and add fields for each type.
    foreach ($ticket_rows as $i => $ticket_row) {
      $product_wrapped = entity_metadata_wrapper('commerce_product', $ticket_row);
      // Setup the fieldset
      $element[$product_wrapped->sku->value()] = array(
        '#type' => 'select',
        '#options' => range(0,50),
        '#title' => '',
        '#prefix' => '<tr class="ticket_row">' . '<td>' . $product_wrapped->title->value()
          . '(' . commerce_currency_format($product_wrapped->commerce_price->amount->value(), $product_wrapped->commerce_price->currency_code->value(), $product_wrapped) . ')</td>'
          . '<td>' . $product_wrapped->fr_tickets_description->value() . '</td>'
          . '<td>',
        '#suffix' => '</td><td id="' . $product_wrapped->sku->value() .'_total">$0</td>' . '</tr>',
        '#attributes' => array(
          'class' => array(''),
        ),
      );
    }

  }

  return $element;
}

/**
 * Pre-render function to add our id to the js settings
 */
function _fundraiser_component_tickets_pre_render($element) {
  // Add the id of the outer fieldset to the js settings
  //drupal_add_js(array('fundraiserWebform' => array('components' => array('payment_fields' => array('id' => $element['#attributes']['id'])))), 'setting');
  return $element;
}
