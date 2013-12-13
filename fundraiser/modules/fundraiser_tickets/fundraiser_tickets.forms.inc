<?php
/**
 * @file
 * fundraiser_tickets.forms.inc - Ticket management form
 */

function fundraiser_tickets_ticket_form($form, &$form_state, $node, $product = NULL) {

  $edit = FALSE;

  if (is_object($product)){
    $product_wrapper = entity_metadata_wrapper('commerce_product', $product);
    $edit = TRUE;
    // TODO: verify that this product is associated with this form
  }

  // Build the form
  $form = array();

  $form['options']['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $edit ? $product_wrapper->title->value() : '',
  );

  $form['options']['price'] = array(
    '#title' => t('Price'),
    '#type' => 'textfield',
    '#size'=> 5,
    '#default_value' => $edit ? commerce_currency_amount_to_decimal($product_wrapper->commerce_price->amount->value(), $product_wrapper->commerce_price->currency_code->value()) : 0,
  );

  $form['options']['fr_tickets_description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $edit ? $product_wrapper->fr_tickets_description->value() : '',
  );

  $form['options']['fr_tickets_sold_out_message'] = array(
    '#title' => t('Sold Out Message'),
    '#type' => 'textfield',
    '#default_value' => $edit ? $product_wrapper->fr_tickets_sold_out_message->value() : '',
  );

  $form['options']['fr_tickets_threshold'] = array(
    '#title' => t('Warning threshold'),
    '#type' => 'textfield',
    '#size'=> 3,
    '#default_value' => $edit ? $product_wrapper->fr_tickets_threshold->value() : 0,
  );

  $form['options']['fr_tickets_quantity'] = array(
    '#title' => t('Quantity'),
    '#type' => 'textfield',
    '#size'=> 5,
    '#default_value' => $edit ? $product_wrapper->fr_tickets_quantity->value() : 0,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 19,
  );

  return $form;
}

function fundraiser_tickets_ticket_form_validate($form, &$form_state) {
  // TODO: Validated submitted data
  return false;
}


function fundraiser_tickets_ticket_form_submit($form, &$form_state) {
  global $user;
  $edit = FALSE;

  $node = $form_state['build_info']['args'][0];
  if (isset($form_state['build_info']['args'][1])){
    $edit = TRUE;
    $cp = $form_state['build_info']['args'][1];
  } else {
    $cp = commerce_product_new('fundraiser_ticket');
    $cp->is_new = TRUE;
    $cp->revision_id = NULL;
    $cp->uid = $user->uid;
    $cp->status = 1;
    $cp->created = $cp->changed = time();
    $cp->sku = 'ticket_' . $node->nid . '_' . time();
  }
  $cp->title = $form_state['values']['title'];
  $cp->fr_tickets_quantity = array(LANGUAGE_NONE => array( 0 => array('value' => $form_state['values']['fr_tickets_quantity'])));
  $cp->fr_tickets_description = array(LANGUAGE_NONE => array( 0 => array('value' => $form_state['values']['fr_tickets_description'])));
  $cp->fr_tickets_threshold = array(LANGUAGE_NONE => array( 0 => array('value' => $form_state['values']['fr_tickets_threshold'])));
  $cp->fr_tickets_sold_out_message = array(LANGUAGE_NONE => array( 0 => array('value' => $form_state['values']['fr_tickets_sold_out_message'])));

  if ($edit == FALSE){
    $cp->fr_tickets_weight = array(LANGUAGE_NONE => array( 0 => array('value' => 10)));
  }

  $cp->language = LANGUAGE_NONE;
  $cp->commerce_price = array(LANGUAGE_NONE => array( 0 => array('amount' => $form_state['values']['price'] * 100, 'currency_code' => 'USD',)));
  if (commerce_product_save($cp) && $cp->product_id){
    if ($edit == FALSE){
      $node_wrapped = entity_metadata_wrapper('node', $node);
      $node_wrapped->fr_tickets_ticket_types[] = $cp->product_id;
      $node_wrapped->save();
    }
  }
  drupal_goto('node/' . $node->nid . '/tickets/tickets');
}


function fundraiser_tickets_order_ticket_types_form($form, &$form_state, $node){
  global $language;
  $products = array();

  if (! empty($node->fr_tickets_ticket_types)){
    $node_wrapper = entity_metadata_wrapper('node', $node);
    $products_wrapped = $node_wrapper->language($language->language)->fr_tickets_ticket_types;
    $products = $products_wrapped->value();
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
      $product_wrapped = entity_metadata_wrapper('commerce_product', $product);
      $form['tickets'][$key]['price'] = array(
        '#markup' => commerce_currency_format($product_wrapped->commerce_price->amount->value(), $product_wrapped->commerce_price->currency_code->value(), $product)
      );
      $form['tickets'][$key]['title'] = array(
        '#markup' => $product_wrapped->title->value(),
      );
      $form['tickets'][$key]['description'] = array(
        '#markup' => $product_wrapped->fr_tickets_description->value(),
      );
      $form['tickets'][$key]['quantity'] = array(
        '#markup' => $product_wrapped->fr_tickets_quantity->value() ,
      );
      $form['tickets'][$key]['sold_out_message'] = array(
        '#markup' => $product_wrapped->fr_tickets_sold_out_message->value() ,
      );
      $form['tickets'][$key]['warning'] = array(
        '#markup' => $product_wrapped->fr_tickets_threshold->value() ,
      );
      $form['tickets'][$key]['edit_link'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'node/' . $node->nid . '/tickets/' . $product_wrapped->product_id->value() . '/edit',
      );

      $form['tickets'][$key]['ticket_id'] = array(
        '#type' => 'hidden',
        '#value' => $product_wrapped->product_id->value()
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
    drupal_goto('node/' . $form_state['build_info']['args'][0]->nid . '/tickets/add');
  }
}


function fundraiser_tickets_order_ticket_types_form_submit($form, &$form_state) {
  foreach ($form_state['values']['tickets'] as $i => $ticket){
    $product_wrapped = entity_metadata_wrapper('commerce_product', commerce_product_load($ticket['ticket_id']));
    if ($product_wrapped->fr_tickets_weight->value() != $ticket['weight']){
      $product_wrapped->fr_tickets_weight->set($ticket['weight']);
      $product_wrapped->save();
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



