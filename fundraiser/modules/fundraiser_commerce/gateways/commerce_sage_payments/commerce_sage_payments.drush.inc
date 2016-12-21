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
      return dt('Generates sage payment tokens from an uploaded .csv file. Can be used with or without arguments. Missing arguments will be prompted for. After processing, payment API request results will be added to the original data, and saved in a new file, destination.csv.');
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
    'module' => 'commerce_sage_payments',
    'arguments' => array(
      'gateway_id' => "The gateway id. Ex: 'commerce_sage_payments_eft|commerce_payment_commerce_sage_payments_eft' Use quotes.",
      'path' => 'Absolute path to the .csv source file directory',
      'account_filename' => 'The customer account info .csv file, including extension.',
      'customer_code_filename' => 'The customer code file used for deduping, including extension.',
      'test_run' => 'TRUE or FALSE. If TRUE, API request will not be sent to Sage.',
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
 * @param string $account_filename
 *   The account_filename, including extension.
 * @param string $customer_code_filename
 *   The code_filename, including extension.
 * @param bool $test_run
 *   Whether to skip the call to Sage, TRUE or FALSE.
 */
function drush_commerce_sage_payments_sage_tokenizer($gateway_id = NULL, $path = NULL, $account_filename = NULL, $customer_code_filename = NULL, $test_run = NULL) {

  if (!isset($test_run)) {
    $options = array(
      TRUE => 'Yes',
      'No' => 'No',
    );
    $test_run = drush_choice($options, dt('Test run without a call to Sage?'), '!value');
    if (empty($test_run)) {
      // User chose "cancel'.
      drush_print('Operation cancelled.');
      return;
    }
    if ($test_run == 'No') {
      $test_run = FALSE;
    }
  }

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
    $path = drush_prompt("Enter the absolute path to the directory where the csv files are located.", NULL, TRUE);
  }

  if (empty($account_filename)) {
    $account_filename = drush_prompt("Enter the filename of the .csv file continaing the account information", NULL, TRUE);
  }

  if (empty($customer_code_filename)) {
    $customer_code_filename = drush_prompt("Enter the filename of the .csv file containing the customer code information", NULL, TRUE);
  }

  $account_source_file = $path . '/' . $account_filename;
  $code_source_file = $path . '/' . $customer_code_filename;

  if (!file_exists($account_source_file)) {
    drush_print('Cannot find your customer account info .csv file. Please check your arguments and try again.');
    return;
  }

  if (!file_exists($code_source_file)) {
    drush_print('Cannot find your customer code .csv file. Please check your arguments and try again.');
    return;
  }

  $accounts = commerce_sage_payments_load_accounts_from_csv($account_source_file);
  $codes = commerce_sage_payments_load_codes_from_csv($code_source_file);

  $chunks = array_chunk($accounts['accounts'], 1000);
  $operations = array();
  foreach ($chunks as $chunk) {
    $operations[] = array(
      'commerce_sage_payments_tokenizer_batch_process',
      array(
        $chunk,
        $gateway,
        $accounts['headers'],
        $path,
        $codes,
        $test_run,
      ),
    );
  }

  $batch = array(
    'operations' => $operations,
    'title' => t('Processing account tokens.'),
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
function commerce_sage_payments_tokenizer_batch_process($accounts, $gateway, $headers, $path, $codes, $test_run, &$context) {

  foreach ($accounts as $index => $account) {

    if (!in_array($account['CustomerCode'], $codes)) {
      unset($accounts[$index]);
      continue;
    }

    // Set default values for the new columns in the destination.csv file.
    $account['routing_number'] = 'fail';
    $account['account_number'] = 'fail';
    $account['account_type'] = 'fail';

    // Translate the source .csv file headers into the format Sage expects.
    if (isset($account['Account or Card Number'])) {
      $account['routing_number'] = substr($account['Account or Card Number'], 0, 8);
      $account['account_number'] = substr($account['Account or Card Number'], 9);
    }
    if (isset($account['AccountType'])) {
      if ($account['AccountType'] == 'SAVING') {
        $account['account_type'] = 'SAV';
      }
      if ($account['AccountType'] == 'CHECKING') {
        $account['account_type'] = 'DDA';
      }
    }

    $payment_details = array(
      'ROUTING_NUMBER' => $account['routing_number'],
      'ACCOUNT_NUMBER' => $account['account_number'],
      'C_ACCT_TYPE' => $account['account_type'],
    );

    if (!$test_run) {
      $result = commerce_sage_payments_vault_request_insert($gateway['gateway_details'], $payment_details);
    }

    if (!empty($result['GUID'])) {
      $accounts[$index]['guid'] = $result['GUID'];
    }
    else {
      $fail_guid = !empty($test_run) ? 'test_run' : '';
      $accounts[$index]['guid'] = $fail_guid;
    }

    $accounts[$index]['routing_number'] = $account['routing_number'];
    $accounts[$index]['account_number'] = $account['account_number'];
    $accounts[$index]['account_type'] = $account['account_type'];
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
    array_push($headers, 'payment_token');
    array_push($headers, 'routing_number');
    array_push($headers, 'account_number');
    array_push($headers, 'account_type');
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
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
      '%error_operation' => $error_operation[0],
      '@arguments' => print_r($error_operation[1], TRUE)
    ));
    drush_print($message);
  }
}


/**
 * @param path to file $path
 *
 * @return array
 */
function commerce_sage_payments_load_accounts_from_csv($path) {
  $headers = NULL;
  $accounts = array();
  if (($handle = fopen($path, 'r')) !== FALSE) {
    while (($row = fgetcsv($handle, 1000, ',')) !== FALSE) {
      if (empty($headers)) {
        $headers = $row;
      }
      else {
        $accounts[] = array_combine($headers, $row);
      }
    }
    fclose($handle);
  }

  return array('accounts' => $accounts, 'headers' => $headers);
}

/**
 * @param path to file $path
 *
 * @return array
 */
function commerce_sage_payments_load_codes_from_csv($path) {
  $codes = array();
  if (($handle = fopen($path, 'r')) !== FALSE) {
    while (($row = fgetcsv($handle, 1000, ',')) !== FALSE) {
      $codes[] = $row[0];
    }
    fclose($handle);
  }
  return $codes;
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
    if (strpos($id, 'sage_payments_eft') !== FALSE) {
      $fundraiser_gateways[$id] = $id;
    }
  }
  return $fundraiser_gateways;
}