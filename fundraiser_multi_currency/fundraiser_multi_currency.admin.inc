<?php
/**
 * @file
 * Adds administrative support for currency.
 */

/**
 * Menu callback, main administration page.
 */
function fundraiser_multi_currency_admin() {
  $headers = array(
    t('Symbol'),
    t('Code'),
    t('Name'),
    t('Operations'),
  );
  $currencies = fundraiser_multi_currency_get_currencies();
  $rows = array();
  foreach ($currencies as $currency) {
    $links = l(t('edit'), 'admin/config/system/currencies/edit/' . $currency['currency_id']) .
      ' | ' . l(t('delete'), 'admin/config/system/currencies/delete/' . $currency['currency_id']);
    $rows[] = array(
      $currency['symbol'],
      $currency['code'],
      $currency['name'],
      $links,
    );
  }
  if (empty($table_rows)) {
    $rows[] = array(array(
      'data' => t('No currencies have been created. Click the Add Currency link above to add your first.'),
      'colspan' => count($headers),
    ));
  }
  return theme('table', array('header' => $headers, 'rows' => $rows));
}

/**
 * Menu callback, currency add form. Calls the edit form without defaults.
 */
function fundraiser_multi_currency_add($form, $form_state) {
  if (!isset($form_state['values'])) {
    $form_state['values'] = array();
  }
  return fundraiser_multi_currency_currency_form($form_state['values']);
}

/**
 * Submit handler for the currency add form.
 */
function fundraiser_multi_currency_add_submit($form, &$form_state) {
  if (_fundraiser_multi_currency_save($form_state['values'])) {
    drupal_set_message(t('New currency added.'));
    $form_state['redirect'] = 'admin/config/system/currencies';
  }
}

/**
 * Menu callback, currency edit form. Calls the edit form with defaults.
 */
function fundraiser_multi_currency_edit($form, $form_state, $currency_id) {
  if (!isset($form_state['values'])) {
    $currency = fundraiser_multi_currency_get_currancy($currency_id);
    $form_state['values'] = (array) $currency;
  }
  if (!isset($form_state['values'])) {
    $form_state['values'] = array();
  }
  return fundraiser_multi_currency_currency_form($form_state['values']);
}

/**
 * Submit handler for the currency edit form.
 */
function fundraiser_multi_currency_edit_submit($form, &$form_state) {
  if (_fundraiser_multi_currency_save($form_state['values'])) {
    drupal_set_message(t('Currency updated.'));
    $form_state['redirect'] = 'admin/config/system/currencies';
  }
}

/**
 * Form to add or edit currencies.
 */
function fundraiser_multi_currency_currency_form($values) {
  $form = array();
  if (isset($values['currency_id']) && $values['currency_id'] > 0) {
    $form['currency_id'] = array(
      '#type' => 'value',
      '#value' => isset($values['currency_id']) ? $values['currency_id'] : '',
    );
  }
  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Symbol'),
    '#description' => t('Example: $, £, €, ¥.'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => isset($values['symbol']) ? $values['symbol'] : '',
  );
  $form['code'] = array(
    '#type' => 'textfield',
    '#title' => t('Code'),
    '#description' => t('The code for the currency type. Examples: USD, GBP, EUR, CAD, etc.'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => isset($values['code']) ? $values['code'] : '',
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Name for public display. Examples: Euro, UK Pound, US Dollar, etc.'),
    '#size' => 20,
    '#maxlength' => 255,
    '#default_value' => isset($values['name']) ? $values['name'] : '',
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Save a new or updated currency
 */
function _fundraiser_multi_currency_save($values) {
  $record = (object) $values;
  fundraiser_multi_currency_update_currancy($record);
  return TRUE;
}

/**
 * Delete form for currencies
 * Calls fundraiser_multi_currency_confirm_delete() to build the confirmation form
 */
function fundraiser_multi_currency_delete($currency_id = NULL) {
  if ($currency_id != NULL) {
    $currency = fundraiser_multi_currency_get_currancy($currency_id);
    if (isset($currency) && !empty($currency)) {
      return drupal_get_form('fundraiser_multi_currency_confirm_delete', $currency);
    }
    else {
      drupal_set_message(t('That currency does not exist.'), 'error');
      drupal_goto('admin/config/system/currencies');
    }
  }
}

/**
 * Confirmation form for deleting currency
 */
function fundraiser_multi_currency_confirm_delete($form, &$form_state, $currency) {
  $form = array();
  $form['#currency'] = $currency;
  return confirm_form(
    $form,
    t('Are you sure you want to delete the currency %name?', array('%name' => $currency->name)),
    'admin/config/currencies',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel'),
    'fundraiser_multi_currency_confirm_delete');
}

/**
 * Process fundraiser_multi_currency_confirm_delete form submissions.
 */
function fundraiser_multi_currency_confirm_delete_submit($form, &$form_state) {
  $currency = $form['#currency'];
  fundraiser_multi_currency_delete_currancy($currency->currency_id);
  drupal_set_message(t('The currency has been deleted.'));
  $form_state['redirect'] = "admin/config/system/currencies";
}
