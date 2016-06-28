<?php

/**
 * Add ask amounts to commerce product edit form.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 * @param object $product
 *   A new or existing commerce product.
 */
function fundraiser_fund_group_price_override_form(&$form, $form_state, $group, $node) {

  $data = !empty($group->fd_overrides) ? $group->fd_overrides : array();
  $form['#attached']['js'][] = drupal_get_path('module', 'fundraiser_fund_group') . '/js/price-sets.js';
  $form['override_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Override Ask Amounts'),
    '#weight' => 10,
    '#collapsible' => TRUE,
    '#collapsed' => empty($data['status']),
    '#description' => t('Ask amounts set here will override the ask amounts set on the form, for this group only.'),
  );

  $form['override_settings']['statuses'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#weight' => -10,
    '#default_value' => isset($data['status']) ? $data['status'] : 0,
    '#decription' => t('Set prices that will override the donation form defaults if this item is selected.'),
  );

  if ($node->recurring_setting == 'user_choice' || $node->recurring_setting == 'never') {
    fundraiser_fund_group_default_ask($form, $form_state, 'default', $data);
  }
  if ((!empty($node->recurring_dual_ask_amounts) && $node->recurring_setting == 'user_choice') || $node->recurring_setting == 'always') {
    fundraiser_fund_group_default_ask($form, $form_state, 'recurring', $data);
  }

  $form['actions']['submit']['#submit'][] = 'fundraiser_fund_group_override_form_submit';

}

