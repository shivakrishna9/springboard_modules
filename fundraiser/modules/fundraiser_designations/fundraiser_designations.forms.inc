<?php
/**
 * @file
 * Fundraiser_designations.forms.inc - Designation management form.
 */

function fundraiser_designations_designation_groups_page($node) {
  $build = array();
  $build['#attached']['css'][] = drupal_get_path('module', 'fundraiser_designations') . '/css/designation_form.css';
  $form = drupal_get_form('fundraiser_designations_bundler_form', $node);
  $build['content'] = array(
    'header' => array(
      '#weight' => -9,
      '#markup' => '<h1>Title and description header</h1>',
    ),
    'categories_table' => array(
      '#id' => 'categories-table',
      '#type' => 'markup',
      '#markup' => fundraiser_designations_categories_list(),
      '#prefix' => '<div id="fundraiser-designations-categories"><h2>Funds</h2>',
      '#suffix' => '</div>',
      '#weight' => -8,
    ),
    'fund_group_table' => array(
      '#id' => 'fund-group-table',
      '#type' => 'markup',
      '#markup' => render($form),
      '#prefix' => '<div id="fundraiser-designations-groups"><h2>Fund Groups</h2>',
      '#suffix' => '</div>',
      '#weight' => -7,
    ),

  );

  return $build;
}

function fundraiser_designations_categories_list() {

  $products_with_tags = db_query("
    SELECT ttd.name AS category,
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

  $products_by_tag = array();
  foreach ($products_with_tags as $pwt) {
    $products_by_tag[$pwt->category]['tid'] = $pwt->tid;
    $products_by_tag[$pwt->category]['product'][] = array(
      'title' => $pwt->product_title,
      'id' => $pwt->product_id,
    );
  }
  $output = '';
  foreach ($products_by_tag as $name => $values) {
    $rows = [];
    foreach ($values['product'] as $pwt) {
      $rows[] = array(
        'data' => array('data' => $pwt['title']),
        'data-product-id' => $pwt['id'],
      );
    }
    $output .= theme('table', array(
      'header' => array(
        'data' => array('data' => $name . ' funds', 'data-category-id' => $values['tid']),
      ),
      'rows' => $rows,
      'attributes' => array('data-category-id' => $values['tid']),
    ));

  }
  return $output;
}

function fundraiser_designations_bundler_form($form, $form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'groups' => array(),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $funds = db_query('
    SELECT g.fund_group_name,
    g.fund_group_id,
    cp.title as product_name,
    cp.product_id as fund_id,
    cp.sku,
    f.fund_weight,
    g.fund_group_weight,
    g.fund_group_featured
    FROM {fd_fund_group_funds} f
    LEFT JOIN {commerce_product} cp ON f.fund_id = cp.product_id
    LEFT JOIN {fd_fund_group} g ON f.fund_group_id = g.fund_group_id
    WHERE g.fund_group_form_id = :nid',
    array(':nid' => $node->nid)
  )->fetchAll();

  $groups = db_query('
    SELECT g.fund_group_name,
    g.fund_group_id,
    g.fund_group_weight,
    g.fund_group_featured
    FROM {fd_fund_group} g
    WHERE g.fund_group_form_id = :nid',
    array(':nid' => $node->nid)
  )->fetchAll();


  $funds_by_group = [];
  foreach ($groups as $group) {
    $funds_by_group[$group->fund_group_id]['name'] = $group->fund_group_name;
    $funds_by_group[$group->fund_group_id]['featured'] = $group->fund_group_featured;
    $funds_by_group[$group->fund_group_id]['weight'] = $group->fund_group_weight;
    foreach ($funds as $fund) {
      if ($fund->fund_group_id == $group->fund_group_id) {
        $funds_by_group[$group->fund_group_id]['funds'][] = array(
          'fund_name' => $fund->product_name,
          'fund_id' => $fund->fund_id,
          'fund_weight' => $fund->fund_weight,
        );
      }
    }
  }


  foreach ($funds_by_group as $group_id => $group) {
    $form['groups'][$group_id]['name'] = array(
      '#type' => 'hidden',
      '#default_value' => $group['name'],
    );
    $form['groups'][$group_id]['weight'] = array(
      '#type' => 'hidden',
      '#default_value' => $group['weight'],
    );
    $form['groups'][$group_id]['featured'] = array(
      '#type' => 'checkbox',
      '#title' => t('Featured'),
      '#default_value' => $group['featured'],
    );

    if (!empty($group['funds'])) {
      foreach ($group['funds'] as $fund) {
        $form['groups'][$group_id]['funds'][$fund['fund_id']]['name'] = array(
          '#type' => 'hidden',
          '#default_value' => $fund['fund_name'],
        );
        $form['groups'][$group_id]['funds'][$fund['fund_id']]['weight'] = array(
          '#type' => 'hidden',
          '#default_value' => $fund['fund_weight'],
        );
        $form['groups'][$group_id]['funds'][$fund['fund_id']]['fund_id'] = array(
          '#type' => 'hidden',
          '#default_value' => $fund['fund_id'],
        );
      }
    }

  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  return $form;
}

function theme_fundraiser_designations_bundler_form($variables) {

  $form = $variables['form'];
  $output = '';
  $children = element_children($form['groups']);

  foreach ($children as $child) {
    $header = array(
      'data' => array('data' => $form['groups'][$child]['name']['#value'] . ' funds'),
      array('data' => drupal_render($form['groups'][$child]['featured'])),
    );
    $rows = [];

    if (!empty($form['groups'][$child]['funds'])) {
      $fund_children = element_children($form['groups'][$child]['funds']);
      foreach ($fund_children as $id) {
        $rows[] = array(
          'data' => array(
            'data' => array(
              'data' => $form['groups'][$child]['funds'][$id]['name']['#value'],
              'colspan' => 2,
            ),
          ),
          'data-product-id' => $id,
        );
      }
    }

    $output .= theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'designations-group-table'),
      )
    );
  }

  $output .= drupal_render_children($form);
  return $output;
}