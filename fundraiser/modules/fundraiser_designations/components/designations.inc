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
    $groups_by_weight = [];
    foreach ($groups as $key => $group) {
      $groups_by_weight[$group->data['weight'] . $key] = $group;
    }
    ksort($groups_by_weight);
    foreach ($groups_by_weight as $group) {
      $group_wrapper = entity_metadata_wrapper('fundraiser_fund_group', $group);
      $funds = $group_wrapper->field_ffg_fd_reference->value();
      $num = count($funds);
      if ($num > 1) {
        _webform_render_funds_multiple($element, $node, $group_wrapper, $funds, $amounts, $groups_by_weight);
      }
      else {
        _webform_render_funds_single($element, $node, $group_wrapper, $funds, $amounts, $groups_by_weight);
      }
    }
  }
  return $element;
}

function _webform_render_funds_multiple(&$element, $node, $group_wrapper, $funds, $amounts, $groups_by_weight) {

  $options = array();
  $options[] = 'Select';
  foreach ($funds as $fund) {
    $options[$fund->sku] = $fund->title;
  }

  $id = $group_wrapper->fundraiser_fund_group_id->value();

  $item = array();
  $item['#id'] = 'group-item-' . $id;
  $item['#extra']['group_id'] = $id;
  $item['#extra']['fund_type'] = 'multiple';
  $item['#extra']['position'] = _webform_render_funds_get_position($groups_by_weight, $id);
  $item['#type'] = 'item';
  $element[$id] = $item;

  $child = array();
  $child['#type'] = 'select';
  $child['#id'] = 'funds-select-' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#options'] = $options;
  $child['#title'] = '';
  $element[$id]['funds_select_' . $id] = $child;

  _fundraiser_designations_build_child_elements($id, $item, $element, $amounts);
}

function _webform_render_funds_single(&$element, $node, $group_wrapper, $funds, $amounts, $groups_by_weight) {
  $id = $group_wrapper->fundraiser_fund_group_id->value();

  $item = array();
  $item['#id'] = 'group-item-' . $id;
  $item['#extra']['group_id'] = $id;
  $item['#extra']['fund_type'] = 'single';
  $item['#extra']['position'] = _webform_render_funds_get_position($groups_by_weight, $id);
  $item['#type'] = 'item';
  $element[$id] = $item;

  $child = array();
  $child['#type'] = 'item';
  $child['#id'] = 'funds-placeholder-' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#title'] = '';
  $element[$id]['funds_placeholder_' . $id] = $child;

  _fundraiser_designations_build_child_elements($id, $item, $element, $amounts);

}

function _webform_render_funds_get_position($groups_by_weight, $id) {
  reset($groups_by_weight);
  $first_key = key($groups_by_weight);
  end($groups_by_weight);
  $last_key = key($groups_by_weight);
  if ($groups_by_weight[$first_key]->fundraiser_fund_group_id == $id) {
    return 'desigation-group-first';
  }
  if ($groups_by_weight[$last_key]->fundraiser_fund_group_id == $id) {
    return 'desigation-group-last';
  }
  return 'desigation-group-middle';
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
    $child['#id'] = 'default-amounts-' . $id;
    $child['#extra']['group_id'] = $id;
    $child['#options'] = $donation_amounts;
    $child['#title'] = '';
    $element[$id]['default_amounts_' . $id] = $child;
  }

  $recurring_donation_amounts = [];
  if (!empty($amounts['recurring'])) {
    foreach ($amounts['recurring'] as $key => $amount) {
      $recurring_donation_amounts[$amount[0]] = "$amount[1]";
    }
    $child = array();
    $child['#type'] = 'radios';
    $child['#id'] = 'recurring-amounts-' . $id;
    $child['#extra']['group_id'] = $id;
    $child['#options'] = $recurring_donation_amounts;
    $child['#title'] = '';
    $element[$id]['recurring_amounts_' . $id] = $child;
  }

  $child = array();
  $child['#type'] = 'textfield';
  $child['#id'] = 'fd-other-' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#title'] = 'Other amount';
  $element[$id]['fd_other_' . $id] = $child;


  $child = array();
  $child['#type'] = 'button';
  $child['#id'] = 'add-' . $id;
  $child['#extra']['group_id'] = $id;
  $child['#value'] = 'Add';
  $element[$id]['add_' . $id] = $child;
}

/**
 * Theme the form elements into a table.
 */
function theme_fundraiser_designations_fund_groups_component($variables) {

  $element = $variables['element'];
  $output = '';

  foreach (element_children($element) as $id) {
    $build = array();
    if (isset($element[$id]['#extra']['group_id'])) {

      $group_wrapper = entity_metadata_wrapper('fundraiser_fund_group', entity_load_single('fundraiser_fund_group', $element[$id]['#extra']['group_id']));
      $description = $group_wrapper->field_ffg_description->value();
      $image = $group_wrapper->field_ffg_image->value();
      $image['path'] = file_create_url($image['uri']);
      $build['content'] = array(

        '#prefix' => '<div class="designation-group-wrapper ' . $element[$id]['#extra']['position'] . '" id="designation-group-' . $element[$id]['#extra']['group_id'] . '">',
        '#suffix' => '</div>',

        'fund_group_title' => array(
          '#prefix' => '<div class="designation-group-title"><h4>',
          '#suffix' => '</h4></div>',
          '#markup' => check_plain($group_wrapper->name->value()),
          '#weight' => -9,
        ),

        'fund_group_image' => array(
          '#type' => 'markup',
          '#markup' => theme('image', $image),
          '#prefix' => '<div class="designation-group-image">',
          '#suffix' => '</div>',
          '#weight' => -8,
        ),

        'fund_group_description' => array(
          '#type' => 'markup',
          '#markup' => check_markup($description['value'], $description['format']),
          '#prefix' => '<div class="designation-group-description">',
          '#suffix' => '</div>',
          '#weight' => -7,
        ),

        'fund_group_table' => array(
          '#type' => 'markup',
          '#markup' => fundraiser_designations_selection_table($element[$id]),
          '#prefix' => '<div class="designation-group-funds-table-wrapper">',
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
  $row[] = array(
    'data' => array(
      array(
        'data' => drupal_render($element),
        'align' => 'right',
      ),
    ),
    'class' => array('group-row-' . $element['#extra']['group_id']),
  );

  $table = array(
    '#theme' => 'table',
    '#header' => array(),
    '#rows' => $row,
    '#attributes' => array(
      'class' => array('designation-group-funds-table'),
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
