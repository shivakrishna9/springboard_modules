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

  $element = array(
    '#type' => 'item',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#weight' => $component['weight'],
    '#attributes' => array('class' => 'webform-component-tickets'),
    '#title_display' => 'none',
    '#webform_component' => $component,
    '#theme' => 'fundraiser_tickets_webform_table'
  );

  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $node_wrapper = entity_metadata_wrapper('node', $node);
  $ticket_types = $node_wrapper->fr_tickets_ticket_types->value();
  $element['#ticket_types'] = $ticket_types;

  // Hide the event_table title if #title_display is 'none'.
  if (!empty($component['extra']['title_display']) && $component['extra']['title_display'] == 'none') {
    $element['#title'] = NULL;
  }

  return $element;
}

/**
 * Pre-render function to add our id to the js settings
 */
function webform_tickets_prerender($element) {
  // $id = 'webform-component-' . str_replace('_', '-', implode('--', array_slice($element['#parents'], 1)));
  // $element['#attributes']['id'] = $id;
  // drupal_add_js(array('acluEvents' => array('table' => array('id' => $id))), 'setting');
  return $element;
}


/**
 * Theme the form elements into a table
 */
function theme_fundraiser_tickets_webform_table($variables) {


  $element = $variables['element'];

  if (! empty($element['#ticket_types'])){

    $rows = array();
    // Loop through the element and output the children fields as rows
    foreach ($element['#ticket_types'] as $i => $ticket_type) {
      $product_wrapper = entity_metadata_wrapper('commerce_product', $ticket_type);
      $id = $product_wrapper->product_id->value();
      $row  = array();
      $item = array();
      $item['#type'] = 'select';
      $item['#options'] = range(0,50);
      $title = check_plain($product_wrapper->title->value()) . ' (' . commerce_currency_format($product_wrapper->commerce_price->amount->value(), $product_wrapper->commerce_price->currency_code->value(), $product_wrapper) . ')';
      $item['#title'] = '';
      $description = check_plain($product_wrapper->fr_tickets_description->value());
      // Add a div to contain the ticket total
      $ticket_total = array('data' => '$ 0.00', 'class' => 'event-ticket-select-total', 'id' => $id . '-ticket-total', 'align' => 'right');
      $row['data'] = array($title, $description, array('data'=> drupal_render($item), 'align' => 'right'), $ticket_total);
      $rows[] = $row;
    }

    // Add a row to contain our totals
    $rows[] = array('data' => array(t('Totals'), '', array('data' => 0, 'id' => $id . '-total-quantity', 'align' => 'right'), array('data' => '$ 0.00', 'id' => $id . '-total-cost', 'align' => 'right')));
    $header = array(t('Type'), '', array('data' => t('Quantity'), 'align' => 'right'), array('data' => t('Amount'), 'align' => 'right'));
    $attributes = array('class' => $element['#attributes']['class'], 'id' => $id, 'width' => '100%');

    $table = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    );
    $output = drupal_render($table);
    return $output;
  }
}
