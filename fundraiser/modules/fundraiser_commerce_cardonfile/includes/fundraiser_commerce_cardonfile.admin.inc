<?php
/**
 * @file
 * Fundraiser Commerce Cardonfile admin functions.
 */

define('FUNDRAISER_COMMERCE_CARDONFILE_CARD_ENTRIES_THRESHOLD', 1000);

/**
 * Admin settings form.
 */
function fundraiser_commerce_cardonfile_admin_settings($form, $form_state) {
  /*$form['fundraiser_commerce_cardonfile_patch_cardonfile'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable separate billing addresses for unique cards'),
    '#description' => t("Checking this will enable functionality to separate the user's regular address (stored in the user table) from their stored card's billing address, a feature the commerce_cardonfile module does not yet currently support."),
    '#default_value' => variable_get('fundraiser_commerce_cardonfile_patch_cardonfile', FALSE),
  );*/

  // Consider removing.
  /*$form['fundraiser_commerce_cardonfile_update_on_view'] = array(
    '#type' => 'checkbox',
    '#title' => t("Set a donation's card id record when that donation is viewed."),
    '#description' => t(''),
    '#default_value' => variable_get('fundraiser_commerce_cardonfile_update_on_view', FALSE),
  );*/

  // $form['#submit'][] = '_fundraiser_commerce_cardonfile_patch_cardonfile';

  return system_settings_form($form);
}

/**
 * Submission handler.
 */
function _fundraiser_commerce_cardonfile_patch_cardonfile($form, &$form_state) {
  if (!db_table_exists('fundraiser_commerce_cardonfile')) {
    db_create_table('fundraiser_commerce_cardonfile', array(
      'description' => 'Records billing records for different cardonfile records',
      'fields' => array(
        'donation_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'card_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'profile_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      'indexes' => array(
        'donation_id' => array('donation_id'),
        'card_id' => array('card_id'),
        'profile_id' => array('profile_id'),
      ),
      'unique keys' => array('card_id'),
    ));
  }

  _fundraiser_commerce_cardonfile_update_cards($form, $form_state);
}

/**
 *
 */
function _fundraiser_commerce_cardonfile_update_cards($form, &$form_state) {
  $limit = 25;

  // Retrieve the next group of donations to update.
  $result = db_select('fundraiser_donation', 'fd')
    ->fields('fd', array('did', 'uid'))
    ->orderBy('fd.did', 'ASC')
    ->condition('fd.did', $sandbox['current_donation'], '>')
    ->condition('fd.status', 'payment_received', '=')
    ->isNull('card_id')
    ->range(0, $limit)
    ->execute()
    ->fetchAll();

  foreach ($result as $row) {
    $card_id = NULL;

    if (empty($cards[$row->uid])) {
      $user_cards = commerce_cardonfile_load_multiple_by_uid($row->uid);
    }
    else {
      $user_cards = $cards[$row->uid];
    }
    $cards_n = count($user_cards);

    if ($cards_n == 1) {
      // If the user only has one card, we can just set it on the donation
      // record right away.
      $card = array_shift($user_cards);
      $card_id = $card->card_id;
    }
    else {
      // Otherwise we'll need to grab the card_id from the order record.
      $order = commerce_order_load($row->did);
      $data = _fundraiser_commerce_order_data_retrieve($order->data['fundraiser_commerce']);
      $card_id = $data['cardonfile'];
    }

    if (empty($card_id)) {
      // Worst case time-wise, since the card_id wasn't set in the order for
      // some reason, we'll need to try a last-ditch effort to load the donation
      // and get the card_id from a fundraiser_commerce function.
      $donation = fundraiser_donation_get_donation($row->did);
      $card = _fundraiser_commerce_donation_cardonfile_card($donation);
      if (!empty($card)) {
        $card_id = $card->card_id;
      }
    }

    if (empty($card_id) || !is_numeric($card_id)) {
      // Nothing found, we'll have to skip this donation.
      watchdog('fundraiser', 'No card was found for donation @did while updating {fundraiser} for update 7011.', array(
        '@did' => $row->did,
      ), WATCHDOG_NOTICE);
      continue;
    }

    db_query('UPDATE {fundraiser_donation} SET card_id = :card_id WHERE did = :did', array(
      ':card_id' => $card_id,
      ':did' => $row->did,
    ));
  }

  $number_of_cards = db_query('SELECT COUNT(card_id) FROM {commerce_cardonfile}')->fetchField();
  if ($number_of_cards > FUNDRAISER_COMMERCE_CARDONFILE_CARD_ENTRIES_THRESHOLD) {

  }
}

/**
 * Submission handler for fundraiser_quick_donate_admin_settings().
 */
function fundraiser_quick_donate_admin_settings_submit($form, &$form_state) {
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, FUNDRAISER_QUICK_DONATE_DEFAULT_FORM_PREFIX) === FALSE || empty($value)) {
      continue;
    }
    $match = preg_match('/.*\[([0-9]+)\].*/', $value, $matches);
    if (!$match || $match !== FALSE && count($matches) < 2) {
      if (!is_numeric($value)) {
        continue;
      }

      // Try to get the title based on just the node ID.
      $query = db_query('SELECT title FROM {node} WHERE nid = :nid', array(
        ':nid' => $value,
      ))->fetchField();

      if (empty($query)) {
        continue;
      }

      $form_state['values'][$key] = $query . ' [' . $value . ']';
      $matches[1] = $value;
    }
    $data = array(
      'title' => $form_state['values'][$key],
      'nid' => $matches[1],
    );
    $form_state['values'][$key] = serialize($data);
  }
  $form_state['values']['fundraiser_quick_donate_default_help_message_format'] = $form_state['values']['fundraiser_quick_donate_default_help_message']['format'];
  $form_state['values']['fundraiser_quick_donate_default_help_message'] = $form_state['values']['fundraiser_quick_donate_default_help_message']['value'];
}