function fundraiser_fund_group_default_ask(&$form, $form_state, $type, $data) {

  $form['override_settings'][$type . '_amount_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => ucfirst($type) . ' ' . t('Ask amounts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div class="clear-block" id="' . $type . '-ask-wrapper">',
    '#suffix' => '</div>',
    '#states' => array(
      'visible' => array(
        ':input[name="statuses"]' => array('checked' => TRUE),
      ),
    ),
    '#tree' => TRUE,
  );

  $form['override_settings'][$type . '_amount_wrapper']['donation_amounts'] = array(
    '#prefix' => '<div id="' . $type . '-ask-amounts">',
    '#suffix' => '</div>',
    '#theme' => 'fundraiser_ask_amounts',
    '#table_id' => $type . '-ask-amounts-table',
    '#tree' => TRUE,
  );

  // AJAXy bits.
  $donation_amounts = array();
  $default_amount = NULL;
  // The last submission takes precedence.
  if (isset($form_state['values'][$type . '_amount_wrapper']['donation_amounts'])) {
    foreach ($form_state['values'][$type . '_amount_wrapper']['donation_amounts'] as $index => $values) {
      $donation_amounts[$values['weight']] = array($values['amount'], $values['label']);
    }
    ksort($donation_amounts);
  }
  else {
    $price_set = [];
    if (!empty($data['price_set'])) {
      $amounts = unserialize($data['price_set']);
      if (isset($amounts[$type])) {
        foreach ($amounts[$type] as $amount) {
          $price_set[] = array($amount['amount'], $amount['label']);
          if (!empty($amount['default_amount'])) {
            $default_amount = $amount['amount'];
          }
        }
      }
    }
    $donation_amounts = $price_set;
    // If not there, then grab some defaults.
    if (count($donation_amounts) == 0) {
      $donation_amounts = array();
    }
  }

  // Default provide one spare blank for users to fill in.
  // If the button clicked was to add another,
  // this line will add an extra field as expected on reload.
  $donation_amounts[] = array('', '');
  // Generate a form set for each existing amount.
  // (To be themed into table format.)
  $index = 0;
  foreach ($donation_amounts as $weight => $donation_amount) {

    // Filter out other|Other combinations.
    // We don't want to display those in this form.
    // And if we did, the value would fail validation (not a number).
    // There is probably a better way to track this, but for now this'll do.
    if (count($donation_amount) == 2 && $donation_amount[0] != 'other') {
      $form['override_settings'][$type . '_amount_wrapper']['donation_amounts'][$index] = _designation_ask_form($donation_amount, $default_amount, $weight);
    }
    ++$index;
  }

  // AJAX fix: reorder donation amounts in $form_state['input'] by weight.
  // Corrects the display order of donation amounts when an ajax event is
  // triggered.
  if (isset($form_state['input'][$type . '_amount_wrapper']['donation_amounts'])) {
    $old_donation_amounts = $form_state['input'][$type . '_amount_wrapper']['donation_amounts'];
    unset($form_state['input'][$type . '_amount_wrapper']['donation_amounts']);
    foreach ($old_donation_amounts as $donation_amount) {
      $new_donation_amounts[$donation_amount['weight']] = $donation_amount;
    }
    ksort($new_donation_amounts);
    foreach ($new_donation_amounts as $donation_amount) {
      $form_state['input'][$type . '_amount_wrapper']['donation_amounts'][] = $donation_amount;
    }
  }

  $form['override_settings'][$type . '_amount_wrapper'][$type . '_amount_more'] = array(
    '#type' => 'button',
    '#submit' => array(),
    '#validate' => array('fundraiser_fund_group_override_form_button_validate'),
    '#value' => t('Add ' . $type),
    '#description' => t('Click here to add more choices.'),
    '#ajax' => array(
      'callback' => '_fundraiser_fund_group_settings_' .$type .'_form_amounts',
      'wrapper' => $type . '-ask-amounts',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#ask_type' => $type,
  );

}

/**
 * Ajax replacement.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return mixed
 */
function _fundraiser_fund_group_settings_default_form_amounts(&$form, &$form_state) {
  return $form['override_settings']['default_amount_wrapper']['donation_amounts'];
}

function _fundraiser_fund_group_settings_recurring_form_amounts(&$form, &$form_state) {
  return $form['override_settings']['recurring_amount_wrapper']['donation_amounts'];
}


/**
 * Implements hook_form_validate().
 */
function fundraiser_fund_group_override_form_button_validate(&$form, &$form_state) {
  if (form_get_errors()) {
    form_clear_error();
    drupal_get_messages('error');
  }
}

/**
 * Implements hook_form_submit().
 */
function fundraiser_fund_group_override_form_submit($form, &$form_state) {

  $values = $form_state['values'];
  $sets = [];
  $values['gid'] = $form_state['fundraiser_fund_group']->fundraiser_fund_group_id;

  db_delete('fundraiser_fund_group_overrides')
    ->condition('gid', $values['gid'], '=')
    ->execute();

  if (isset($values['default_amount_wrapper'])) {
    foreach ($values['default_amount_wrapper']['donation_amounts'] as $amount) {
      if (empty($amount['remove']) && !empty($amount['amount'])) {
        $sets['default'][] = $amount;
      }
    }
  }
  if (isset($values['recurring_amount_wrapper'])) {
    foreach ($values['recurring_amount_wrapper']['donation_amounts'] as $amount) {
      if (empty($amount['remove']) && !empty($amount['amount'])) {
        $sets['recurring'][] = $amount;
      }
    }
  }

  $data = array(
    'gid' => $values['gid'],
    'status' => (int) $values['statuses'],
    'default_amt' => '',
    'price_set' => $sets,
  );

  drupal_write_record('fundraiser_fund_group_overrides', $data);

}

/**
 * @param array $amount_values
 *   The array of prices and labels.
 * @param int $default_amount
 *   The default amount.
 * @param int $weight
 *   The table row weight in the form widget.
 *
 * @return mixed
 *   A renderable form array.
 */
function _designation_ask_form($amount_values, $default_amount, $weight) {
  $form['default_amount'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($default_amount) ? ($default_amount == $amount_values[0]) : FALSE,
  );
  $form['amount'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($amount_values[0]) ? $amount_values[0] : '',
    '#size' => 10,
  );
  $form['label'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($amount_values[1]) ? $amount_values[1] : '',
    '#size' => 30,
  );
  $form['remove'] = array(
    '#type' => 'checkbox',
    '#return_value' => 1,
    '#default_value' => 0,
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => $weight,
    '#delta' => 10,
    '#title_display' => 'invisible',
  );

  return $form;
}