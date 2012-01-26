<?php

/**
 * Index callback for springboard service.
 */
function springboard_api_list_webforms($pubkey, $timestamp, $checksum, $page, $pagesize, $override_400 = FALSE) {
  unset($_SESSION['messages']);
  $data = array();
  $error_code = (isset($submission['override_400']) && $submission['override_400']) ? 200 : 400;
  $valid = _springboard_api_validate_transaction('index', $timestamp, $pubkey, $checksum);
  
  if ($valid) {
    $webform_node_types = implode("','", variable_get('webform_node_types', ''));
    $sql = sprintf("SELECT nid, title, type FROM {node} WHERE type IN ('%s') LIMIT %d, %d", $webform_node_types, $page, $pagesize);
    $results = db_query($sql);

    while ($result = db_fetch_object($results)) {
      $webforms[] = array('nid' => $result->nid, 'type' => $result->type, 'title' => $result->title);
    }
    return $webforms;
  }
  else {
    services_error('Error: Access denied.', $error_code, t('Your submission failed validation.'));
  }
}

/**
 * Retrieve callback for springboard service.
 *
 * @param $nid
 * Node id of the requested webform.
 * 
 * @return
 * Returns an array of field definitions for the requested webform.
 */
function springboard_api_get_webform($pubkey, $timestamp, $checksum, $nid, $override_400 = FALSE) {
  global $user;
  $error_code = (isset($submission['override_400']) && $submission['override_400']) ? 200 : 400;
  unset($_SESSION['messages']);
  $nid = (int) $nid;
  $webform = array();
  $fields = array();
  $valid = _springboard_api_validate_transaction('retrieve', $timestamp, $pubkey, $checksum);
  if ($valid) {
    $cache_user = $user;
    $uid = db_result(db_query("SELECT uid FROM {springboard_api} WHERE pubkey='%s'", $pubkey));
    $user = user_load(array('uid' => $uid));
    $webform_node_types = variable_get('webform_node_types', '');
    $node = node_load(array('nid' => $nid));

    
    if (!is_numeric($nid) || $nid == 0) {
      watchdog('springboard_api', t('Error: nid missing or invalid'), array(), WATCHDOG_ERROR);
      services_error('Error: webform id missing or invalid', $error_code, t('The webform id is either missing or invalid (not an integer).'));
    }

    if (!in_array($node->type, $webform_node_types)) {
      watchdog('springboard_api', t('Invalid resource requested.'), array(), WATCHDOG_ERROR);
      services_error('Error: Invalid id', $error_code, t('The webform id submitted does not match any webforms on the site.'));
    }

    if (!node_access('view', $node)) {
      watchdog('springboard_api', t('Access denied to @node for @user.', array('@node' => $node->title, '@user'=> $user->name)), array(), WATCHDOG_ERROR);
      services_error(t('Error:Access denied to @node for @user.', array('@node' => $node->title, '@user'=> $user->name)), $error_code, t('Your API account does not have access to retrieve the requested webform.'));
    }

    $webform['nid'] = $node->nid;
    $webform['type'] = $node->type;
    $webform['title'] = $node->title;
    
  
    $webform['components'] = _springboard_api_get_webform_components($node->nid);
    $user = $cache_user;
    return $webform;
  }
  else {

    services_error('Error: Access denied.', $error_code, t('Your submission failed validation.'));
  }
}

/**
 * Process a donation submitted via the API. 
 *
 * @param $submission
 * Array of webform values.
 */
