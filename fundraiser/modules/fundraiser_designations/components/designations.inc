<?php

/**
 * @file
 */

/**
 * Default values for designation form.
 *
 * @return array
 *   Component defaults.
 */
function _webform_defaults_designations() {
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
function _webform_edit_designations($component) {
  $form = array();
  $form['edit_notice']['#markup'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  return $form;
}

/**
 * Implements _webform_submit_component().
 *
 * Convert submitted keys and values into serialized, saveable data.
 */
function _webform_submit_designations($component, $values) {
  $return = array();
  foreach ($values as $key => $submitted_value) {
    $return[] = serialize(array($key => $submitted_value));
  }
  return $return;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_designations($component, $value = NULL, $filter = TRUE) {

  $element = array(
    '#type' => 'item',
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#weight' => $component['weight'],
    '#attributes' => array('class' => array('webform-component-designations')),
    '#title_display' => 'none',
    '#webform_component' => $component,
    '#theme' => 'fundraiser_designations_fund_groups_component',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'fundraiser_designations') . '/js/fundraiser_designations.js'),
      'css' => array(drupal_get_path('module', 'fundraiser_designations') . '/css/designation-client-form.css'),
    ),
  );

  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $amounts = fundraiser_designations_get_display_amounts($node);

  if (fundraiser_designations_is_designation_type($node->type) && !empty($node->designation_group_ids)) {
    $groups = entity_load('fundraiser_fund_group', $node->designation_group_ids);
    //@todo sort by group weight.
    foreach ($groups as $group) {

      $group_wrapper = entity_metadata_wrapper('fundraiser_fund_group', $group);
      $funds = $group_wrapper->field_ffg_fd_reference->value();
      $num = count($funds);
      if ($num > 1) {
        _webform_render_funds_multiple($element, $node, $group_wrapper, $funds, $amounts);
      }
      else {
        _webform_render_funds_single($element, $node, $group_wrapper, $funds, $amounts);
      }
    }
  }
  return $element;
}

function _webform_render_funds_multiple(&$element, $node, $group_wrapper, $funds, $amounts) {

  $options = array();
  foreach ($funds as $fund) {
    $options[$fund->sku] = $fund->title;
  }

  $id = $group_wrapper->fundraiser_fund_group_id->value();

  $item = array();
  $item['#id'] = $id;
  $item['#extra']['group_id'] = $id;
  $item['#type'] = 'item';
  $element[$item['#id']] = $item;

  $child = array();
  $child['#type'] = 'select';
  $child['#id'] = 'funds_select';
  $child['#extra']['group_id'] = $id;
  $child['#options'] = $options;
  $child['#title'] = '';
  $element[$item['#id']][$child['#id']] = $child;

  _fundraiser_designations_build_child_elements($id, $item, $element, $amounts);

}

function _webform_render_funds_single(&$element, $node, $group_wrapper, $funds, $amounts) {
  $id = $group_wrapper->fundraiser_fund_group_id->value();

  $item = array();
  $item['#id'] = $id;
  $item['#extra']['group_id'] = $id;
  $item['#type'] = 'item';
  $element[$item['#id']] = $item;

  $child = array();
  $child['#type'] = 'item';
  $child['#id'] = 'funds_placeholder';
  $child['#extra']['group_id'] = $id;
  $child['#title'] = '';
  $element[$item['#id']][$child['#id']] = $child;

  _fundraiser_designations_build_child_elements($id, $item, $element, $amounts);

}

/**
 *
 */
function _fundraiser_designations_build_child_elements($id, $item, &$element, $amounts) {
  $donation_amounts = [];
  if (!empty($amounts['default'])) {
    foreach ($amounts['default'] as $key => $amount) {
      $donation_amounts[$amount[0]] = "$amount[1]";
    }
    $child = array();
    $child['#type'] = 'radios';
    $child['#id'] = 'default_amounts_' . $id;
    $child['#extra']['group_id'] = $id;
    $child['#options'] = $donation_amounts;
    $child['#title'] = '';
    $element[$item['#id']][$child['#id']] = $child;
  }

  $recurring_donation_amounts = [];
  if(!empty($amounts['recurring'])) {
    foreach ($amounts['recurring'] as $key => $amount) {
      $recurring_donation_amounts[$amount[0]] = "$amount[1]";
    }
    $child = array();
    $child['#type'] = 'radios';
    $child['#id'] = 'recurring_amounts_' . $id;
    $child['#extra']['group_id'] = $id;
    $child['#options'] = $recurring_donation_amounts;
    $child['#title'] = '';
    $element[$item['#id']][$child['#id']] = $child;
  }

  $child = array();
  $child['#type'] = 'textfield';
  $child['#id'] = 'fd_other_' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#title'] = 'Other amount';
  $element[$item['#id']][$child['#id']] = $child;


  $child = array();
  $child['#type'] = 'button';
  $child['#id'] = 'add_' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#value'] = 'Add';
  $element[$item['#id']][$child['#id']] = $child;
}

