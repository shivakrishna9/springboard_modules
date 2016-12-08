<?php

/**
 * @file
 * Drush utility to retrieve payment tokens from a set of account data.
 *
 * Sends data from a source .csv file to Sage Vault, and combines the returned
 * data with the original data in a new .csv file in the same directory.
 */

/**
 * Implements hook_drush_help().
 */
function commerce_sage_payments_drush_help($command) {
  switch ($command) {
    case 'drush:sage-tokenizer':
      return dt('Generates sage payment tokens from an uploaded csv file.');
  }
}

/**
 * Implements hook_drush_command().
 *
 * Utility to retrieve a payment token from Sage Vault for accounts
 * listed in a csv file in a secure location.
 */
function commerce_sage_payments_drush_command() {
  $items['sage-tokenizer'] = array(
    'description' => 'Obtain sage payment tokens using a source file',
    'arguments' => array(
      'gateway_id' => "The gateway id. Ex: 'commerce_sage_payments_eft|commerce_payment_commerce_sage_payments_eft' Use quotes.",
      'path' => 'Absolute path to the source file directory',
      'filename' => 'The filename',
    ),
  );
  return $items;
}

/**
 * Token retrieval callback.
 *
 * If input parameters are missing, they will be prompted for.
 *
 * @param string $gateway_id
 *   The gateway ID, ex:
 *   commerce_sage_payments_eft|commerce_payment_commerce_sage_payments_eft.
 * @param string $path
 *   The absolute path to the directory where the file is.
 * @param string $filename
 *   The filename, including extension.
 */
function drush_commerce_sage_payments_sage_tokenizer($gateway_id = NULL, $path = NULL, $filename = NULL) {

  // No initial input parameter.
  if (empty($gateway_id)) {
    $fundraiser_gateways = commerce_sage_payments_find_gateways();
    $gateway_id = drush_choice($fundraiser_gateways, dt('Choose the Sage EFT gateway to use.'));
    if (empty($gateway_id)) {
      // User chose "cancel'.
      drush_print('Operation cancelled.');
      return;
    }
    $gateway = _fundraiser_gateway_info($gateway_id);
  }
  else {
    // We have input param.
    $gateway = _fundraiser_gateway_info($gateway_id);
    if (empty($gateway)) {
      // It was a bad input param.
      drush_print('Gateway not found. Try again.');
      // Try again.
      $fundraiser_gateways = commerce_sage_payments_find_gateways();
      $gateway_id = drush_choice($fundraiser_gateways, dt('Choose the Sage EFT gateway to use.'));
      if (empty($gateway_id)) {
        // User chose "cancel'.
        drush_print('Operation cancelled.');
        return;
      }
      $gateway = _fundraiser_gateway_info($gateway_id);
    }
  }

  if (empty($gateway)) {
    // Ya never know...
    drush_print('Gateway not found. Giving up.');
    return;
  }

  if (empty($path)) {
    $path = drush_prompt("Enter the absolute path to the directory where the csv file is located.", NULL, TRUE);
  }

  if (empty($filename)) {
    $filename = drush_prompt("Enter the filename of the .csv file", NULL, TRUE);
  }

  $source_file = $path . '/' . $filename;

  if (!file_exists($source_file)) {
    drush_print('Cannot find your csv file.');
    return;
  }

  $accounts = commerce_sage_payments_load_accounts_from_csv($source_file);

  $chunks = array_chunk($accounts['accounts'], 100);
  $operations = array();
  foreach ($chunks as $chunk) {
    $operations[] = array(
      'commerce_sage_payments_tokenizer_batch_process',
      array(
        $chunk,
        $gateway,
        $accounts['headers'],
        $path,
      ),
    );
  }

  $batch = array(
    'operations' => $operations,
    'title' => t('Creating missing webform users'),
    'init_message' => t('Initializing'),
    'error_message' => t('An error occurred'),
    'finished' => 'commerce_sage_payments_tokenizer_batch_process_complete',
  );

  batch_set($batch);
  $batch = &batch_get();
  $batch['progressive'] = FALSE;

  drush_backend_batch_process();

}

/**
 * Batch process the account data.
 *
 * @param array $accounts
 *   The accounts loaded from csv file.
 * @param array $gateway
 *   The gateway settings array.
 * @param array $headers
 *   The csv headers.
 * @param string $path
 *   The path to the directory containg the csv file.
 * @param array $context
 *   The context aggragator.
 */
function commerce_sage_payments_tokenizer_batch_process($accounts, $gateway, $headers, $path, &$context) {

  foreach ($accounts as $index => $account) {
    $payment_details = array(
      'ROUTING_NUMBER' => $account['routing_number'],
      'ACCOUNT_NUMBER' => $account['account_number'],
      'C_ACCT_TYPE' => $account['account_type'],
    );

    $result = commerce_sage_payments_vault_request_insert($gateway['gateway_details'], $payment_details);

    if (!empty($result['GUID'])) {
      $accounts[$index]['guid'] = $result['GUID'];
    }
    else {
      $accounts[$index]['guid'] = '';
    }
  }

  $context['results']['path'] = $path;
  $context['results']['headers'] = $headers;
  $context['results'][] = $accounts;

}

/**
 * Create a new csv file with updated data including payment token.
 */
function commerce_sage_payments_tokenizer_batch_process_complete($success, $results, $operations) {

  if ($success) {
    $updated_accounts = [];
    $headers = $results['headers'];
    $path = $results['path'];
    unset($results['headers']);
    unset($results['path']);

    foreach ($results as $chunk) {
      foreach ($chunk as $account) {
        $updated_accounts[] = $account;
      }
    }

    array_push($headers, 'GUID');
    array_unshift($updated_accounts, $headers);
    $file = fopen($path . "/destination.csv", "w");
    foreach ($updated_accounts as $line) {
      fputcsv($file, $line);
    }
    fclose($file);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
    drush_print($message);
  }
}


/**
 * @param $path
 * @return array
 */
function commerce_sage_payments_load_accounts_from_csv($path) {
  $file = file($path);
  $accounts = [];
  if (!empty($file)) {
    $accounts = array_map('str_getcsv', $file);
    array_walk($accounts, function (&$a) use ($accounts) {
      $a = array_combine($accounts[0], $a);
    });
    $headers = array_shift($accounts);
  }
  return array('accounts' => $accounts, 'headers' => $headers);
}

/**
 * @return array
 */
function commerce_sage_payments_find_gateways() {
  $methods = new stdClass();
  $methods->payment_methods = array();
  rules_invoke_all('commerce_payment_methods', $methods);
  $fundraiser_gateways = array();
  foreach ($methods->payment_methods as $id => $info) {
    if (strpos($id, 'sage') !== FALSE) {
      $fundraiser_gateways[$id] = $id;
    }
  }
  return $fundraiser_gateways;
}