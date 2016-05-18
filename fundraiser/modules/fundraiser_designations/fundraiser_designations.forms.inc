<?php
/**
 * @file
 * Fundraiser_designations.forms.inc - Designation management form.
 */

function fundraiser_designations_designation_groups_page($node) {
  $build = array();
  $build['content'] = array(
    'header' => array(
      '#weight' => -9,
      '#markup' => 'Title and description header',
    ),
    'page_description_content' => array(
      '#id' => 'message-action-preview-page',
      '#type' => 'markup',
      '#markup' => fundraiser_designations_categories_list(),
      '#prefix' => '<div class="fundraiser-designations-categories"><p>',
      '#suffix' => '</p></div>',
      '#weight' => -8,
    ),
    'fd_bundler_form' => drupal_get_form('fundraiser_designations_bundler_form', $node),
  );

  return $build;
}

function fundraiser_designations_categories_list() {

  $products_with_tags = db_query("
    SELECT ttd.name AS category,
    ttd.vid AS vid,
    ttd.tid AS tid,
    cp.product_id AS product_id,
    cp.title AS product_title
    FROM
    {taxonomy_term_data} ttd
    LEFT JOIN {field_data_field_fd_designation_categories} dc
      ON ttd.tid = dc.field_fd_designation_categories_tid
      AND (dc.entity_type = 'commerce_product'
      AND dc.deleted = '0')
    LEFT JOIN {commerce_product} cp ON dc.entity_id = cp.product_id
    LEFT JOIN {taxonomy_vocabulary} tv ON ttd.vid = tv.vid
    WHERE tv.machine_name IN  ('fd_designation_categories')
  ")->fetchAll();

  $rows = array();
  foreach ($products_with_tags as $pwt) {
    $rows[$pwt->category]['data'] = $pwt->category;
    $rows[$pwt->category]['data-category-id'] = array($pwt->vid);
    $rows[$pwt->category]['children'][$pwt->product_title]['data'] = $pwt->product_title;
    $rows[$pwt->category]['children'][$pwt->product_title]['data-product-id'] = array($pwt->product_id);
  }
  $output = theme('item_list', array('items' => $rows));
  return $output;
}

function fundraiser_designations_bundler_form($form, $form_state, $node) {
  //db_query.
  return array();
}