/**
 * Pre-render function to add our id to the js settings.
 */
function webform_designations_prerender($element) {
  $id = 'webform-component-' . str_replace('_', '-', implode('--', array_slice($element['#parents'], 1)));
  $element['#attributes']['id'] = $id;
  drupal_add_js(array('fundraiserDesignations' => array('table' => array('id' => $id))), 'setting');
  return $element;
}

/**
 * Theme the form elements into a table.
 */
function theme_fundraiser_designations_fund_groups_component($variables) {

  $element = $variables['element'];
  $node = node_load($element['#webform_component']['nid']);
  $output = '';

  foreach (element_children($element) as $id) {
    $build = array();
    if (isset($element[$id]['#extra']['group_id'])) {

      $group_wrapper = entity_metadata_wrapper('fundraiser_fund_group', entity_load_single('fundraiser_fund_group', $element[$id]['#extra']['group_id']));
      $description = $group_wrapper->field_ffg_description->value();
      $image = $group_wrapper->field_ffg_image->value();
      $image['path'] = file_create_url($image['uri']);

      $build['content'] = array(

        '#prefix' => '<div class="fundraiser-designation-group-wrapper">',
        '#suffix' => '</div>',

        'fund_group_title' => array(
          '#prefix' => '<div class="fundraiser-designation-group-title"><h4>',
          '#suffix' => '</h4></div>',
          '#markup' => check_plain($group_wrapper->name->value()),
          '#weight' => -9,
        ),

        'fund_group_image' => array(
          '#type' => 'markup',
          '#markup' => theme('image', $image),
          '#prefix' => '<div class="fundraiser-designation-group-image">',
          '#suffix' => '</div>',
          '#weight' => -8,
        ),

        'fund_group_description' => array(
          '#type' => 'markup',
          '#markup' => check_markup($description['value'], $description['format']),
          '#prefix' => '<div class="fundraiser-designation-group-description">',
          '#suffix' => '</div>',
          '#weight' => -7,
        ),

        'fund_group_table' => array(
          '#type' => 'markup',
          '#markup' => fundraiser_designations_selection_table($element[$id]),
          '#prefix' => '<div class="fundraiser-designation-group-table">',
          '#suffix' => '</div>',
          '#weight' => -6,
        ),
      );
      $output .= drupal_render($build);
    }
  }


  return $output;
}

function fundraiser_designations_selection_table($element) {
  $row['data'] = array(
    array('data' => drupal_render($element), 'align' => 'right'),
  );

  $table = array(
    '#theme' => 'table',
    '#header' => array(),
    '#rows' => $row,
    '#attributes' => array(
      'class' => array('fund-box'),
    ),
  );
  return drupal_render($table);
}

function fundraiser_designations_get_display_amounts($node) {
  $recurring = $node->recurring_setting;
  $recurring_amounts = $node->recurring_donation_amounts;
  $donation_amounts = $node->donation_amounts;
  $dual_ask = $node->recurring_dual_ask_amounts;

  $price_sets = [];
  $price_sets['default_amount'] = $node->default_amount;
  $price_sets['recurring_default_amount'] = $node->recurring_default_amount;

  switch ($recurring) {
    case 'never':
      $price_sets['default'] = $donation_amounts;
      break;

    case 'user_choice':
      if (!empty($dual_ask)) {
        $price_sets['default'] = $donation_amounts;
        $price_sets['recurring'] = $recurring_amounts;
      }
      else {
        $price_sets['default'] = $donation_amounts;
      }
      break;

    case 'always':
      $price_sets['recurring'] = $recurring_amounts;
      break;
  }

  return $price_sets;
}
