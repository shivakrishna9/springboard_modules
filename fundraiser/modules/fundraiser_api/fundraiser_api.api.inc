<?php

/**
 * Index callback for fundraiser service.
 */
function fundraiser_api_list_webforms($page, $fields, $pagesize, $groupid = NULL, $term = NULL) {

  $webform_node_types = implode("','", variable_get('webform_node_types', ''));

  $sql = sprintf("SELECT nid, title, type FROM {node} WHERE type IN ('%s') LIMIT %d, %d", $webform_node_types, $page, $pagesize);
  $results = db_query($sql);
  while ($result = db_fetch_object($results)) {
    $webforms[] = array('nid' => $result->nid, 'type' => $result->type, 'title' => $result->title);
  }
  return $webforms;
}

/**
 * Retrieve callback for fundraiser service.
 *
 * @param $nid
 * Node id of the requested webform.
 *
 * @return
 * Returns an array of field definitions for the requested webform.
 */
function fundraiser_api_get_webform($nid) {

  $nid = (int) $nid;
  global $user;
  $webform = array();
  $fields = array();
  $webform_node_types = variable_get('webform_node_types', '');
  $node = node_load(array('nid' => $nid));

  // Return an error if
  if (!is_numeric($nid) || !in_array($node->type, $webform_node_types) || !node_access('view', $node)) {
    watchdog('webform_api', t('Access denied to %node for %user.', array('%node' => l(t('$node->title'), 'node/' . $node->nid), '%user', l($user->name, 'user/' . $user->uid))), WATCHDOG_ERROR);
    return services_error('Access denied.');
  }

  $webform = (array) $node;

  $webform['components'] = _fundraiser_api_get_webform_components($node->nid);

  return $webform;
}




/**
 * TODO: work in support for fields, webformid, status and uid
 */
function fundraiser_api_list_donations($page, $fields, $pagesize, $webformid = NULL, $status = NULL, $uid = NULL) {
  $order_ids = implode(',' . _fundraiser_api_list_order_ids());
  $results = db_query('SELECT
      fwo.order_id,
      fwo.webform_nid,
      fwo.sid,
      fwo.sfid,
      fwo.gateway,
      fwo.txn_id,
      fwo.recurring_status,
      uo.uid,
      uo.order_status,
      uo.order_total,
      uo.primary_email,
      uo.billing_first_name,
      uo.billing_last_name,
      uo.created
    FROM {uc_orders} uo
    INNER JOIN {fundraiser_webform_order} fwo
      ON uo.order_id = fwo.order_id
    LIMIT %d, %d',
    $page,
    $pagesize
  );
  while ($result = db_fetch_array($results)) {
    $orders[] = $result;
  }
  return $orders;
}

/**
 * Retrieve callback for Fundraiser donation API
 *
 * @param $oid
 * Ubercart Order ID
 *
 * @return
 * returns an array of order fields.
 */
