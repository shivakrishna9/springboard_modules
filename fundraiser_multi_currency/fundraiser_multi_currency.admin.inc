<?php
/**
 * @file
 */
function fundraiser_multi_currency_list() {
  $table_columns = array(
    t('Symbol'),
    t('Code'),
    t('Name'),
    t('Operations'),
  );
  $table_rows = array();

  $result = db_query('SELECT * FROM {fundraiser_currencies}');
  while ($row = db_fetch_object($result)) {
    $links = l('edit', 'admin/settings/currencies/edit/' . $row->currency_id) . ' | ' . l('delete', 'admin/settings/currencies/delete/' . $row->currency_id);
    $table_rows[] = array(
      $row->symbol,
      $row->code,
      $row->name,
      $links,
    );
  }

  if (empty($table_rows)) {
    $table_rows[] = array(array('data' => t('No currencies have been created. Click the Add link above to add your first.'), 'colspan' => count($table_columns)));
  }

  return theme('table', $table_columns, $table_rows);
}

/**
 * Currency add form
 * Calls fundraiser_multi_currency_currency_form()
 */
function fundraiser_multi_currency_add($form_state) {
  return fundraiser_multi_currency_currency_form($form_state['values']);
}

/**
 * Currency edit form
 * Calls fundraiser_multi_currency_currency_form()
 */
function fundraiser_multi_currency_edit($form_state, $currency_id) {
  if (empty($form_state['values']) && $currency_id > 0) {
    $form_state['values'] = db_fetch_array(db_query('SELECT * FROM {fundraiser_currencies} WHERE currency_id = %d', $currency_id));
  }

  return fundraiser_multi_currency_currency_form($form_state['values']);
}


/**
 * Submit handler for the Currency Add form
 * Calls _fundraiser_multi_currency_save()
 */
function fundraiser_multi_currency_add_submit($form, &$form_state) {
  if (_fundraiser_multi_currency_save($form_state['values'])) {
    drupal_set_message(t('New currency added.'));
    $form_state['redirect'] = 'admin/settings/currencies';
  }
}

/**
 * Submit handler for the Currency Edit form
 * Calls _fundraiser_multi_currency_save()
 */
function fundraiser_multi_currency_edit_submit($form, &$form_state) {
  if (_fundraiser_multi_currency_save($form_state['values'])) {
    drupal_set_message(t('Currency updated.'));
    $form_state['redirect'] = 'admin/settings/currencies';
  }
}

/**
 * Form to add or edit currencies
 */
function fundraiser_multi_currency_currency_form($values) {
  $form = array();

  if ($values['currency_id'] > 0) {
    $form['currency_id'] = array(
      '#type' => 'value',
      '#value' => $values['currency_id']
    );
  }

  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Symbol'),
    '#description' => t('Example: $, £, €, ¥'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => $values['symbol'],
  );

  $form['code'] = array(
    '#type' => 'textfield',
    '#title' => t('Code'),
    '#description' => t('The code for the currency type. Examples: USD, GBP, EUR, CAD, etc.'),
    '#size' => 10,
    '#maxlength' => 255,
    '#default_value' => $values['code'],
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Name for public display. Examples: Euro, UK Pound, US Dollar, etc.'),
    '#size' => 20,
    '#maxlength' => 255,
    '#default_value' => $values['name'],
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
  $record = (object)$values;
  if ($record->currency_id > 0) {
    return drupal_write_record('fundraiser_currencies', $record, array('currency_id'));
  }
  else {
    return drupal_write_record('fundraiser_currencies', $record);
  }

}


/**
 * Delete form for currencies
 * Calls fundraiser_multi_currency_confirm_delete() to build the confirmation form
 */
function fundraiser_multi_currency_delete($currency_id = NULL) {
  $currency_details = db_fetch_object(db_query('SELECT currency_id, name FROM {fundraiser_currencies} WHERE currency_id = %d', $currency_id));
  if (is_object($currency_details) && is_numeric($currency_details->currency_id)) {
    return drupal_get_form('fundraiser_multi_currency_confirm_delete', $currency_details);
  }
  else {
    drupal_set_message(t('That currency does not exist.'), 'error');
    drupal_goto('admin/settings/currencies');
  }
}

/**
 * Confirmation form for deleting currency
 */
function fundraiser_multi_currency_confirm_delete(&$form_state, $currency_details) {
  $form = array();
  $form['#currency'] = $currency_details;
  return confirm_form(
    $form,
    t('Are you sure you want to delete the currency %name?', array('%name' => $currency_details->name)),
    'admin/settings/currencies',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel'),
    'fundraiser_multi_currency_confirm_delete');
}

/**
 * Process fundraiser_multi_currency_confirm_delete form submissions.
 */
function fundraiser_multi_currency_confirm_delete_submit($form, &$form_state) {
  $currency_details = $form['#currency'];
  db_query('DELETE FROM {fundraiser_currencies} WHERE currency_id = %d', $currency_details->currency_id);
  drupal_set_message(t('The currency has been deleted.'));
  $form_state['redirect'] = "admin/settings/currencies";
}