function springboard_api_submit_webform($submission) {
  unset($_SESSION['messages']);
  $valid = _springboard_api_validate_transaction('submit', $submission['time'], $submission['pubkey'], $submission['checksum']);
  $error_code = (isset($submission['override_400']) && $submission['override_400']) ? 200 : 400;
  if ($valid) {

    springboard_api_validate_donation($submission, $error_code);

    //$submission = NULL;
    $sid = $submission['sid'] ? (int) $submission['sid'] : NULL;

    $node = node_load(array('nid' => trim($submission['webform_id'])));

    // Start with some defaults.
    $form_state = array(
      'storage' => '',
      'submitted' => 1,
      'values' => array(
        'op' => 'Submit',
        'submit' => 'Submit',
        'form_id' => 'webform_client_form_' . $submission['webform_nid'],
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
      ORDER BY parent_key', $submission['webform_id'], $submission['webform_id']);

    // FIXME: selects flake on numeric inputs due to incorrect default typecasting of submitted values by PHP. trim() helps but doesn't solve the problem.
    while ($field = db_fetch_object($results)) {
      $submission[$field->form_key] = trim($submission[$field->form_key]);

      if ($field->parent_key == NULL) {
          $save = &$form_state['values']['submitted'];
      }
      else {
        $save = &$form_state['values']['submitted'][$field->parent_key];
      }

      switch ($field->type) {
        case 'fieldset';
          $form_state['values']['submitted'][$field->form_key] = array();
          break;
        case 'email':
        case 'textfield':
          $save[$field->form_key] = $submission[$field->form_key];
          break;
        case 'select':
          // FIXME: hack workaround for the recurring payment field
          if ($field->form_key == 'recurs_monthly') {
            $save[$field->form_key] = array(
              'recurs' => $submission[$field->form_key],
            );
            break;
          }
          $extra = unserialize($field->extra);
          if (!strcmp($extra['multiple'],'Y')) {
            $extra['multiple'] = 1;
          }
          if (!isset($extra['multiple']) || $extra['multiple'] == 0) {
            $save[$field->form_key] = $submission[$field->form_key];
          }
          else {
          
          $save[$field->form_key] = array();
          $submission[$field->form_key] = explode(',', $submission[$field->form_key]);

            if (preg_match("/\n/", $extra['items'])) {
              $extra['items'] = explode("\n", $field->extra['items']);
            }
            else {
              $extra['items'] = array($extra['items']);
            }

            foreach ($extra['items'] as $item) {
              $item = preg_replace('/\|.*$/', '', $item);

              if (in_array($item, $submission[$field->form_key])) {
                $save[$field->form_key][$item] = $item; 
              }
              else {
                $save[$field->form_key][$item] = '';
              }
            }
          
            foreach ($submission[$field->form_key] as $item) {
              $item = trim($item);
              $save[$field->form_key][$item] = $item;
            }
          }
          break;
        case 'date':
          $date = explode('/', $submission[$field->form_key]);
          $save[$field->form_key] = array(
            'month' => $date[0],
            'day' => $date[1],
            'year' => $date[2],
          );
          break;
      }
    }

    drupal_execute('webform_client_form_' . $submission['webform_nid'], $form_state, $node, $submission, FALSE, FALSE);
    $errors = form_get_errors();
    if (!$errors) {
      return $_SESSION['messages'];
    }
    else {
      foreach($errors as $key => $message) {
        $key = explode('][', $key);
        $item['field_key'] = str_replace(']', '', $key[count($key) -1]);
        $item['message'] = $message;
        $formatted_errors['errors'][] = $item;
      }
      services_error('Error: One or more problems where detected with your submission.', $error_code, $formatted_errors);
    }

  }
  else {
    watchdog('springboard_api', t('Invalid submission sent from %ip', array('%ip' => $_SERVER['REMOTE_ADDR'])), array(), WATCHDOG_ERROR);
    services_error('Error: Access denied.', $error_code, t('Your submission failed validation.'));
  }
}


/**
 * Validate donation array for required values.
 * 
 * @param $donation
 * Keyed array of submitted donation values.
 *
 */
function springboard_api_validate_donation($donation = array(), $error_code) {
   unset($_SESSION['messages']);
   $node_type = db_result(db_query('SELECT type FROM {node} WHERE nid = %d', $donation['webform_id']));
   
  // TODO: replace this with a function call that ascertains required fields for this webform.
  $results = db_query('SELECT form_key FROM {webform_component} WHERE nid=%d AND mandatory = 1', $donation['webform_id']);
  $donation_keys = array_keys($donation);
  while ($form_key = db_result($results)) {

    if(!in_array($form_key, $donation_keys)) {
   
      $missing_fields[] = $form_key;
    }
  }
  unset($form_key);
  
  if(count($missing_fields)) {
    drupal_set_message(t('The following required fields where missing values: !values', array('!values' => implode(', ', $missing_fields))));
  }
  
  $results = db_query('SELECT form_key FROM {webform_component} WHERE type = "date" AND nid=%d', $donation['webform_id']);
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
    return services_error('Error: one or more required fields are missing.', $error_code, $_SESSION['messages']);
  }
}

/**
 * 
 */
function _springboard_api_list_order_ids() {
  $results = db_query('SELECT fwo.order_id FROM {springboard_webform_order} fwo UNION SELECT fr.order_id FROM {springboard_recurring} fr');
  while ($order_id = db_result($results)) {
    $order_ids[] = $order_id;
  }
  return $order_ids;
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
function _springboard_api_get_webform_components($nid) {
  $results = db_query('SELECT 
      form_key as field_id,
      name,
      type,
      value as default_value,
      mandatory as required,
      extra as details
    FROM {webform_component} 
    WHERE nid = %d', $nid);
  while ($field = db_fetch_array($results)) {

    $field['details'] = unserialize($field['details']);
    
    if ($field['type'] == 'select') {
      $items = array();
      $items_raw = explode("\n", $field['details']['items']);
      foreach ($items_raw as $item) {
        $item = explode('|', $item);
        $items[] = array('key' => $item[0], 'value' => $item[1]);
      }
      $field['details']['items'] = $items;
    }
    $fields[] = $field;
  }
  return $fields;
}