function fundraiser_api_get_donation($oid) {
  if (is_numeric($oid)) {

    $sql = sprintf('
      SELECT
        fwo.order_id,
        fwo.webform_nid,
        fwo.sid,
        fwo.sfid,
        fwo.gateway,
        fwo.txn_id,
        fwo.recurring_status,
        uo.uid,
        uo.order_status,
        uo.order_total,
        uo.primary_email,
        uo.billing_first_name as first_name,
        uo.billing_last_name as last_name,
        uo.billing_street1 as address_1,
        uo.billing_street2 as address_2,
        uo.billing_city as city,
        uo.billing_zone as state,
        uo.billing_postal_code as postal_code,
        uo.billing_country as country,
        uo.payment_method,
        uo.created,
        uo.modified
    FROM {fundraiser_webform_order} fwo
    INNER JOIN {uc_orders} uo
      ON uo.order_id = fwo.order_id
    WHERE fwo.order_id = %d
    ', $oid);

    $result = db_query($sql);
    $order = db_fetch_array($result);
    if (isset($order['order_id'])) {
      return $order;
    }
    else {
      return services_error('Order id ' . $oid . ' not found', 404);
    }
  }
  else {
    return services_error('order id must be numeric, "' . $oid . '" not found', 404);
  }
}

/**
 * Process a donation submitted via the API.
 *
 * @param $donation
 * Array of donation form values.
 */
function fundraiser_api_create_donation($donation) {

  global $user;
  fundraiser_api_validate_donation($donation);
  // return $donation;
  // }
  global $user;
  // TODO: add detection & conversion logic to translate country values (name or code) to numeric value.
  // TODO: convert state input to numeric code.

  $submission = NULL;
  $sid = $donation['sid'] ? (int) $donation['sid'] : NULL;

  $node = node_load(array('nid' => $donation['webform_nid']));

  // Start with some defaults.
  $form_state = array(
    'storage' => '',
    'submitted' => 1,
    'values' => array(
      'op' => 'Submit',
      'submit' => 'Submit',
      'form_id' => 'webform_client_form_' . $donation['webform_nid'],
      'process_input' => 1,
    ),

  );


  $results = db_query('
    SELECT
      wc.cid,
      wc.form_key,
      wc.type,
      wc.extra,
      wc2.form_key as parent_key
    FROM {webform_component} wc
    LEFT JOIN {webform_component} wc2
      ON wc2.cid = wc.pid
      AND wc2.nid = %d
    WHERE wc.nid = %d
    ORDER BY parent_key', $donation['webform_nid'], $donation['webform_nid']);

  // FIXME: selects flake on numeric inputs due to incorrect default typecasting of submitted values by PHP. trim() helps but doesn't solve the problem.
  while ($field = db_fetch_object($results)) {
    $donation[$field->form_key] = trim($donation[$field->form_key]);

    if ($field->parent_key == NULL) {
      $save = &$form_state['values']['submitted'];
    }
    else {
      $save = &$form_state['values']['submitted'][$field->parent_key];
    }
    switch ($field->type) {
      case 'fieldset':
        $form_state['values']['submitted'][$field->form_key] = array();
        break;
      case 'email':
      case 'textfield':
        $save[$field->form_key] = $donation[$field->form_key];
        break;
      case 'select':
        // FIXME: hack workaround for the recurring payment field
        if ($field->form_key == 'recurs_monthly') {
          $save[$field->form_key] = array(
            'recurs' => $donation[$field->form_key],
          );
          break;
        }
        $extra = unserialize($field->extra);
        if (!strcmp($extra['multiple'], 'Y')) {
          $extra['multiple'] = 1;
        }
        if (!isset($extra['multiple']) || $extra['multiple'] == 0) {
          $save[$field->form_key] = $donation[$field->form_key];
        }
        else {

          $save[$field->form_key] = array();
          $donation[$field->form_key] = explode(',', $donation[$field->form_key]);

          if (preg_match("/\n/", $extra['items'])) {
            $extra['items'] = explode("\n", $field->extra['items']);
          }
          else {
            $extra['items'] = array($extra['items']);
          }

          foreach ($extra['items'] as $item) {
            $item = preg_replace('/\|.*$/', '', $item);

            if (in_array($item, $donation[$field->form_key])) {
              $save[$field->form_key][$item] = $item;
            }
            else {
              $save[$field->form_key][$item] = '';
            }
          }


          foreach ($donation[$field->form_key] as $item) {
            $item = trim($item);
            $save[$field->form_key][$item] = $item;
          }
        }
        break;
      case 'date':
        $date = explode('/', $donation[$field->form_key]);
        $save[$field->form_key] = array(
          'month' => $date[0],
          'day' => $date[1],
          'year' => $date[2],
        );
        break;
    }

  }


  drupal_execute('webform_client_form_' . $donation['webform_nid'], $form_state, $node, $submission, FALSE, FALSE);
  return $_SESSION['messages'];
}

/**
 *
 */
function _fundraiser_api_list_order_ids() {
  $results = db_query('SELECT fwo.order_id FROM {fundraiser_webform_order} fwo UNION SELECT fr.order_id FROM {fundraiser_recurring} fr');
  while ($order_id = db_result($results)) {
    $order_ids[] = $order_id;
  }
  return $order_ids;
}

/**
 * Validate donation array for required values.
 *
 * @param $donation
 * Keyed array of submitted donation values.
 *
 * @param $op
 * Optional operation
 */
function fundraiser_api_validate_donation($donation = array(), $op = 'create') {
  unset($_SESSION['messages']);
  $node_type = db_result(db_query('SELECT type FROM {node} WHERE nid = %d', $donation['webform_nid']));
  if (strcmp($node_type, 'donation_form')) {
    drupal_set_message(t('The webform node id (webform_nid) is missing or incorrect.'));
  }

  // TODO: replace this with a function call that ascertains required fields for this webform.
  $results = db_query('SELECT form_key FROM {webform_component} WHERE nid=%d AND mandatory = 1', $donation['webform_nid']);
  $donation_keys = array_keys($donation);
  while ($form_key = db_result($results)) {

    if (!in_array($form_key, $donation_keys)) {
      $missing_fields[] = $form_key;
    }
  }
  unset($form_key);

  if (count($missing_fields)) {
    drupal_set_message(t('The following required fields where missing values: !values', array('!values' => implode(', ', $missing_fields))));
  }

  $results = db_query('SELECT form_key FROM {webform_component} WHERE type = "date" AND nid=%d', $donation['webform_nid']);
  while ($form_key = db_result($results)) {
    $date = explode('/', $donation[$form_key]);
    if (count($date) != 3) {
      drupal_set_message(t('Incorrect date format for !field: !value. Date values must be submitted in M/D/Y format.', array('!field' => $form_key, '!value' => $donation[$form_key])));
    }
    if (strpos($date[0], '0') === 0 || strpos($date[1], '0') === 0) {
      drupal_set_message(t('Incorrect date format for !field: month and day values must not include leading zeroes.', array('!field' => $form_key)));
    }
  }
  if (count($_SESSION['messages'])) {
    return services_error('One or more problems where detected with your submission.', 0, $_SESSION['messages']);
  }
}


function fundraiser_api_update_donation($sid, $donation = array()) {
  if (!$donation['submission_id']) {
    $donation['submission_id'] = $sid;
  }

  fundraiser_api_validate_donation($donation, 'update');

  // Grind through the updates.

  return $_SESSION['messages'];

}

/**
 * Return an array of webform components for a given node.
 *
 * @param $nid
 * Node ID
 *
 * @return
 * Returns an array containing data from {webform_component}
 */
function _fundraiser_api_get_webform_components($nid) {
  $results = db_query('SELECT * FROM {webform_component} WHERE nid = %d', $nid);
  while ($field = db_fetch_array($results)) {
    $fields[] = $field;
  }
  return $fields;
}
