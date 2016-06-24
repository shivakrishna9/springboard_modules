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
function fundraiser_designations_price_override_form(&$form, &$form_state, $product) {

  $data = !empty($product->fd_overrides) ? $product->fd_overrides : array();
  $form['#attached']['js'][] = drupal_get_path('module', 'fundraiser_designations') . '/js/price-sets.js';

  $form['override_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Override Ask Amounts'),
    '#weight' => -4.96,
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t('Ask amounts set here will override the ask amounts set on the form, for this designation only.'),
  );

  $form['override_settings']['statuses'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#weight' => -10,
    '#default_value' => isset($data['status']) ? $data['status'] : 0,
    '#decription' => t('Set prices that will override the fund group defaults if this item is selected.'),
  );

  $form['override_settings']['amount_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ask amounts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div class="clear-block" id="ask-wrapper">',
    '#suffix' => '</div>',
    '#states' => array(
      'visible' => array(
        ':input[name="statuses"]' => array('checked' => TRUE),
      ),
    ),
    '#tree' => TRUE,
  );

  $form['override_settings']['amount_wrapper']['donation_amounts'] = array(
    '#prefix' => '<div id="ask-amounts">',
    '#suffix' => '</div>',
    '#theme' => 'fundraiser_ask_amounts',
    '#tree' => TRUE,
  );

  // AJAXy bits.
  $donation_amounts = array();
  $default_amount = NULL;
  // The last submission takes precedence.
  if (isset($form_state['values']['amount_wrapper']['donation_amounts'])) {
    foreach ($form_state['values']['amount_wrapper']['donation_amounts'] as $index => $values) {
      $donation_amounts[$values['weight']] = array($values['amount'], $values['label']);
    }
    ksort($donation_amounts);
  }
  else {
    $price_set = [];
    if (!empty($data['price_set'])) {
      $amounts = unserialize($data['price_set']);
      foreach ($amounts as $amount) {
        $price_set[] = array($amount['amount'], $amount['label']);
        if (!empty($amount['default_amount'])) {
          $default_amount = $amount['amount'];
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
      $form['override_settings']['amount_wrapper']['donation_amounts'][$index] = _designation_ask_form($donation_amount, $default_amount, $weight);
    }
    ++$index;
  }

  // AJAX fix: reorder donation amounts in $form_state['input'] by weight.
  // Corrects the display order of donation amounts when an ajax event is
  // triggered.
  if (isset($form_state['input']['amount_wrapper']['donation_amounts'])) {
    $old_donation_amounts = $form_state['input']['amount_wrapper']['donation_amounts'];
    unset($form_state['input']['amount_wrapper']['donation_amounts']);
    foreach ($old_donation_amounts as $donation_amount) {
      $new_donation_amounts[$donation_amount['weight']] = $donation_amount;
    }
    ksort($new_donation_amounts);
    foreach ($new_donation_amounts as $donation_amount) {
      $form_state['input']['amount_wrapper']['donation_amounts'][] = $donation_amount;
    }
  }

  $form['override_settings']['amount_wrapper']['amount_more'] = array(
    '#type' => 'button',
    '#submit' => array(),
    '#validate' => array('fundraiser_designations_override_form_button_validate'),
    '#value' => t('Add another'),
    '#description' => t('Click here to add more choices.'),
    '#ajax' => array(
      'callback' => '_fundraiser_designations_settings_form_amounts',
      'wrapper' => 'ask-amounts',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form['actions']['submit']['#submit'][] = 'fundraiser_designations_override_form_submit';

}

/**
 * Ajax replacement.
 *
 * @param array $form
 * @param array $form_state
 *
 * @return mixed
 */
function _fundraiser_designations_settings_form_amounts(&$form, &$form_state) {
  return $form['override_settings']['amount_wrapper']['donation_amounts'];
}

/**
 * Implements hook_form_validate().
 */
function fundraiser_designations_override_form_button_validate(&$form, &$form_state) {
  if (form_get_errors()) {
    form_clear_error();
    drupal_get_messages('error');
  }
}

/**
 * Implements hook_form_submit().
 */
function fundraiser_designations_override_form_submit($form, &$form_state) {

  $values = $form_state['values'];
  $sets = [];
  $values['pid'] = $form_state['commerce_product']->product_id;

  db_delete('fundraiser_designations_overrides')
    ->condition('pid', $values['pid'], '=')
    ->execute();

  foreach ($values['amount_wrapper']['donation_amounts'] as $amount) {
    if (empty($amount['remove']) && !empty($amount['amount'])) {
      $sets[] = $amount;
    }
  }

  $data = array(
    'pid' => $values['pid'],
    'status' => (int) $values['statuses'],
    'default_amt' => '',
    'price_set' => $sets,
  );

  drupal_write_record('fundraiser_designations_overrides', $data);

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
