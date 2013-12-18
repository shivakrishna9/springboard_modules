<?php
/**
 * @file
 * fundraiser_tickets.forms.inc - Ticket management form
 */

function fundraiser_tickets_order_ticket_types_form($form, &$form_state, $node){
  global $language;
  $products = array();

  if (! empty($node->fr_tickets_ticket_types)){
    $node_wrapper = entity_metadata_wrapper('node', $node);
    $products_wrapper = $node_wrapper->language($language->language)->fr_tickets_ticket_types;
    $products = $products_wrapper->value();
    uasort($products, '_fundraiser_tickets_weight_sort');
  }

  // Build the form
  $form = array();

// create a parent element and use our custom theme
  $form['tickets'] = array(
    '#prefix' => '<div id="ticket-container">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'fundraiser_tickets_theme_order_ticket_types_form'
  );

  if (! empty($products)){

    // create the form elements for each product
    foreach ($products as $key => $product) {
      $product_wrapper = entity_metadata_wrapper('commerce_product', $product);
      $form['tickets'][$key]['price'] = array(
        '#markup' => commerce_currency_format($product_wrapper->commerce_price->amount->value(), $product_wrapper->commerce_price->currency_code->value(), $product)
      );
      $form['tickets'][$key]['title'] = array(
        '#markup' => check_plain($product_wrapper->title->value()),
      );
      $form['tickets'][$key]['description'] = array(
        '#markup' => check_plain($product_wrapper->fr_tickets_description->value()),
      );
      $form['tickets'][$key]['quantity'] = array(
        '#markup' => check_plain($product_wrapper->fr_tickets_quantity->value()),
      );
      $form['tickets'][$key]['sold_out_message'] = array(
        '#markup' => check_plain($product_wrapper->fr_tickets_sold_out_message->value()),
      );
      $form['tickets'][$key]['warning'] = array(
        '#markup' => check_plain($product_wrapper->fr_tickets_threshold->value()),
      );
      $form['tickets'][$key]['edit_link'] = array(
        '#markup' =>  l(t('edit'), 'admin/commerce/products/' . $product_wrapper->product_id->value() . '/edit', array('query' => array('destination' => 'node/' . $node->nid . '/tickets/tickets')))
      );

      $form['tickets'][$key]['ticket_id'] = array(
        '#type' => 'hidden',
        '#value' => $product_wrapper->product_id->value()
      );

      $form['tickets'][$key]['weight'] = array(
        '#type' => 'textfield',
        '#default_value' => $key,
        '#size' => 3,
        '#attributes' => array('class' => array('rank-weight')), // needed for table dragging
      );
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  }

  $form['add'] = array(
    '#type' => 'button',
    '#value' => t('Add ticket type'),
  );



  return $form;
}


function theme_fundraiser_tickets_theme_order_ticket_types_form($vars) {
  $element = $vars['element'];
  drupal_add_tabledrag('form_id', 'order', 'sibling', 'rank-weight'); // needed for table dragging

  $header = array(
    'price' => t('Price'),
    'title' => t('Title'),
    'description' => t('Description'),
    'quantity' => t('Quantity'),
    'sold_out_message' => t('Sold out message'),
    'warning' => t('Warning'),
    'edit_link' => t(''),
    'weight' => t('Weight'),

  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
      $row['class'] = array('draggable'); // needed for table dragging
    }
    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'form_id'), // needed for table dragging
  ));
}


function fundraiser_tickets_order_ticket_types_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == 'Add ticket type'){
    drupal_goto('admin/commerce/products/add/fundraiser-ticket', array('query' => array('destination' => 'node/' . $form_state['build_info']['args'][0]->nid . '/tickets/tickets')));
  }
}


function fundraiser_tickets_order_ticket_types_form_submit($form, &$form_state) {
  foreach ($form_state['values']['tickets'] as $i => $ticket){
    $product_wrapper = entity_metadata_wrapper('commerce_product', commerce_product_load($ticket['ticket_id']));
    if ($product_wrapper->fr_tickets_weight->value() != $ticket['weight']){
      $product_wrapper->fr_tickets_weight->set($ticket['weight']);
      $product_wrapper->save();
    }
  }
}

// Custom array sort function by weight.
function _fundraiser_tickets_weight_sort($a, $b) {
  if (isset($a->fr_tickets_weight) && isset($b->fr_tickets_weight)) {
  //  dpm($a);
    return $a->fr_tickets_weight['und'][0]['value'] < $b->fr_tickets_weight['und'][0]['value'] ? -1 : 1;
  }
  return 0;
}



