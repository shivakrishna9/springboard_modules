<?php

/**
 * Implements hook_drush_command().
 */

function commerce_sage_payments_drush_help($command) {
  switch ($command) {
    case 'drush:sage-tokenizer':
      return dt('Generates sage payment tokens from an uploaded csv file.');
  }
}

function commerce_sage_payments_drush_command() {
  $items['sage-tokenizer'] = array(
    'description' => 'Obtain sage payment tokens using a source file',
    'arguments' => array(
      'gateway_id' => 'The gateway id',
      'path' => 'path to the source file directory',
      'filename' => 'the filename',
    ),
  );
  return $items;
}

function drush_commerce_sage_payments_sage_tokenizer($gateway_id = NULL, $path = NULL, $filename = NULL) {

  // Get all sage gateways.
  if (empty($gateway_id)) {
    $fundraiser_gateways = commerce_sage_payments_find_gateways();
    $gateway_id = drush_choice($fundraiser_gateways, dt('Choose the sage eeft gateway to use.'));
  }

  $gateway = _fundraiser_gateway_info($gateway_id);

  if (empty($gateway)) {
    drush_print('Gateway not found');
    $fundraiser_gateways = commerce_sage_payments_find_gateways();
    $gateway_id = drush_choice($fundraiser_gateways, dt('Choose the sage eeft gateway to use.'));
    $gateway = _fundraiser_gateway_info($gateway_id);
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

function commerce_sage_payments_tokenizer_batch_process_complete($success, $results, $operations) {

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

function commerce_Sage_payments_find_gateways() {
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